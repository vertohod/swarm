#ifndef RANGE_H
#define RANGE_H

#include <unordered_set>

#include "types.h"

namespace swarm
{

class range
{
private:
    OID                     m_remainder;
    OID                     m_divider;
    OID                     m_parts;
    std::unordered_set<OID> m_remainders;

public:
    void init(OID remainder, OID divider, OID parts);
    bool check(OID something_oid);
    std::unordered_set<OID> get_remainders(OID something_oid);
};

} // end of namespace

#endif

