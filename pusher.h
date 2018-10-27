#ifndef PUSHER_H
#define PUSHER_H

#include <stdexcept>
#include <memory>
#include <vector>
#include <list>

#include "object.h"
#include "types.h"
#include "json.h"

namespace swarm
{

namespace pusher_namespace
{
    rapidjson::Value serialization(rapidjson::MemoryPoolAllocator<>& al, const swarm::object& value);

    template <typename T, typename U>
    struct st_value_maker
    {
        rapidjson::Value operator()(rapidjson::MemoryPoolAllocator<>& al, const T& value)
        {
            return rapidjson::Value(value);
        }
    };

    template <typename T>
    struct st_value_maker<T, swarm::object&>
    {
        rapidjson::Value operator()(rapidjson::MemoryPoolAllocator<>& al, const swarm::object& value)
        {
            return serialization(al, value);
        }
    };

    void selector(...);
    swarm::object& selector(swarm::object*);

    template <typename T>
    inline rapidjson::Value fn_value_maker(rapidjson::MemoryPoolAllocator<>& al, const T& value)
    {
        return st_value_maker<T, decltype(selector((T*) 0))>()(al, value);
    }

    template <>
    inline rapidjson::Value fn_value_maker(rapidjson::MemoryPoolAllocator<>& al, const std::string& value)
    {
        rapidjson::Value res(value.c_str(), value.length(), al);
        return res;
    }

    template <>
    inline rapidjson::Value fn_value_maker(rapidjson::MemoryPoolAllocator<>& al, const swarm::index& value)
    {
        return rapidjson::Value(value.value());
    }

    template <typename T>
    inline rapidjson::Value fn_value_maker(rapidjson::MemoryPoolAllocator<>& al, const std::shared_ptr<T>& value)
    {
        return fn_value_maker(al, *value);
    }

    template <typename T>
    inline rapidjson::Value fn_value_maker(rapidjson::MemoryPoolAllocator<>& al, const sptr_string<T>& value)
    {
        return fn_value_maker(al, *value);
    }
} // end of namespace

template <typename T>
void pusher(rapidjson::Value& obj, rapidjson::MemoryPoolAllocator<>& al, const std::string& name, const T& value)
{
    rapidjson::Value key(name.c_str(), name.length(), al);
    obj.AddMember(key, pusher_namespace::fn_value_maker(al, value), al);
}

template <typename T>
void pusher(rapidjson::Value& obj, rapidjson::MemoryPoolAllocator<>& al, const std::string& name, const std::vector<T>& value)
{
    rapidjson::Value key(name.c_str(), name.length(), al);
    rapidjson::Value arr(rapidjson::kArrayType);
    for (auto& temp : value) {
        arr.PushBack(pusher_namespace::fn_value_maker(al, temp), al);
    }
    obj.AddMember(key, arr, al);
}

template <typename T>
void pusher(rapidjson::Value& obj, rapidjson::MemoryPoolAllocator<>& al, const std::string& name, const std::list<T>& value)
{
    rapidjson::Value key(name.c_str(), name.length(), al);
    rapidjson::Value arr(rapidjson::kArrayType);
    for (auto& temp : value) {
        arr.PushBack(pusher_namespace::fn_value_maker(al, temp), al);
    }
    obj.AddMember(key, arr, al);
}

template <typename T>
void pusher(rapidjson::Value& obj, rapidjson::MemoryPoolAllocator<>& al, const std::string& name, const std::shared_ptr<T>& value)
{
    pusher(obj, al, name, *value);
}

} // end of namespace

#endif
