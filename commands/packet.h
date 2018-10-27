#ifndef PACKET_H
#define PACKET_H

#include <memory>
#include <list>

#include "object_includes.h"
#include "defines_object.h"

namespace swarm
{

DECLARATION_OBJECT(packet,
    DECLARATION_FIELD(OID, transaction_id)
    DECLARATION_FIELD(OID, task_id)
    DECLARATION_FIELD(OID, action_id)
    DECLARATION_FIELD(OID, server_id)
    DECLARATION_FIELD(OID, gateaway_id)
    DECLARATION_FIELD(OID, recipient_id)
    DECLARATION_FIELD(std::shared_ptr<std::list<std::shared_ptr<object>>>, payload)
    DECLARATION_FIELD(std::shared_ptr<std::list<OID>>, route)
)

} // end of namespace

#endif
