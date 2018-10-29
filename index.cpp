#include "index.h"

namespace swarm
{

index::index() {}

index::index(OID i)
{
    m_index = i;
}

void index::set_value(OID id)
{
    m_index = id;
}

OID index::value() const
{
    return m_index;
}

} // end of namespace
