#include "converter.h"
#include "object.h"

namespace swarm
{

namespace converter_namespace
{
    void deserialization(rapidjson::Value& obj, swarm::object& res)
    {
        res.deserialization(obj);
    }
} // end of namespace

template <>
void converter(rapidjson::Value& obj, std::shared_ptr<object>& res)
{
    if (obj.IsObject() && obj.HasMember(FIELD_OBJECT_TYPE) && obj[FIELD_OBJECT_TYPE].IsString()) {
        res = swarm::object::create_object(obj[FIELD_OBJECT_TYPE].GetString());
        converter_namespace::fn_converter(obj, *res);
    } else throw std::runtime_error("Can't identify the object (1)");
}

template <>
void converter(rapidjson::Value& obj, std::shared_ptr<const object>& res)
{
    if (obj.IsObject() && obj.HasMember(FIELD_OBJECT_TYPE) && obj[FIELD_OBJECT_TYPE].IsString()) {
        res = swarm::object::create_object(obj[FIELD_OBJECT_TYPE].GetString());
        converter_namespace::fn_converter(obj, *res);
    } else throw std::runtime_error("Can't identify the object (2)");
}

} // end of namespace
