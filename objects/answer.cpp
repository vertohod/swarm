#include "answer.h"
#include "empty.h"

namespace swarm
{

DEFINITION_FIELDS_FUNCTIONS_STORES(answer)

DEFINITION_CONSTS(answer, "answer", BIT_SYNCHRONIZE, 1)

DEFINITION_FIELD(answer, std::shared_ptr<std::list<std::shared_ptr<record>>>, result)

DEFINITION_SPECIAL_MEMBERS(answer)

answer::answer()
{
    FIELD(result) = std::make_shared<std::list<std::shared_ptr<record>>>();
}

} // end of namespace
