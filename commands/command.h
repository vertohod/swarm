#ifndef COMMAND_H
#define COMMAND_H

#include <memory>

#include "object_includes.h"
#include "defines_object.h"

namespace swarm
{

DECLARATION_OBJECT(command,
    DECLARATION_FIELD(OID, id)
    DECLARATION_FIELD(std::shared_ptr<object>, pl)
)

} // end of namespace

#endif
