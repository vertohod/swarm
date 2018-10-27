#include "pusher.h"
#include "object.h"

namespace swarm
{

namespace pusher_namespace
{

    rapidjson::Value serialization(rapidjson::MemoryPoolAllocator<>& al, const swarm::object& value)
    {
        rapidjson::Value res(rapidjson::kObjectType);
        value.serialization(res, al);
        return res;
    }

} // end of namespace

} // end of namespace
