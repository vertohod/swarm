#include "server_short.h"
#include "defines_key_store.h"

namespace swarm
{

DEFINITION_FIELDS_FUNCTIONS_STORES(server_short)

DEFINITION_CONSTS(server_short, "server_short", BIT_NULL, 1)

DEFINITION_FIELD(server_short, OID, id);
DEFINITION_FIELD(server_short, OID, remainder);
DEFINITION_FIELD(server_short, time_tt, time);
DEFINITION_FIELD(server_short, bool, main);

server_short::server_short() :
    FIELD(id)(0),
    FIELD(remainder)(0),
    FIELD(time)(0),
    FIELD(main)(false)
{}

DEFINITION_SPECIAL_MEMBERS(server_short)

} // end of namespace

