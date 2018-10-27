#include "range.h"

namespace swarm
{

void range::init(OID remainder, OID divider, OID parts)
{
    m_remainders.clear();

    m_remainder = remainder;
    m_divider = divider;
    m_parts = parts;

    for (OID count = 0; count < m_parts; ++count) {
        m_remainders.insert(remainder++);
        if (remainder == m_divider) remainder = 0;
    }
}

bool range::check(OID something_oid)
{
    return m_remainders.end() != m_remainders.find(something_oid % m_divider);
}

std::unordered_set<OID> range::get_remainders(OID id)
{
    std::unordered_set<OID> result;
    id = id % m_divider;
    for (OID count = 0; count < m_parts; ++count) {
        result.insert(id);
        if (id == 0) id = m_divider;
        --id;
    }
    return result;
}

} // end of namespace
