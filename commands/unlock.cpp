#include "unlock.h"

namespace swarm
{

DEFINITION_FIELDS_FUNCTIONS_STORES(unlock)

DEFINITION_CONSTS(unlock, "unlock", BIT_NULL, 1)

DEFINITION_FIELD(unlock, sptr_cstr, table_name)
DEFINITION_FIELD(unlock, OID, oid)

DEFINITION_SPECIAL_MEMBERS(unlock)

unlock::unlock() {}

} // end of namespace

