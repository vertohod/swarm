#ifndef OBJECT_H
#define OBJECT_H

#include <unordered_map>
#include <functional>
#include <memory>
#include <string>
#include <list>

#include "types.h"
#include "json.h"

namespace swarm
{

// дружественный класс для object
template <typename T> class table;

class object
{
protected:
    typedef std::unordered_map<std::string, std::function<void(object*, rapidjson::Value& obj)>> deserialization_functions_t;
    typedef std::list<std::function<void(const object*, rapidjson::Value& obj, rapidjson::MemoryPoolAllocator<>& al)>> serialization_functions_t;
    typedef std::unordered_map<std::string, std::function<std::shared_ptr<object>()>> create_functions_t;
    typedef std::list<std::function<void(object*, OID)>> index_fields_t;

    static create_functions_t m_create_functions;

public:
    static serialization_functions_t m_serialization_functions;
    static deserialization_functions_t m_deserialization_functions;
    static index_fields_t m_index_fields;

public:
    enum EVENT {
        NONE    = 0,
        SELECT  = 1,
        INSERT  = 2,
        UPDATE  = 3,
        REMOVE  = 4,
        RESULT  = 5
    };

private:
    static create_functions_t& instance_store_functions();

public:
    object();
    virtual ~object();

    virtual const std::string& tp() const = 0;
    virtual int vr() const = 0;
    virtual size_t settings() const = 0;
    virtual std::shared_ptr<object> make_copy() const = 0;
    virtual void set_index_fields(OID) = 0;

    static void registration_object(const std::string& name, std::function<std::shared_ptr<object>()> func);
    static std::shared_ptr<object> create_object(sptr_cstr name);

    virtual sptr_cstr serialization() const final;
    virtual void serialization(rapidjson::Value& obj, rapidjson::MemoryPoolAllocator<>& al) const;
    virtual void deserialization(sptr_cstr json_ptr) final;
    virtual void deserialization(rapidjson::Document& obj);
    virtual void deserialization(rapidjson::Value& obj);
    const object* get_object() const;

public:
    static sptr_cstr get_field(sptr_cstr json_ptr, const std::string& field_name);

public:
    bool operator==(const object& obj) const;
};

class index
{
private:
    OID m_index;
public:
    index();
    index(OID);
    void set_value(OID);
    OID value() const;
};

} // end of namespace
#endif
