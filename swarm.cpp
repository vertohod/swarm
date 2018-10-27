#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include "defines_for_filter.h"
#include "get_remainder.h"
#include "swarm.h"
#include "alphabet.h"
#include "util.h"
#include "md5.h"
#include "hex.h"
#include <set>

namespace swarm
{

swarm::swarm(OID server_id, bool main, const std::string& address, const std::string& secret_key, const std::unordered_map<std::string, std::string>& upstreams) :
    m_server_id(server_id),
    m_main(main),
    m_main_server_id(0),
    m_remainder(0),
    m_secret_key(secret_key),
    m_commands_counter(0),
    m_divider(0),
    m_parts(0),
    m_stop_flag(false),
    m_upstreams(upstreams)
{
    lo::l(lo::DEBUG) << "swarm::swarm, add_table<server>";

    add_table<server>();

    lo::l(lo::DEBUG) << "swarm::swarm, listen: " << address;

    m_server.listen(address);
    m_thread_dispatcher = std::make_shared<std::thread>(std::bind(&swarm::thread_dispatcher, this));
    m_thread_manager = std::make_shared<std::thread>(std::bind(&swarm::thread_manager, this));

    if (m_main) {
        m_main_server_id = m_server_id;
        m_remainder = 0;
        m_divider = 1;
        m_parts = 1;
    }

    lo::l(lo::DEBUG) << "swarm::swarm, init";

    m_range.init(m_remainder, m_divider, m_parts);
}

swarm::~swarm()
{
    lo::l(lo::DEBUG) << "swarm::~swarm";

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_stop_flag = true;
        m_cv.notify_all();
    }

    m_server.stop();

    if (m_thread_manager) m_thread_manager->join();
    if (m_thread_dispatcher) m_thread_dispatcher->join();
}

void swarm::thread_dispatcher()
{
    lo::l(lo::DEBUG) << "swarm::thread_dispatcher";

    m_server.set_accept_handler(std::bind(&swarm::on_accept, this, std::placeholders::_1));
    m_server.set_connect_handler(std::bind(&swarm::on_connect, this, std::placeholders::_1, std::placeholders::_2));
    m_server.set_close_handler(std::bind(&swarm::on_close, this, std::placeholders::_1));
    m_server.set_receive_handler(std::bind(&swarm::on_receive, this, std::placeholders::_1, std::placeholders::_2));
    m_server.set_send_handler(std::bind(&swarm::on_send, this, std::placeholders::_1));
    m_server.set_wait_handler(std::bind(&swarm::on_wait, this, std::placeholders::_1));

    m_server.run();
}

void swarm::thread_manager()
{
    lo::l(lo::DEBUG) << "swarm::thread_manager";

    while (!m_stop_flag) {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (m_cv.wait_for(lock, std::chrono::milliseconds(1000), [this]{ return m_stop_flag; })) return; 
        }
        for (auto& upstream_pr : m_upstreams) {
            std::lock_guard<std::mutex> lock(m_mutex_connections);
            auto it = m_upstream_socket_id.find(upstream_pr.second);
            if (it != m_upstream_socket_id.end()) continue;

            m_server.connect(upstream_pr.second, 5000);
        }
    }
}

OID swarm::get_commands_number()
{
    return std::atomic_fetch_add(&m_commands_counter, static_cast<OID>(1));
}

void swarm::on_accept(int socket_id)
{
    lo::l(lo::DEBUG) << "swarm::on_accept, socket_id: " << socket_id;

    m_server.receive(socket_id);

    auto get_salt_ptr = get_salt::create();
    auto command_ptr = command::create();
    command_ptr->set_id(get_commands_number());
    command_ptr->set_pl(get_salt_ptr);
    send(socket_id, command_ptr);
}

void swarm::on_connect(std::string address, int socket_id)
{
    lo::l(lo::DEBUG) << "swarm::on_connect, socket_id: " << socket_id;

    m_server.receive(socket_id);

    {
        std::lock_guard<std::mutex> lock(m_mutex_connections);
        m_upstream_socket_id.insert(std::make_pair(address, socket_id));
        m_socket_id_upstream.insert(std::make_pair(socket_id, address));
    }

    auto get_salt_ptr = get_salt::create();
    auto command_ptr = command::create();
    command_ptr->set_id(get_commands_number());
    command_ptr->set_pl(get_salt_ptr);
    send(socket_id, command_ptr);
}

void swarm::on_close(int socket_id)
{
    lo::l(lo::DEBUG) << "swarm::on_close, socket_id: " << socket_id;

    // Удаляем линки socket_id <--> адрес
    {
        std::lock_guard<std::mutex> lock(m_mutex_connections);
        auto it = m_socket_id_upstream.find(socket_id);
        if (it != m_socket_id_upstream.end()) {
            auto it2 = m_upstream_socket_id.find(it->second);
            if (it2 != m_upstream_socket_id.end()) m_upstream_socket_id.erase(it2);
            m_socket_id_upstream.erase(it);
        }
    }

    m_send_buffer.remove(socket_id);
}

void swarm::on_receive(int socket_id, const std::vector<unsigned char>& data)
{
    lo::l(lo::DEBUG) << "swarm::on_receive, socket_id: " << socket_id;
    std::string data_str(data.begin(), data.end());

    auto object_name = object::get_field(data_str, FIELD_OBJECT_TYPE);

    if (object_name->empty()) {
        lo::l(lo::ERROR) << "(swarm::on_receive) Were received unknown JSON: " << data_str;
        return;
    } else {
        lo::l(lo::DEBUG) << "swarm::on_receive, data: " << data_str;
    }

    try {
        auto object_ptr = object::create_object(object_name);
        object_ptr->deserialization(data_str);
        process(socket_id, object_ptr);
    } catch (const std::exception& e) {
        lo::l(lo::ERROR) << "(swarm::on_receive): " << e.what();
    }

    m_server.receive(socket_id);
}

void swarm::on_send(int socket_id)
{
    lo::l(lo::DEBUG) << "swarm::on_send, socket_id: " << socket_id;

    m_send_buffer.set_flag(socket_id);

    send(socket_id);
}

void swarm::on_wait(int socket_id)
{
    lo::l(lo::DEBUG) << "swarm::on_wait, socket_id: " << socket_id;
}

table_interface* swarm::table(const std::string& table_name)
{
    auto it = m_table_map.find(table_name);
    if (it == m_table_map.end()) throw std::runtime_error("The table '" + table_name + "' wasn't added");
    return it->second.get();
}

OID swarm::insert(std::shared_ptr<const object> object_ptr, std::function<void(OID oid)> fn)
{
    OID res = 0;

    auto table_ptr = table(object_ptr->tp());
    // Если нет уникальных ключей, просто делаем вставку без каких-либо проверок
    if (!table_ptr->get_unique_keys_flag()) {
        res = table_ptr->insert(object_ptr);
        if (fn != nullptr) fn(res);
        shard(object_ptr);
        return res;
    }

    // Если есть уникальные ключи, необходимо блокировать таблицу
    // Не требуется синхронизация, делаем вставку только локально
    if (object_ptr->settings() & BIT_SYNCHRONIZE == 0) {
        table_ptr->lock(0);
        res = table_ptr->insert(object_ptr);
        table_ptr->unlock(0);
        return res;
    }


    if (table_ptr->get_unique_keys_flag()) {
        if (object_ptr->settings() & BIT_SYNCHRONIZE != 0) {
            // Блокируем всю таблицу (oid = 0)
            _lock(object_ptr->tp(), 0, 
                [this, table_ptr, object_ptr, fn](bool lock_ok){
                    // Блокировка прошла успешно?
                    if (lock_ok) {
                        // Делаем вставку записи
                        _insert(object_ptr, fn);
                        auto oid = table_ptr->insert(object_ptr);
                        if (fn != nullptr) fn(oid);
                        // Делаем разблокировку таблиц на всех серверах
                        unlock(object_ptr->tp(), 0);
                    } else {
                        if (fn != nullptr) fn(0);
                    }
                }
            );
        } else {
            table_ptr->lock_block(0);
            res = table_ptr->insert(object_ptr);
            table_ptr->unlock(0);
        }
    } else {
        res = table_ptr->insert(object_ptr);
        if (fn != nullptr) fn(res);
        shard(object_ptr);
    }

    return res;
}

bool swarm::update(OID index, std::shared_ptr<const object> object_ptr, std::function<void(bool flag)> fn)
{
    bool res = false;

    auto table_ptr = table(object_ptr->tp());
    if (table_ptr->get_unique_keys_flag()) {
        // Блокируем всю таблицу (oid = 0)
        _lock(object_ptr->tp(), 0, 
            [table_ptr, index, object_ptr, fn](bool lock_ok){
                if (lock_ok) {
                    bool flag = table_ptr->update(index, object_ptr);
                    if (fn != nullptr) fn(flag);
                } else {
                    if (fn != nullptr) fn(false);
                }
            }
        );
    } else {
        res = table_ptr->update(index, object_ptr);
        if (fn != nullptr) fn(res);
        shard(object_ptr);
    }

    return res;
}

void swarm::shard(std::shared_ptr<const object> object_ptr)
{
    // TODO
}

void swarm::process(int socket_id, std::shared_ptr<object>& object_ptr)
{
    OID server_id = 0;
    {
        auto key_ptr = key_server_socket::create();
        key_ptr->set_socket_id(socket_id);
        auto answer_ptr = find(key_ptr);
        if (answer_ptr->get_result()->size() == 1) {
            auto record_ptr = record::convert(answer_ptr->get_result()->front());
            auto server_ptr = server::convert(record_ptr->get_payload());
            server_id = server_ptr->get_id();
        }
    }

    try {
        if (object_ptr->tp() == command::stp()) {

            auto response_ptr = response::create();
            auto command_ptr = std::dynamic_pointer_cast<command>(object_ptr);
            response_ptr->set_command_id(command_ptr->get_id());
            auto payload_ptr = command_ptr->get_pl();
            if (payload_ptr->tp() == authentication::stp()) {
                auto ptr = std::dynamic_pointer_cast<authentication>(payload_ptr);
                process(socket_id, server_id, ptr, response_ptr);
            } else if (payload_ptr->tp() == get_servers_list::stp()) {
                auto ptr = std::dynamic_pointer_cast<get_servers_list>(payload_ptr);
                process(socket_id, server_id, ptr, response_ptr);
            } else if (payload_ptr->tp() == get_salt::stp()) {
                auto ptr = std::dynamic_pointer_cast<get_salt>(payload_ptr);
                process(socket_id, server_id, ptr, response_ptr);
            }

	        send(socket_id, response_ptr);

        } else if (object_ptr->tp() == response::stp()) {

            auto response_ptr = std::dynamic_pointer_cast<response>(object_ptr);
            for (auto& result_ptr : *response_ptr->get_result()) {

                if (result_ptr->tp() == salt::stp()) {
                    auto ptr = salt::convert(result_ptr);
                    process(socket_id, server_id, ptr);
                } else if (result_ptr->tp() == server_short::stp()) {
                    auto ptr = server_short::convert(result_ptr);
                    process(socket_id, server_id, ptr);
                }
            }

        } else if (object_ptr->tp() == packet::stp()) {

            auto packet_ptr = std::dynamic_pointer_cast<packet>(object_ptr);
            m_router.add_route(packet_ptr->get_route());

            if (packet_ptr->get_server_recipient_id() == m_server_id || packet_ptr->get_remainder_recipient() == m_remainder) {

                for (auto& payload_ptr : *packet_ptr->get_payload()) {

                    if (payload_ptr->tp() == get_remainder::stp()) {
                        auto ptr = std::dynamic_pointer_cast<get_remainder>(payload_ptr);
                        process(ptr);
                    } else if (payload_ptr->tp() == lock::stp()) {
                        auto ptr = std::dynamic_pointer_cast<lock>(payload_ptr);
                        process(packet_ptr->get_id(), ptr);
                    } else if (payload_ptr->tp() == result::stp()) {
                        auto ptr = std::dynamic_pointer_cast<result>(payload_ptr);
                        process(ptr);
                    }
                }
            } else {
                insert(packet_ptr);
            }
        }
    } catch (const std::exception& e) {
        lo::l(lo::ERROR) << e.what();
    }
}

void swarm::process(int socket_id, OID server_id, std::shared_ptr<get_salt>& object_ptr, std::shared_ptr<response>& response_ptr)
{
    auto key_ptr = key_server_socket::create();
    key_ptr->set_socket_id(socket_id);
    auto answer_ptr = find(key_ptr);

    sptr_cstr salt_str;
    if (answer_ptr->get_result()->size() == 0) {
        auto server_ptr = server::create();
        server_ptr->set_socket_id(socket_id);
        salt_str = util::get_random_string(ALPH_ENGLISH ALPH_NUMBERS, 100);
        server_ptr->set_salt(salt_str);
        auto oid = insert(server_ptr);
        if (oid == 0) {
            response_ptr->set_code(SMESS001_ERROR);
            response_ptr->set_description(smessage(SMESS001_ERROR));
            return;
        }
    } else {
        auto server_ptr = server::convert(answer_ptr->get_result()->front()->get_payload());
        salt_str = server_ptr->get_salt();
    }
    auto salt_ptr = salt::create();
    salt_ptr->set_value(salt_str);
    response_ptr->get_result()->push_back(salt_ptr);
}

void swarm::process(int socket_id, OID server_id, std::shared_ptr<authentication>& object_ptr, std::shared_ptr<response>& response_ptr)
{
    auto key_ptr = key_server_socket::create();
    key_ptr->set_socket_id(socket_id);
    auto answer_ptr = find(key_ptr);

    if (answer_ptr->get_result()->size() == 1) {
        auto index = answer_ptr->get_result()->front()->get_index();
        auto server_ptr = server::convert(answer_ptr->get_result()->front()->get_payload());
        auto salt = server_ptr->get_salt();

        if (md5(m_secret_key + *salt) == object_ptr->get_secret_key_hash()) {
            auto server_new = std::dynamic_pointer_cast<server>(server_ptr->make_copy());
            server_new->set_id(object_ptr->get_server_id());
            update(index, server_new);

            auto get_servers_list_ptr = get_servers_list::create();
            auto command_ptr = command::create();
            command_ptr->set_id(get_commands_number());
            command_ptr->set_pl(get_servers_list_ptr);
            send(socket_id, command_ptr);
        }
    }
}

void swarm::process(int socket_id, OID server_id, std::shared_ptr<get_servers_list>& object_ptr, std::shared_ptr<response>& response_ptr)
{
    auto answer_ptr = get_with_limit<server>(0, 0);
    for (auto& record_ptr : *answer_ptr->get_result()) {
        auto server_ptr = server::convert(record_ptr->get_payload());
        auto server_short_ptr = server_short::create();

        // Не сообщаем серверу о нем самом
        if (server_ptr->get_id() == server_id) continue;

        server_short_ptr->set_id(server_ptr->get_id());
        server_short_ptr->set_remainder(server_ptr->get_remainder());
        server_short_ptr->set_time(server_ptr->get_time());
        server_short_ptr->set_main(server_ptr->get_main());
        response_ptr->get_result()->push_back(server_short_ptr);
    }

    auto server_short_ptr = server_short::create();
    server_short_ptr->set_id(m_server_id);
    server_short_ptr->set_remainder(m_remainder);
    server_short_ptr->set_time(time(NULL));
    server_short_ptr->set_main(m_main);
    response_ptr->get_result()->push_back(server_short_ptr);
}

void swarm::process(int socket_id, OID server_id, std::shared_ptr<const salt>& object_ptr)
{
    auto authentication_ptr = authentication::create();
    authentication_ptr->set_server_id(m_server_id);
    authentication_ptr->set_remainder(m_remainder);
    authentication_ptr->set_secret_key_hash(md5(m_secret_key + *object_ptr->get_value()));
    auto command_ptr = command::create();
    command_ptr->set_id(get_commands_number());
    command_ptr->set_pl(authentication_ptr);
    send(socket_id, command_ptr);
}

void swarm::process(int socket_id, OID server_id, std::shared_ptr<const server_short>& object_ptr)
{
    // На всякий случай исключаем возможность записать в базу о себе
    if (object_ptr->get_id() == m_server_id) return;

    if (object_ptr->get_main()) {
        if (m_main_server_id != 0 && m_main_server_id != object_ptr->get_id()) return;
        m_main_server_id = object_ptr->get_id();
    }

    auto key_ptr = key_server_server::create();
    key_ptr->set_id(object_ptr->get_id());
    auto answer_ptr = find(key_ptr);
    if (answer_ptr->get_result()->size() > 0) {
        auto record_ptr = answer_ptr->get_result()->front();
        auto server_ptr = server::convert(record_ptr->get_payload());
        if (server_ptr->get_time() < object_ptr->get_time()) {
            auto index = record_ptr->get_index();
            auto new_server_ptr = std::dynamic_pointer_cast<server>(server_ptr->make_copy());
            new_server_ptr->set_remainder(object_ptr->get_remainder());
            new_server_ptr->set_time(object_ptr->get_time());
            new_server_ptr->set_main(object_ptr->get_main());
            update(index, new_server_ptr);
        }
    } else {
        auto server_ptr = server::create();
        server_ptr->set_id(object_ptr->get_id());
        server_ptr->set_remainder(object_ptr->get_remainder());
        server_ptr->set_time(object_ptr->get_time());
        server_ptr->set_main(object_ptr->get_main());
        insert(server_ptr);
    }

    if (m_main_server_id != 0) {
        auto get_remainder_ptr = get_remainder::create();
        auto packet_ptr = packet::create();
        packet_ptr->set_id(get_commands_number());
        packet_ptr->set_server_id(m_server_id);
        packet_ptr->set_server_recipient_id(m_main_server_id);
        packet_ptr->get_payload()->push_back(get_remainder_ptr);
        insert(packet_ptr);
    }
}

void swarm::process(std::shared_ptr<get_remainder>& object_ptr)
{
    auto lock_ptr = lock::create();
    auto packet_ptr = packet::create();
    packet_ptr->set_id(get_commands_number());
    packet_ptr->set_server_id(m_server_id);
    packet_ptr->set_server_recipient_id(m_main_server_id);
    packet_ptr->get_payload()->push_back(lock_ptr);
    insert(packet_ptr);
}

void swarm::process(OID packet_id, std::shared_ptr<lock>& object_ptr)
{
    auto result_ptr = result::create();
    result_ptr->set_packet_id(packet_id);
    // 0 - все в порядке, иначе код ошибки
    result_ptr->set_code(lock_record(object_ptr->get_table_name(), object_ptr->get_oid()) ? 0 : 1);
    auto packet_ptr = packet::create();
    packet_ptr->set_id(get_commands_number());
    packet_ptr->set_server_id(m_server_id);
    packet_ptr->set_server_recipient_id(m_main_server_id);
    packet_ptr->get_payload()->push_back(result_ptr);
    insert(packet_ptr);
}

void swarm::process(std::shared_ptr<result>& object_ptr)
{
    auto transaction_ptr = m_transactions_in_processing.get_transaction(object_ptr->get_transaction_id());
    auto task_ptr = transaction_ptr->get_task(object_ptr->get_task_id());
    task_ptr->confirm(object_ptr->server_id());

    if (task_ptr->is_finished()) {
        if (transaction_ptr->is_finished()) {
            transaction_ptr->fn(true);
            m_transactions_in_processing.remove_transaction(transaction_ptr->get_id());
        } else {
            send_packets(transaction_ptr);
        }
    } else {
        if (task_ptr->is_ready()) send_packets(transaction_ptr);
    }
}

void swarm::lock_table(std::shared_ptr<const object> object_ptr, function_t fn)
{
    auto table_ptr = table(object_ptr->tp());

    if (table_ptr->get_unique_keys_flag() && (object_ptr->settings() & BIT_SYNCHRONIZE != 0)) {

        auto lock_ptr = lock::create(object_ptr->tp());

        auto packet_ptr = packet::create();
        packet_ptr->set_id(get_commands_number());
        packet_ptr->set_server_id(m_server_id);
        packet_ptr->get_payload()->push_back(lock_ptr);

        insert(packet_ptr, true, fn);
    }
}
/*
void swarm::lock_table(sptr_cstr table_name, OID oid, function_t fn)
{
    // Заблокировали таблицу/запись локально
    table(table_name)->lock(oid);

    auto lock_ptr = lock::create();
    lock_ptr->set_table_name(table_name_temp);
    lock_ptr->set_oid(oid);

    auto packet_ptr = packet::create();
    packet_ptr->set_id(get_commands_number());
    packet_ptr->set_server_id(m_server_id);
    packet_ptr->get_payload()->push_back(lock_ptr);

    // Отправили пакет для блокировки таблицы/записи на других серверах
    insert(packet_ptr, true, fn);
}
*/

/*
bool swarm::lock_table(sptr_cstr table_name)
{
    return table(table_name)->lock();
}

bool swarm::lock_record(sptr_cstr table_name, OID oid)
{
    return table(table_name)->lock_record(oid);
}
*/

void swarm::unlock(sptr_cstr table_name, OID oid)
{
    function_t fn = [this, table_name, oid](bool unlock_ok){ table(*table_name)->unlock(oid); };

    auto lock_ptr = unlock::create();
    lock_ptr->set_table_name(table_name);
    lock_ptr->set_oid(oid);

    auto packet_ptr = packet::create();
    packet_ptr->set_id(get_commands_number());
    packet_ptr->set_server_id(m_server_id);
    packet_ptr->get_payload()->push_back(lock_ptr);

    // Отправили пакет для разблокировки таблицы/записи на других серверах
    insert(packet_ptr, true, fn);
}

OID swarm::get_new_remainder(OID server_id, OID remainder)
{
    OID res = 0;
    // проверка в базе
    {
        auto key_ptr = key_server_server::create();
        key_ptr->set_id(server_id);
        auto answer_ptr = find(key_ptr);
        if (answer_ptr->get_result()->size() == 1) {
            auto server_ptr = server::convert(answer_ptr->get_result()->front()->get_payload());
            if (server_ptr->get_remainder() == remainder) return remainder;
        }
    }
    // поиск свободного или генерация нового
    {
        std::set<OID> remainders;
        auto answer_ptr = get_with_limit<server>(0, 0);
        for (auto& record_ptr : *answer_ptr->get_result()) {
            remainders.insert(server::convert(record_ptr->get_payload())->get_remainder());
        }
        for (auto val : remainders) if (val != res++) { --res; break; }
    }
    return res;
}

sptr_cstr swarm::md5(const std::string& data)
{
    sptr_str result;

    byte digest[CryptoPP::Weak::MD5::DIGESTSIZE];
    CryptoPP::Weak::MD5 hash;
    hash.CalculateDigest(digest, reinterpret_cast<const byte*>(data.c_str()), data.length());
    CryptoPP::HexEncoder encoder;
    encoder.Attach(new CryptoPP::StringSink(*result));
    encoder.Put(digest, sizeof(digest));
    encoder.MessageEnd();

    return result;
}

void swarm::send(int socket_id)
{
    auto data = m_send_buffer.pop(socket_id);

    if (!data->empty()) _send(socket_id, data);
}

void swarm::_send(int socket_id, sptr_cstr data)
{
    std::vector<unsigned char> temp(data->begin(), data->end());
    m_server.send(socket_id, temp);
}

std::unordered_set<OID> swarm::get_recipient_servers(OID remainder)
{
    std::unordered_set<OID> recipient_servers;

    auto remainder_range = m_range.get_remainders(remainder);
    for (auto temp : remainder_range) {
        auto key_ptr = key_server_remainder::create();
        key_ptr->set_remainder(temp);
        auto answer_ptr = find(key_ptr);
        for (auto& result_ptr : *answer_ptr->get_result()) {
            recipient_servers.insert(server::convert(result_ptr->get_payload())->get_id());
        }
    }

    return recipient_servers;
}

std::vector<std::pair<OID, OID>> swarm::get_gateaways(std::unordered_set<OID>& recipient_servers)
{
    // <server_id, gateaway>
    std::vector<std::pair<OID, OID>> gateaways;

    for (auto server_id : recipient_servers) {
        auto gateaway = m_router.get_route(server_id);
        if (gateaway == 0) {
            // Если для одного из серверов неизвестен шлюз, делаем широковещательную отправку
            auto answer_ptr = get_with_limit<server>(0, 0, [](const object& obj){FROM(server) WHERE(FL(socket_id) != 0)});
            for (auto& record_ptr : *answer_ptr->get_result()) {
                auto temp = server::convert(record_ptr->get_payload())->get_id();
                gateaways.push_back(std::make_pair(server_id, temp));
            }
        } else gateaways.push_back(std::make_pair(gateaway, server_id));
    }

    return gateaways;
}

void swarm::insert(std::shared_ptr<const object> object_ptr, function_t fn)
{
    OID remainder = 0; // TODO

    auto recipient_servers = get_recipient_servers(remainder);
    auto gateaways = get_gateaways(recipient_servers);

    auto transaction_ptr = std::make_shared<transaction>(fn);

    // lock
    {
        auto packet_ptr = packet::create();
        auto lock_ptr = lock::create();
        lock_ptr->set_table_name(object_ptr->tp());
        packet_ptr->get_payload()->push_back(lock_ptr);
        transaction_ptr->create_inorder_task(packet_ptr, gateaways);
    }

    // insert
    {
        auto packet_ptr = packet::create();
        auto insert_ptr = insert::create();
        insert_ptr->set_payload(object_ptr);
        packet_ptr->get_payload()->push_back(insert_ptr);
        transaction_ptr->create_parallel_task(packet_ptr, gateaways);
    }

    // unlock
    {
        auto packet_ptr = packet::create();
        auto unlock_ptr = unlock::create();
        unlock_ptr->set_table_name(object_ptr->tp());
        packet_ptr->get_payload()->push_back(unlock_ptr);
        transaction_ptr->create_parallel_task(packet_ptr, gateaways);
    }

    begin_transaction(transaction_ptr);
}

void swarm::begin_transaction(std::shared_ptr<transactin> transaction_ptr)
{
    m_transactions_in_processing.add_transaction(transaction_ptr);
    send_packets(transaction_ptr);
}

void swarm::send_packets(std::shared_ptr<transactin> transaction_ptr)
{
    auto packets_list = transaction_ptr->get_packets();
    for (auto packet_ptr : *packets_list) {
        send(packet_ptr->get_recipient_id(), packet_ptr->serializate()); 
    }
}

bool swarm::send_buffer::empty(int socket_id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_buffer.find(socket_id);
    if (it != m_buffer.end()) return it->second.second.empty();
    return true;
}

void swarm::send_buffer::push(int socket_id, sptr_cstr data)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_buffer.find(socket_id);
    if (it == m_buffer.end()) {
        auto pr = m_buffer.insert(std::make_pair(socket_id, std::make_pair(false, std::queue<sptr_cstr>())));
        if (pr.second) pr.first->second.second.push(data);
        else throw std::runtime_error("Can't insert structure for keeping datas");
    } else {
        it->second.first = false;
        it->second.second.push(data);
    }
}

void swarm::send_buffer::set_flag(int socket_id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_buffer.find(socket_id);
    if (it != m_buffer.end()) {
        if (it->second.second.empty()) it->second.first = true;
    }
}

void swarm::send_buffer::unset_flag(int socket_id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_buffer.find(socket_id);
    if (it == m_buffer.end()) {
        m_buffer.insert(std::make_pair(socket_id, std::make_pair(false, std::queue<sptr_cstr>())));
    } else {
        it->second.first = false;
    }
}

bool swarm::send_buffer::check_flag(int socket_id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_buffer.find(socket_id);
    if (it != m_buffer.end()) return  it->second.first;
    return true;
}

sptr_cstr swarm::send_buffer::pop(int socket_id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_buffer.find(socket_id);
    if (it != m_buffer.end()) {
        if (!it->second.second.empty()) {
            auto data = it->second.second.front();
            it->second.second.pop();
            return data;
        }
    }
    return "";
}

void swarm::send_buffer::remove(int socket_id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_buffer.find(socket_id);
    if (it != m_buffer.end()) m_buffer.erase(it);
}

// --------------- transaction --------------------

swarm::task::task(bool parallel) : m_parallel(parallel), m_index(0)
{
    m_packets = std::make_shared<packets_t>();
}

void swarm::task::set_packet(std::shared_ptr<packet> packet_ptr, std::set<std::pair<OID, OID>>& servers)
{
    for (auto& pr : servers) {
        auto packet_new_ptr = packet_ptr->make_copy();
        packet_new_ptr->set_gateaway_id(pr.first);
        packet_new_ptr->set_recipient_id(pr.second);
        m_packets->push_back(packet_new_ptr);
    }
}

std::shared_ptr<packets_t> swarm::get_packets()
{
    std::shared_ptr<packets_t> res;

    if (m_parallel) {
        res = m_packets;
        for (auto packet_ptr : *m_packets) {
            m_need_confirmation.insert(packet_ptr->get_recipient_id());
        }
    } else {
        res = std::make_shared<packets_t>();
        if (m_index < m_packets->size()) {
            auto packet_ptr = m_packets->at(m_index++);
            res->push_back(packet_ptr );
            m_need_confirmation.insert(packet_ptr->get_recipient_id());
        }
    }

    return res;
}

bool swarm::is_ready()
{
    return m_need_confirmation.empty();
}

bool swarm::is_finished()
{
    return is_ready() && m_index == m_packets->size();
}

void swarm::confirm(OID server_id)
{
    m_need_confirmation.erase(server_id);
    if (!m_parallel) ++m_index;
}

transaction::transaction(size_t number) : m_number(number), m_index(0) {}

std::shared_ptr<task::packets_t> transaction::get_packets()
{
    auto task_ptr = m_tasks[m_index];
    if (task_ptr->is_finished() && (m_index + 1) < m_tasks.size()) {
        task_ptr = m_tasks[++m_index];
    }
    return task_ptr->get_packets(); 
}

bool transaction::is_finished()
{
    if (m_tasks.size() == 0) return true;

    return ((m_index + 1) == m_tasks.size() && m_tasks[m_index]->is_finished());
}

void transaction::fn(bool ok)
{
    m_fn(ok);
}

} // end of namespace
