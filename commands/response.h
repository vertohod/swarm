#ifndef RESPONSE_H
#define RESPONSE_H

#include "object_includes.h"
#include "defines_object.h"

namespace swarm
{

DECLARATION_OBJECT(response,
    DECLARATION_FIELD(int, code)
    DECLARATION_FIELD(OID, command_id)
    DECLARATION_FIELD(sptr_cstr, description)
    DECLARATION_FIELD(std::shared_ptr<std::list<std::shared_ptr<const object>>>, result)
)

} // end of namespace

#endif
