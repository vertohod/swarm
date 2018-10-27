#include "record.h"
#include "empty.h"

namespace swarm
{

DEFINITION_FIELDS_FUNCTIONS_STORES(record)

DEFINITION_CONSTS(record, "record", BIT_NULL, 1)

DEFINITION_FIELD(record, OID, index)
DEFINITION_FIELD(record, OID, owner)
DEFINITION_FIELD(record, time_tt, time)
DEFINITION_FIELD(record, std::shared_ptr<const object>, payload)

DEFINITION_SPECIAL_MEMBERS(record)

record::record() : 
    FIELD(index)(0),
    FIELD(owner)(0),
    FIELD(time)(time(NULL))
{
    FIELD(payload) = empty::create();
}

} // end of namespace
