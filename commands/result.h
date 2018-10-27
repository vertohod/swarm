#ifndef RESULT_H
#define RESULT_H

#include "object_includes.h"
#include "defines_object.h"

namespace swarm
{

DECLARATION_OBJECT(result,
    DECLARATION_FIELD(OID, packet_id)
    DECLARATION_FIELD(int, code)
)

} // end of namespace

#endif
