#ifndef RECORD_H
#define RECORD_H

#include <memory>

#include "object_includes.h"
#include "defines_object.h"

namespace swarm
{

DECLARATION_OBJECT(record,
    DECLARATION_FIELD(OID, index)
    DECLARATION_FIELD(OID, owner)
    DECLARATION_FIELD(time_tt, time)
    DECLARATION_FIELD(std::shared_ptr<const object>, payload)
)

} // end of namespace

#endif
