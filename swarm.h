#ifndef SWARM_H
#define SWARM_H

#include <condition_variable>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <memory>
#include <vector>
#include <thread>
#include <atomic>
#include <string>
#include <mutex>
#include <queue>
#include <list>

#include "transaction_in_processing.h"
#include "key_store_interface.h"
#include "get_servers_list.h"
#include "socket_service.h"
#include "authentication.h"
#include "get_remainder.h"
#include "server_short.h"
#include "transaction.h"
#include "send_buffer.h"
#include "response.h"
#include "get_salt.h"
#include "command.h"
#include "packet.h"
#include "socket.h"
#include "unlock.h"
#include "server.h"
#include "object.h"
#include "answer.h"
#include "router.h"
#include "result.h"
#include "insert.h"
#include "types.h"
#include "table.h"
#include "range.h"
#include "salt.h"
#include "lock.h"
#include "task.h"

#define TIMEOUT = 500; // milliseconds

namespace swarm
{

class swarm
{
private:
    typedef std::unordered_map<std::string, std::shared_ptr<table_interface>> table_map_t;
    table_map_t                         m_table_map;
    table_map_t                         m_cache_map;
    std::shared_ptr<std::thread>        m_thread_dispatcher;
    std::shared_ptr<std::thread>        m_thread_manager;
    net::socket_service<net::socket>    m_server;

    OID                                 m_server_id;
    bool                                m_main;
    OID                                 m_main_server_id;
    OID                                 m_remainder;
    std::string                         m_secret_key;
    std::atomic<OID>                    m_commands_counter;

    OID                                 m_divider;
    OID                                 m_parts;
    range                               m_range;

    router                              m_router;

    typedef const std::unordered_map<std::string, std::string> upstreams_t;
    std::mutex                              m_mutex_connections;
    std::unordered_map<std::string, int>    m_upstream_socket_id;
    std::unordered_map<int, std::string>    m_socket_id_upstream;
    bool                                    m_stop_flag;
    upstreams_t                             m_upstreams;

    typedef std::function<void(bool)> function_t;

    std::mutex                              m_mutex;
    std::condition_variable                 m_cv;

    send_buffer                             m_send_buffer;
    transactions_in_processing              m_transactions_in_processing;

private:
    void thread_dispatcher();
    void thread_manager();
    void on_accept(int socket_id);
    void on_connect(std::string address, int socket_id);
    void on_close(int socket_id);
    void on_receive(int socket_id, const std::vector<unsigned char>& receive);
    void on_send(int socket_id);
    void on_wait(int socket_id);

public:
    swarm(OID server_id, bool main, const std::string& address, const std::string& secret_key, const std::unordered_map<std::string, std::string>& upstreams);
    ~swarm();

    template <typename T>
    inline void add_table()
    {
        auto it = m_table_map.find(T::stp());
        if (it == m_table_map.end()) {
            auto pr = m_table_map.insert(std::make_pair(T::stp(), std::make_shared<::swarm::table<T>>(m_server_id)));
            if (pr.second) it = pr.first;
            else throw std::runtime_error("Error during adding storage with name: " + T::stp());
        } else throw std::runtime_error("The storage with such name exist already (" + T::stp() + ")");
    }

    table_interface* table(const std::string& table_name);

    OID insert(std::shared_ptr<const object> object_ptr, function_t fn = nullptr);
    OID insert(std::shared_ptr<const object> object_ptr, std::function<void(OID oid)> fn = nullptr);

    bool update(OID index, std::shared_ptr<const object> object_ptr, std::function<void(bool flag)> fn = nullptr);

    template <typename T>
    bool remove(OID index)
    {
        auto res = table(T::stp())->remove(index);
        //shard(object);

        return res;
    }

    template <typename T>
    std::shared_ptr<const answer> get(OID oid)
    {
        return table(T::stp())->get(oid);
    }

    template <typename T>
    std::shared_ptr<const object> get_object(OID oid)
    {
        return table(T::stp())->get_object(oid);
    }

    template <typename T>
    std::shared_ptr<const answer> get_with_limit(size_t start, size_t limit, std::function<bool(const object&)> where = [](const object&){return true;})
    {
        return table(T::stp())->get_with_limit(where, start, limit);
    }

    std::shared_ptr<const answer> find(std::shared_ptr<const key_interface> key_ptr, size_t start = 0, size_t limit = 0, std::function<bool(const object&)> where = [](const object&){return true;})
    {
        return table(key_ptr->table_name())->find(key_ptr, start, limit, where);
    }

    std::shared_ptr<const answer> find_range(std::shared_ptr<const key_interface> lower_ptr, std::shared_ptr<const key_interface> upper_ptr, size_t start, size_t limit, std::function<bool(const object&)> where = [](const object&){return true;})
    {
        std::string table_name = (lower_ptr) ? lower_ptr->table_name() : ((upper_ptr) ? upper_ptr->table_name() : "null");
        return table(table_name)->find_range(lower_ptr, upper_ptr, start, limit, where);
    }

    template <typename T>
    size_t size()
    {
        return table(T::stp())->size();
    }
    // ---------

private:
    sptr_cstr md5(const std::string& data);
    void send(int socket_id, std::shared_ptr<object> object_ptr);
    void send(int socket_id);
    void _send(int socket_id, sptr_cstr data);
    OID _insert(std::shared_ptr<const object> object_ptr, std::function<void(OID oid)> fn);
    void insert(std::shared_ptr<packet> packet_ptr, bool need_confirmation = false, bool in_order = false, function_t fn = nullptr);
    void shard(std::shared_ptr<const object> object_ptr);
    OID get_new_remainder(OID server_id, OID remainder);
    void process(int socket_id, std::shared_ptr<object>& object_ptr);
    OID get_commands_number();

    void _lock(const std::string& table_name, OID oid, function_t fn = nullptr);
    // Пытается заблокировать всю таблицу
    void lock_table(std::shared_ptr<const object> object_ptr, function_t fn);
    void lock_table(sptr_cstr table_name, OID oid, function_t fn);
    bool lock_table(sptr_cstr table_name);
    // Пытается заблокировать запись в таблице
    bool lock_record(sptr_cstr table_name, OID oid);
    // Блокирует либо таблицу, либо запись. В случае неудачи ждет разблокировки.
    void lock_block(const std::string& table_name, OID oid = 0);
    void unlock(sptr_cstr table_name, OID oid = 0);

    void process(int socket_id, OID server_id, std::shared_ptr<get_salt>& object_ptr, std::shared_ptr<response>& response_ptr);
    void process(int socket_id, OID server_id, std::shared_ptr<authentication>& object_ptr, std::shared_ptr<response>& response_ptr);
    void process(int socket_id, OID server_id, std::shared_ptr<get_servers_list>& object_ptr, std::shared_ptr<response>& response_ptr);

    void process(int socket_id, OID server_id, std::shared_ptr<const salt>& object_ptr);
    void process(int socket_id, OID server_id, std::shared_ptr<const server_short>& object_ptr);

    void process(std::shared_ptr<get_remainder>& object_ptr);
    void process(OID packet_id, std::shared_ptr<lock>& object_ptr);
    void process(std::shared_ptr<result>& object_ptr);

    void begin_transaction(std::shared_ptr<transaction> transaction_ptr);
    void send_packets(std::shared_ptr<transaction> transaction_ptr);
    std::unordered_set<OID> get_recipient_servers(OID remainder);
    std::vector<std::pair<OID, OID>> get_gateaways(std::unordered_set<OID>& recipient_servers);
};

} // end of namespace
#endif
