#ifndef CONVERTER_H
#define CONVERTER_H

#include <stdexcept>
#include <memory>
#include <vector>
#include <list>

#include "defines_properties_methods.h"
#include "object.h"
#include "types.h"
#include "json.h"

namespace swarm
{

namespace converter_namespace
{

    template <typename T, typename U>
    struct st_converter
    {
        void operator()(rapidjson::Value& obj, T& res)
        {
            if (obj.IsUint()) {
                res = static_cast<T>(obj.GetUint());
            } else if (obj.IsInt()) {
                res = static_cast<T>(obj.GetInt());
            } else if (obj.IsUint64()) {
                res = static_cast<T>(obj.GetUint64());
            } else if (obj.IsInt64()) {
                res = static_cast<T>(obj.GetInt64());
            } else if (obj.IsDouble()) {
                res = static_cast<T>(obj.GetDouble());
            } else if (obj.IsBool()) {
                res = static_cast<T>(obj.GetBool());
            } else {
                throw std::runtime_error("Unknown type (1)");
            }
        }
    };
    
    void deserialization(rapidjson::Value& obj, swarm::object& res);

    template <typename T>
    struct st_converter<T, const swarm::object&>
    {
        void operator()(rapidjson::Value& obj, T& res)
        {
            auto& temp = *const_cast<swarm::object*>(static_cast<const swarm::object*>(&res));
            deserialization(obj, temp);
        }
    };

    void selector(...);
    const swarm::object& selector(const swarm::object*);

    template <typename T>
    inline void fn_converter(rapidjson::Value& obj, T& res)
    {
        st_converter<T, decltype(selector((T*) 0))>()(obj, res);
    }

    template <>
    inline void fn_converter<std::string>(rapidjson::Value& obj, std::string& res)
    {
        if (obj.IsString()) {
            res = obj.GetString();
        } else {
            throw std::runtime_error("Unknown type (2)");
        }
    }

    template <>
    inline void fn_converter<const std::string>(rapidjson::Value& obj, const std::string& res)
    {
        if (obj.IsString()) {
            *const_cast<std::string*>(&res) = obj.GetString();
        } else {
            throw std::runtime_error("Unknown type (3)");
        }
    }

    template <>
    inline void fn_converter<sptr_str>(rapidjson::Value& obj, sptr_str& res)
    {
        fn_converter(obj, *res);
    }

    template <>
    inline void fn_converter<sptr_cstr>(rapidjson::Value& obj, sptr_cstr& res)
    {
        fn_converter(obj, *res);
    }

    template <>
    inline void fn_converter<swarm::index>(rapidjson::Value& obj, swarm::index& res)
    {
        if (obj.IsUint64()) {
            res.set_value(obj.GetUint64());
        } else {
            throw std::runtime_error("Unknown type (4)");
        }
    }

    template <>
    inline void fn_converter<const swarm::index>(rapidjson::Value& obj, const swarm::index& res)
    {
        if (obj.IsUint64()) {
            const_cast<swarm::index*>(&res)->set_value(obj.GetUint64());
        } else {
            throw std::runtime_error("Unknown type (5)");
        }
    }

    template <typename T>
    inline void fn_converter(rapidjson::Value& obj, std::shared_ptr<T>& res)
    {
        res = std::make_shared<T>();
        fn_converter(obj, *res);
    }

    template <>
    inline void fn_converter(rapidjson::Value& obj, std::shared_ptr<object>& res)
    {
        if (obj.IsObject() && obj.HasMember(FIELD_OBJECT_TYPE) && obj[FIELD_OBJECT_TYPE].IsString()) {
            res = swarm::object::create_object(obj[FIELD_OBJECT_TYPE].GetString());
            fn_converter(obj, *res);
        } else throw std::runtime_error("Can't identify the object (3)");
    }

    template <>
    inline void fn_converter(rapidjson::Value& obj, std::shared_ptr<const object>& res)
    {
        if (obj.IsObject() && obj.HasMember(FIELD_OBJECT_TYPE) && obj[FIELD_OBJECT_TYPE].IsString()) {
            res = swarm::object::create_object(obj[FIELD_OBJECT_TYPE].GetString());
            fn_converter(obj, *res);
        } else throw std::runtime_error("Can't identify the object (4)");
    }

} // end of namespace

template <typename T>
void converter(rapidjson::Value& obj, T& res)
{
    converter_namespace::fn_converter(obj, res);
}

template <typename T>
void converter(rapidjson::Value& obj, std::vector<T>& res)
{
	if (obj.IsArray()) {
		for (auto& val : obj.GetArray()) {
            T res_temp;
            converter_namespace::fn_converter(val, res_temp);
            res.push_back(res_temp);
        }
	} else {
        throw std::runtime_error("Unknown type (5)");
	}
}

template <typename T>
void converter(rapidjson::Value& obj, std::list<T>& res)
{
	if (obj.IsArray()) {
		for (auto& val : obj.GetArray()) {
            T res_temp;
            converter_namespace::fn_converter(val, res_temp);
            res.push_back(res_temp);
        }
	} else {
        throw std::runtime_error("Unknown type (6)");
	}
}

template <typename T>
void converter(rapidjson::Value& obj, std::shared_ptr<T>& res)
{
    res = std::make_shared<T>();
    converter(obj, *res);
}

template <>
void converter(rapidjson::Value& obj, std::shared_ptr<object>& res);

template <>
void converter(rapidjson::Value& obj, std::shared_ptr<const object>& res);

} // end of namespace

#endif
