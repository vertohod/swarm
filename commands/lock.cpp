#include "lock.h"

namespace swarm
{

DEFINITION_FIELDS_FUNCTIONS_STORES(lock)

DEFINITION_CONSTS(lock, "lock", BIT_NULL, 1)

DEFINITION_FIELD(lock, sptr_cstr, table_name)
DEFINITION_FIELD(lock, OID, oid)

DEFINITION_SPECIAL_MEMBERS(lock)

lock::lock() {}

} // end of namespace

