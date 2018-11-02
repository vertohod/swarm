#ifndef STORE_H
#define STORE_H

namespace swarm
{

class store
{
private:
    typedef std::unordered_map<std::string, std::shared_ptr<table_interface>> table_map_t;
    table_map_t m_table_map;

    template <typename T>
    inline void add_table()
    {
        auto it = m_table_map.find(T::stp());
        if (it == m_table_map.end()) {
            auto pr = m_table_map.insert(std::make_pair(T::stp(), std::make_shared<::swarm::table<T>>(m_server_id)));
            if (pr.second) it = pr.first;
            else throw std::runtime_error("Error during the adding a table with name: " + T::stp());
        } else throw std::runtime_error("The storage with such name exists (" + T::stp() + ")");
    }

public:
    void insert(std::shared_ptr<const object> object_ptr);
    // Пытается заблокировать всю таблицу
    bool lock_table(sptr_cstr table_name);
    // Пытается заблокировать запись в таблице
    bool lock_record(sptr_cstr table_name, OID oid);
    void unlock(sptr_cstr table_name, OID oid = 0);
};

} // end of namespace

#endif
