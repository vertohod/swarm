#include "key_interface.h"

namespace swarm
{

key_interface::key_interface() {}
key_interface::key_interface(std::shared_ptr<record>& record_ptr) : m_record(record_ptr) {}
key_interface::~key_interface() {}

std::shared_ptr<record> key_interface::get_record() const
{
    return m_record;
}

} // end of namespace
