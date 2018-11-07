#ifndef STORE_H
#define STORE_H

#include <unordered_map>

#include "types.h"
#include "table.h"
#include "table_interface.h"

#include "objects/answer.h"

namespace swarm
{

class store
{
private:
    typedef std::unordered_map<std::string, std::shared_ptr<table_interface>> table_map_t;
    table_map_t m_table_map;
    OID m_server_id;

private:
    std::shared_ptr<table_interface> table(sptr_cstr table_name);

public:
    template <typename T>
    void add_table()
    {
        auto it = m_table_map.find(T::stp());
        if (it == m_table_map.end()) {
            auto pr = m_table_map.insert(std::make_pair(T::stp(), std::make_shared<::swarm::table<T>>(m_server_id)));
            if (pr.second) it = pr.first;
            else throw std::runtime_error("Error during the adding a table with name: " + T::stp());
        } else throw std::runtime_error("The storage with such name exists (" + T::stp() + ")");
    }

    store(OID server_id);
    OID insert(std::shared_ptr<const object> object_ptr);
    bool update(OID index, std::shared_ptr<const object> object_ptr);
    bool remove(sptr_cstr table_name, OID index);
    std::shared_ptr<const answer> select(sptr_cstr table_name, OID index);
    std::shared_ptr<const answer> select(sptr_cstr table_name, std::function<bool(const object&)> where, size_t start = 0, size_t limit = 0);

    // Блокируем всю таблицу
    bool lock_table(sptr_cstr table_name);
    void unlock_table(sptr_cstr table_name);
    // Блокируем запись в таблице
    bool lock_record(sptr_cstr table_name, OID oid);
    void unlock_record(sptr_cstr table_name, OID oid);
};

} // end of namespace

#endif
