#ifndef SERVER_SHORT_H
#define SERVER_SHORT_H

#include "object_includes.h"
#include "defines_object.h"
#include "defines_key.h"

namespace swarm
{

DECLARATION_OBJECT(server_short,
    DECLARATION_FIELD(OID, id)
    DECLARATION_FIELD(OID, remainder)
    DECLARATION_FIELD(time_tt, time)
    DECLARATION_FIELD(bool, main)
)

} // end of namespace

#endif
