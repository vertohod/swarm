#ifndef INSERT_H
#define INSERT_H

#include "object_includes.h"
#include "defines_object.h"

namespace swarm
{

DECLARATION_OBJECT(insert,
    DECLARATION_FIELD(std::shared_ptr<const object>, payload)
)

} // end of namespace

#endif
