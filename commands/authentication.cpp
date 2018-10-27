#include "authentication.h"

namespace swarm
{

DEFINITION_FIELDS_FUNCTIONS_STORES(authentication)

DEFINITION_CONSTS(authentication, "authentication", BIT_NULL, 1)

DEFINITION_FIELD(authentication, OID, server_id);
DEFINITION_FIELD(authentication, OID, remainder);
DEFINITION_FIELD(authentication, sptr_cstr, secret_key_hash)

DEFINITION_SPECIAL_MEMBERS(authentication)

authentication::authentication() : FIELD(server_id)(0) {}

} // end of namespace
