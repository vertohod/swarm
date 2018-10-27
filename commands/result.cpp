#include "result.h"

namespace swarm
{

DEFINITION_FIELDS_FUNCTIONS_STORES(result)

DEFINITION_CONSTS(result, "result", BIT_NULL, 1)

DEFINITION_FIELD(result, OID, packet_id)
DEFINITION_FIELD(result, int, code)

DEFINITION_SPECIAL_MEMBERS(result)

result::result() : FIELD(packet_id)(0), FIELD(code)(0) {}

} // end of namespace

