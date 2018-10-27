#include "convert_to_oid.h"

namespace swarm
{

template <>
void convert_to_oid<swarm::index>(OID oid, swarm::index& out)
{
    out = oid;
}

} // end of namespace
