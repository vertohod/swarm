#ifndef ANSWER_H
#define ANSWER_H

#include <memory>
#include <list>

#include "object_includes.h"
#include "defines_object.h"
#include "record.h"

namespace swarm
{

DECLARATION_OBJECT(answer,
    DECLARATION_FIELD(std::shared_ptr<std::list<std::shared_ptr<record>>>, result)
)

} // end of namespace

#endif
