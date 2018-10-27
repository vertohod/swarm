#include <stdexcept>
#include <ctime>

#include "object.h"
#include "format.h"
#include "stream.h"
#include "util.h"

namespace swarm
{

// обязательно добно быть выше чем определение полей DEFINITION_FIELD
object::serialization_functions_t object::m_serialization_functions;
object::deserialization_functions_t object::m_deserialization_functions;

index::index() {}
index::index(OID i) { m_index = i; }
void index::set_value(OID id) { m_index = id; }
OID index::value() const { return m_index; }

object::create_functions_t& object::instance_store_functions()
{
    static object::create_functions_t m_create_functions;
    return m_create_functions;
}

void object::registration_object(const std::string& name, std::function<std::shared_ptr<object>()> func)
{
    instance_store_functions().insert(std::make_pair(name, func));
}

std::shared_ptr<object> object::create_object(sptr_cstr name)
{
    auto& functions = instance_store_functions();
    auto it = functions.find(*name);

    if (it != functions.end()) {
        return it->second();
    } else {
        throw std::runtime_error("Unknown object: " + *name);
    }    

    return std::shared_ptr<object>();
}

object::object() {}
object::~object() {}

sptr_cstr object::serialization() const
{
    rapidjson::Document doc(rapidjson::kObjectType);
    rapidjson::Value obj(rapidjson::kObjectType);

    serialization(obj, doc.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    obj.Accept(writer);

    return buffer.GetString();
}

void object::serialization(rapidjson::Value& obj, rapidjson::MemoryPoolAllocator<>& al) const
{
    for (auto& func : m_serialization_functions) func(this, obj, al);
}

void object::deserialization(sptr_cstr json_ptr)
{
    rapidjson::Document doc;
    doc.Parse(json_ptr->c_str());

    deserialization(doc);
}

void object::deserialization(rapidjson::Document& obj)
{
    for (auto& member : obj.GetObject()) {
        auto it = m_deserialization_functions.find(member.name.GetString());
        if (it != m_deserialization_functions.end()) {
            it->second(this, member.value);
        }
    }
}

void object::deserialization(rapidjson::Value& obj)
{
    for (auto& member : obj.GetObject()) {
        auto it = m_deserialization_functions.find(member.name.GetString());
        if (it != m_deserialization_functions.end()) {
            it->second(this, member.value);
        }
    }
}

const object* object::get_object() const
{
    return this;
}

sptr_cstr object::get_field(sptr_cstr json_ptr, const std::string& field_name)
{
	rapidjson::Document doc;

	try {
		doc.Parse(json_ptr->c_str());
	} catch (const std::exception& e) {
		throw std::runtime_error(std::string("Can't parse json, ") + e.what() + ", string: " + *json_ptr);
	}

	if (doc.IsObject() && doc.HasMember(field_name.c_str()) && doc[field_name.c_str()].IsString()) {
		return doc[field_name.c_str()].GetString();
	}

    return "";
}

} // end of namespace
