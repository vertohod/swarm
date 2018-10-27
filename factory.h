#ifndef FACTORY_H
#define FACTORY_H

#include <memory>

#include "object.h"

namespace swarm
{

template <typename T>
class factory
{
public:
    static std::shared_ptr<T> create()
    {
        return std::shared_ptr<T>(new T());
    }

    static std::shared_ptr<object> create_interface()
    {
        return std::shared_ptr<object>(new T());
    }

    inline static std::shared_ptr<const T> convert(std::shared_ptr<const object> object_ptr)
    {
        return std::dynamic_pointer_cast<const T>(object_ptr);
    }

    inline static const T& convert(const object& object)
    {
        return dynamic_cast<const T&>(object);
    }
};

} // end of namespace

#endif
