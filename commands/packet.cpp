#include "packet.h"

namespace swarm
{

DEFINITION_FIELDS_FUNCTIONS_STORES(packet)

DEFINITION_CONSTS(packet, "packet", BIT_NULL, 1)

DEFINITION_FIELD(packet, OID, transaction_id)
DEFINITION_FIELD(packet, OID, task_id)
DEFINITION_FIELD(packet, OID, action_id)
DEFINITION_FIELD(packet, OID, server_id)
DEFINITION_FIELD(packet, OID, gateaway_id)
DEFINITION_FIELD(packet, OID, recipient_id)
DEFINITION_FIELD(packet, std::shared_ptr<std::list<std::shared_ptr<object>>>, payload)
DEFINITION_FIELD(packet, std::shared_ptr<std::list<OID>>, route)

DEFINITION_SPECIAL_MEMBERS(packet)

packet::packet() :
    FIELD(transaction_id)(0),
    FIELD(task_id)(0),
    FIELD(action_id)(0),
    FIELD(server_id)(0),
    FIELD(gateaway_id)(0),
    FIELD(recipient_id)(0)
{
    FIELD(payload) = std::make_shared<std::list<std::shared_ptr<object>>>();
    FIELD(route) = std::make_shared<std::list<OID>>();
}

} // end of namespace

