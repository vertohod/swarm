#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include "object_includes.h"
#include "defines_object.h"

namespace swarm
{

DECLARATION_OBJECT(authentication,
    DECLARATION_FIELD(OID, server_id)
    DECLARATION_FIELD(OID, remainder)
    DECLARATION_FIELD(sptr_cstr, secret_key_hash)
)

} // end of namespace

#endif
