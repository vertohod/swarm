#include "salt.h"

namespace swarm
{

DEFINITION_FIELDS_FUNCTIONS_STORES(salt)

DEFINITION_CONSTS(salt, "salt", BIT_NULL, 1)

DEFINITION_FIELD(salt, sptr_cstr, value);

DEFINITION_SPECIAL_MEMBERS(salt)

salt::salt() {}

} // end of namespace

