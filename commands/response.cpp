#include "response.h"

namespace swarm
{

DEFINITION_FIELDS_FUNCTIONS_STORES(response)

DEFINITION_CONSTS(response, "response", BIT_NULL, 1)

DEFINITION_FIELD(response, int, code)
DEFINITION_FIELD(response, OID, command_id)
DEFINITION_FIELD(response, sptr_cstr, description)
DEFINITION_FIELD(response, std::shared_ptr<std::list<std::shared_ptr<const swarm::object>>>, result)

response::response() : FIELD(code)(0), FIELD(command_id)(0)
{
    FIELD(result) = std::make_shared<std::list<std::shared_ptr<const swarm::object>>>();
}

DEFINITION_SPECIAL_MEMBERS(response)

} // end of namespace
