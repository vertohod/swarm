#include "command.h"
#include "empty.h"

namespace swarm
{

DEFINITION_FIELDS_FUNCTIONS_STORES(command)

DEFINITION_CONSTS(command, "command", BIT_NULL, 1)

DEFINITION_FIELD(command, OID, id)
DEFINITION_FIELD(command, std::shared_ptr<object>, pl)

command::command() : FIELD(id)(0)
{
    FIELD(pl) = empty::create();
}

DEFINITION_SPECIAL_MEMBERS(command)

} // end of namespace
