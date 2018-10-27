#ifndef CONVERT_TO_OID
#define CONVERT_TO_OID

#include "object.h"
#include "types.h"

namespace swarm
{

template <typename T>
void convert_to_oid(OID oid, T& out) 
{
}

template <>
void convert_to_oid<swarm::index>(OID oid, swarm::index& out);

} // end of namespace

#endif
