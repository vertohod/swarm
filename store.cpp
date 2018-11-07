#include "store.h"

namespace swarm
{

store::store(OID server_id) : m_server_id(server_id)
{
}

std::shared_ptr<table_interface> store::table(sptr_cstr table_name)
{    auto it = m_table_map.find(*table_name);
    if (it == m_table_map.end()) std::runtime_error("The table " + *table_name + " was not found");
    return it->second; 
}

OID store::insert(std::shared_ptr<const object> object_ptr)
{
    return table(object_ptr->tp())->insert(object_ptr);
}

bool store::update(OID index, std::shared_ptr<const object> object_ptr)
{
    return table(object_ptr->tp())->update(index, object_ptr);
}

bool store::remove(sptr_cstr table_name, OID index)
{
    return table(table_name)->remove(index);
}

std::shared_ptr<const answer> store::select(sptr_cstr table_name, OID index)
{
    return table(table_name)->get(index);
}

std::shared_ptr<const answer> store::select(sptr_cstr table_name, std::function<bool(const object&)> where, size_t start, size_t limit)
{
    return table(table_name)->get_with_limit(where, start, limit);
}

} // end of namespace
