#ifndef KEY_STORE_H
#define KEY_STORE_H

#include <unordered_set>
#include <memory>
#include <set>

#include "key_store_interface.h"
#include "key_interface.h"
#include "answer.h"
#include "record.h"
#include "types.h"
#include "log.h"

namespace swarm
{

template <typename T>
struct owner_less
{
    bool operator()(const T& x, const T& y) const
    {
        return *x < *y;
    }
};

template <typename T>
struct owner_hash
{
    size_t operator()(const T& x) const
    {
        return x->hash();
    }
};

template <typename T>
struct owner_equal
{
    bool operator()(const T& x, const T& y) const
    {
        return *x == *y;
    }
};

// Неупорядоченные ключи
// Неуникальный
typedef std::unordered_multiset<std::shared_ptr<const key_interface>, owner_hash<std::shared_ptr<const key_interface>>, owner_equal<std::shared_ptr<const key_interface>>> key_umset_t;
// Уникальный
typedef std::unordered_set<std::shared_ptr<const key_interface>, owner_hash<std::shared_ptr<const key_interface>>, owner_equal<std::shared_ptr<const key_interface>>> key_uset_t;
// Упорядоченные ключи
// Неуникальный
typedef std::multiset<std::shared_ptr<const key_interface>, owner_less<std::shared_ptr<const key_interface>>> key_mset_t;
// Уникальный
typedef std::set<std::shared_ptr<const key_interface>, owner_less<std::shared_ptr<const key_interface>>> key_set_t;

template <typename T>
inline std::shared_ptr<const answer> _find_range(T& store, std::shared_ptr<const key_interface>& lower_ptr, std::shared_ptr<const key_interface>& upper_ptr, size_t start, size_t limit, std::function<bool(const object&)> where)
{
    auto answer_ptr = swarm::answer::create();
    auto lower_it = (lower_ptr) ? store.lower_bound(lower_ptr) : store.begin();
    auto upper_it = (upper_ptr) ? store.upper_bound(upper_ptr) : store.end();
    size_t begin = 0;
    size_t amount = 0;
    for (auto it = lower_it; it != upper_it; ++it) {
        if (limit > 0 && amount >= limit) break;
        // Проверяем условие
        if (where(*(*it)->get_record()->get_payload())) {
            if (begin >= start) {
                answer_ptr->get_result()->push_back((*it)->get_record());
                ++amount;
            }
            ++begin;
        }
    }
    return answer_ptr;
}

template <>
inline std::shared_ptr<const answer> _find_range<key_umset_t>(key_umset_t& store, std::shared_ptr<const key_interface>& lower_ptr, std::shared_ptr<const key_interface>& upper_ptr, size_t start, size_t limit, std::function<bool(const object&)> where)
{
    auto answer_ptr = swarm::answer::create();
    return answer_ptr;
}

template <>
inline std::shared_ptr<const answer> _find_range<key_uset_t>(key_uset_t& store, std::shared_ptr<const key_interface>& lower_ptr, std::shared_ptr<const key_interface>& upper_ptr, size_t start, size_t limit, std::function<bool(const object&)> where)
{
    auto answer_ptr = swarm::answer::create();
    return answer_ptr;
}

template <typename STORE, typename KEY_TYPE>
class key_store_mset : public key_store_interface
{
private:
    STORE m_store;
    typename STORE::iterator m_rollback_it;

public:
    key_store_mset()
    {
        m_rollback_it = m_store.end();
    }

    virtual bool add(std::shared_ptr<record>& record_ptr) override
    {
        lo::l(lo::TRASH) << "add key (1): " << KEY_TYPE::stp() << ", store size: " << m_store.size();
        m_rollback_it = m_store.insert(std::make_shared<KEY_TYPE>(record_ptr));
        lo::l(lo::TRASH) << "add key (2): " << KEY_TYPE::stp() << ", store size: " << m_store.size();
        // всегда true, т.к. в multiset не возникает проблем со вставкой
        return true;
    }

    virtual bool del(std::shared_ptr<record>& record_ptr) override
    {
        lo::l(lo::TRASH) << "del key (1): " << KEY_TYPE::stp() << ", store size: " << m_store.size();
        auto key = std::make_shared<KEY_TYPE>(record_ptr);

        auto pr = m_store.equal_range(key);
        for (auto it = pr.first; it != pr.second; ++it) {
            if ((*it)->get_record()->get_index() == record_ptr->get_index()) {
                m_store.erase(it);
                lo::l(lo::TRASH) << "del key (2): " << KEY_TYPE::stp() << ", store size: " << m_store.size();
                return true;
            }
        }
        lo::l(lo::TRASH) << "del key (2): " << KEY_TYPE::stp() << ", store size: " << m_store.size();
        return false;
    }

    virtual void rollback() override
    {
        if (m_rollback_it != m_store.end()) m_store.erase(m_rollback_it);
        lo::l(lo::TRASH) << "rollback: " << KEY_TYPE::stp() << ", store size: " << m_store.size();
        m_rollback_it = m_store.end();
    }

    virtual void commit() override
    {
        lo::l(lo::TRASH) << "commit";
        m_rollback_it = m_store.end();
    }

    virtual std::shared_ptr<const answer> find(std::shared_ptr<const key_interface>& key_ptr, size_t start, size_t limit, std::function<bool(const object&)> where) override
    {
        auto answer_ptr = swarm::answer::create();

        auto pr = m_store.equal_range(key_ptr);
        size_t begin = 0;
        size_t amount = 0;
        for (auto it = pr.first; it != pr.second; ++it) {
            if (limit > 0 && amount >= limit) break;

            // Проверяем условие
            if (where(*(*it)->get_object())) {
                if (begin >= start) {
                    answer_ptr->get_result()->push_back((*it)->get_record());
                    ++amount;
                }
                ++begin;
            }
        }

        return answer_ptr;
    }

    virtual std::shared_ptr<const answer> find_range(std::shared_ptr<const key_interface>& lower_ptr, std::shared_ptr<const key_interface>& upper_ptr, size_t start, size_t limit, std::function<bool(const object&)> where) override
    {
        return _find_range(m_store, lower_ptr, upper_ptr, start, limit, where);
    }

    virtual const std::string& name() override
    {
        return KEY_TYPE::stp();
    }

    static bool is_unique()
    {
        return false;
    }
};

template <typename STORE, typename KEY_TYPE>
class key_store_set : public key_store_interface
{
private:
    STORE m_store;
    typename STORE::iterator m_rollback_it;

public:
    key_store_set()
    {
        m_rollback_it = m_store.end();
    }

    virtual bool add(std::shared_ptr<record>& record_ptr) override
    {
        lo::l(lo::TRASH) << "add key (1): " << KEY_TYPE::stp() << ", store size: " << m_store.size();
        auto pr = m_store.insert(std::make_shared<KEY_TYPE>(record_ptr));
        lo::l(lo::TRASH) << "add key (2): " << KEY_TYPE::stp() << ", store size: " << m_store.size();

        if (pr.second) m_rollback_it = pr.first;

        return pr.second;
    }

    virtual bool del(std::shared_ptr<record>& record_ptr) override
    {
        lo::l(lo::TRASH) << "del key (1): " << KEY_TYPE::stp() << ", store size: " << m_store.size();
        auto key = std::make_shared<KEY_TYPE>(record_ptr);

        auto it = m_store.find(key);
        if (it != m_store.end()) {
            m_store.erase(it);
            lo::l(lo::TRASH) << "del key (2): " << KEY_TYPE::stp() << ", store size: " << m_store.size();
            return true;
        }
        lo::l(lo::TRASH) << "del key (2): " << KEY_TYPE::stp() << ", store size: " << m_store.size();
        return false;
    }

    virtual void rollback() override
    {
        if (m_rollback_it != m_store.end()) m_store.erase(m_rollback_it);
        lo::l(lo::TRASH) << "rollback: " << KEY_TYPE::stp() << ", store size: " << m_store.size();
        m_rollback_it = m_store.end();
    }

    virtual void commit() override
    {
        lo::l(lo::TRASH) << "commit";
        m_rollback_it = m_store.end();
    }

    virtual std::shared_ptr<const answer> find(std::shared_ptr<const key_interface>& key_ptr, size_t, size_t, std::function<bool(const object&)>) override
    {
        auto answer_ptr = swarm::answer::create();

        auto it = m_store.find(key_ptr);
        if (it != m_store.end()) {
            answer_ptr->get_result()->push_back((*it)->get_record());
        }

        return answer_ptr;
    }

    virtual std::shared_ptr<const answer> find_range(std::shared_ptr<const key_interface>& lower_ptr, std::shared_ptr<const key_interface>& upper_ptr, size_t start, size_t limit, std::function<bool(const object&)> where) override
    {
        return _find_range(m_store, lower_ptr, upper_ptr, start, limit, where);
    }

    virtual const std::string& name() override
    {
        return KEY_TYPE::stp();
    }

    static bool is_unique()
    {
        return true;
    }
};

} // end of namespace

#endif
