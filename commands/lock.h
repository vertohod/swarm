#ifndef LOCK_H
#define LOCK_H

#include "object_includes.h"
#include "defines_object.h"

namespace swarm
{

DECLARATION_OBJECT(lock,
    DECLARATION_FIELD(sptr_cstr, table_name)
    DECLARATION_FIELD(OID, oid)
)

} // end of namespace

#endif
