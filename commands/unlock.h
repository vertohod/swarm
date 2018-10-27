#ifndef UNLOCK_H
#define UNLOCK_H

#include "object_includes.h"
#include "defines_object.h"

namespace swarm
{

DECLARATION_OBJECT(unlock,
    DECLARATION_FIELD(sptr_cstr, table_name)
    DECLARATION_FIELD(OID, oid)
)

} // end of namespace

#endif
