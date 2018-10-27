#ifndef SERVER_H
#define SERVER_H

#include "object_includes.h"
#include "defines_object.h"
#include "defines_key.h"

namespace swarm
{

DECLARATION_OBJECT(server,
    DECLARATION_FIELD(OID, id)
    DECLARATION_FIELD(int, socket_id)
    DECLARATION_FIELD(OID, remainder)
    DECLARATION_FIELD(time_tt, time)
    DECLARATION_FIELD(bool, approved)
    DECLARATION_FIELD(bool, main)
    DECLARATION_FIELD(sptr_cstr, salt)
)

DECLARATION_KEY(server, key_server_server, DECLARATION_FIELD(OID, id))
DECLARATION_KEY(server, key_server_socket, DECLARATION_FIELD(int, socket_id))
DECLARATION_KEY(server, key_server_remainder, DECLARATION_FIELD(OID, remainder))
DECLARATION_KEY(server, key_server_main, DECLARATION_FIELD(bool, main))

} // end of namespace

#endif
