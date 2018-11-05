#ifndef TABLE_H
#define TABLE_H

#include <condition_variable>
#include <unordered_set>
#include <functional>
#include <random>
#include <memory>
#include <mutex>
#include <list>

#include "service_messages.h"
#include "table_interface.h"
#include "key_interface.h"
#include "object.h"
#include "record.h"
#include "answer.h"
#include "format.h"
#include "types.h"
#include "log.h"

namespace swarm
{

// Наследуется всеми таблицами. По умолчанию не имеет ключей,
// но производные таблицы могут иметь любое количество ключей и должны обрабатывать их
template <typename T>
class table : public table_interface
{
private:
    enum state_t
    {
        START   = 0,
        LOAD    = 1,
        SEND    = 2,
        READY   = 3,
    };

    // Индексы должны быть уникальными
    typedef std::unordered_map<OID, std::shared_ptr<record>> object_store_t;

    object_store_t      m_object_store; // основное хранилище
    keys_stores_t       m_keys_stores;
    const size_t        m_settings;
    state_t             m_state;
    const std::string&  m_name;
    bool                m_unique_keys_exist_flag;

    std::mutex          m_mutex;
    bool                m_event_unlock;

    std::unordered_set<OID> m_locked_records;
    std::condition_variable m_cv;

public:
    table(OID server_id) :
        m_name(T::stp()),
        m_settings(T::ssettings()),
        m_state(START),
        m_unique_keys_exist_flag(false)
    {
        init_keys();
    }

private:
    void init_keys()
    {
        m_unique_keys_exist_flag = T::init_keys(m_keys_stores);
    }

    bool keys_insert(std::shared_ptr<record> record_ptr, keys_stores_t::iterator key_it)
    {
        if (key_it == m_keys_stores.end()) return true;

        bool res = false;
        if (key_it->second->add(record_ptr)) {
            auto next_it = key_it;
            if (keys_insert(record_ptr, ++next_it)) {
                key_it->second->commit();
                res = true;
            } else {
                key_it->second->rollback();
            }
        }
        lo::l(lo::TRASH) << "keys_insert, res: " << res;
        return res;
    }

    bool keys_delete(std::shared_ptr<record> record_ptr, keys_stores_t::iterator it)
    {
        if (it == m_keys_stores.end()) return true;

        it->second->del(record_ptr);
        keys_delete(record_ptr, ++it);

        return true;
    }

    OID insert(std::shared_ptr<record> record_ptr)
    {
        if (!keys_insert(record_ptr, m_keys_stores.begin())) return 0;

        auto pr = m_object_store.insert(std::make_pair(record_ptr->get_index(), record_ptr));

        lo::l(lo::TRASH) << "insert, object: " << record_ptr->get_payload()->tp() << ", store size: " << m_object_store.size();

        if (!pr.second) {
            keys_delete(record_ptr, m_keys_stores.begin());
            return 0;
        }

        return record_ptr->get_index();
    }

public:
    virtual OID insert(std::shared_ptr<const object> object_ptr) override
    {
        auto index = oid((m_settings & BIT_INCREMENT) != 0);
        const_cast<object*>(object_ptr.get())->set_index_fields(index);

        auto record_ptr = record::create();
        record_ptr->set_index(index);
        record_ptr->set_time(time(NULL));
        record_ptr->set_payload(object_ptr);

        return insert(record_ptr);
    }

    virtual bool update(OID index, std::shared_ptr<const object> object_ptr) override
    {
        auto answer_ptr = get(index);
        auto record_ptr = answer_ptr->get_result()->front();

        remove(record_ptr->get_index());
        // Сохраняем, если придется вернуть запись обратно
        auto temp_payload_ptr = record_ptr->get_payload();
        record_ptr->set_payload(object_ptr);

        auto oid = insert(record_ptr);
        if (oid == 0) {
            record_ptr->set_payload(temp_payload_ptr);
            // Предполагается, что это точно выполнится, т.к. эта запись там уже была
            // TODO
            insert(record_ptr);
        }
        return oid;
    }

    virtual bool remove(OID index) override
    {
        auto answer_ptr = get(index);

        keys_delete(answer_ptr->get_result()->front(), m_keys_stores.begin());

        return m_object_store.erase(answer_ptr->get_result()->front()->get_index());
    }

    virtual std::shared_ptr<const answer> get(OID index) override
    {
        auto it = m_object_store.find(index);

        if (it == m_object_store.end()) throw std::runtime_error(*smessage(SMESS220_OBJECT_NOT_FOUND, format(index)));

        auto answer_ptr = answer::create();
        answer_ptr->get_result()->push_back(it->second);

        return answer_ptr;
    }

    virtual std::shared_ptr<const object> get_object(OID index) override
    {
        auto it = m_object_store.find(index);

        if (it == m_object_store.end()) throw std::runtime_error(*smessage(SMESS220_OBJECT_NOT_FOUND, format(index)));

        return it->second->get_payload();
    }

    virtual std::shared_ptr<const answer> get_with_limit(size_t start, size_t limit, std::function<bool(const object&)> where) override
    {
        auto answer_ptr = answer::create();

        size_t begin = 0;
        size_t amount = 0;
        for (auto& pr : m_object_store) {
            if (limit > 0 && amount >= limit) break;

            // Проверяем условие
            if (where(*pr.second->get_payload())) {
                if (begin >= start) {
                    answer_ptr->get_result()->push_back(pr.second);
                    ++amount;
                }
                ++begin;
            }
        }

        return answer_ptr;
    }

    virtual size_t size() override
    {
        return m_object_store.size();
    }

    bool check(OID index)
    {
        auto it = m_object_store.find(index);
        return it == m_object_store.end();
    }

    std::shared_ptr<const answer> find(std::shared_ptr<const key_interface>& key_ptr, size_t start, size_t limit, std::function<bool(const object&)> where) override
    {
        auto it = m_keys_stores.find(key_ptr->tp());
        if (it == m_keys_stores.end()) throw std::runtime_error("Unknown key_ptr: " + key_ptr->tp());

        return it->second->find(key_ptr, start, limit, where);
    }

    std::shared_ptr<const answer> find_range(std::shared_ptr<const key_interface>& lower_ptr, std::shared_ptr<const key_interface>& upper_ptr, size_t start, size_t limit, std::function<bool(const object&)> where) override
    {
        std::string key_name = (lower_ptr) ? lower_ptr->tp() : ((upper_ptr) ? upper_ptr->tp() : "null");
        auto it = m_keys_stores.find(key_name);
        if (it == m_keys_stores.end()) throw std::runtime_error("Unknown key_name: " + key_name);

        return it->second->find_range(lower_ptr, upper_ptr, start, limit, where);
    }

    bool try_lock_record() override
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        auto it = m_locked_records.find(oid);
        if (it != m_locked_records.end()) return false;

        m_locked_records.insert(oid);
        return true;
    }

    bool try_lock_table() override
    {
        return m_mutex.try_lock();
    }

    void unlock_record(OID oid) override
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_locked_records.erase(oid);
        m_event_unlock = true;
        m_cv.notify_all();
    }

    void unlock_table() override
    {
        m_mutex.unlock();
    }

    bool get_unique_keys_flag() override
    {
        return m_unique_keys_exist_flag;
    }
};

} // end of namespace
#endif
