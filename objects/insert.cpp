#include "insert.h"
#include "empty.h"

namespace swarm
{

DEFINITION_FIELDS_FUNCTIONS_STORES(insert)

DEFINITION_CONSTS(insert, "insert", BIT_NULL, 1)

DEFINITION_FIELD(insert, std::shared_ptr<const object>, payload)

DEFINITION_SPECIAL_MEMBERS(insert)

insert::insert()
{
    FIELD(payload) = empty::create();
}

} // end of namespace
