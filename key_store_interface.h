#ifndef KEY_STORE_INTERFACE_H
#define KEY_STORE_INTERFACE_H

#include <unordered_map>
#include <memory>

#include "key_interface.h"

namespace swarm
{

class answer;
class record;

class key_store_interface
{
public:
    virtual ~key_store_interface();
    virtual bool add(std::shared_ptr<record>&) = 0;
    virtual bool del(std::shared_ptr<record>&) = 0;
    virtual void rollback() = 0;
    virtual void commit() = 0;

    virtual const std::string& name() = 0;

    virtual std::shared_ptr<const answer> find(std::shared_ptr<const key_interface>&, size_t, size_t, std::function<bool(const object&)>) = 0;
    virtual std::shared_ptr<const answer> find_range(std::shared_ptr<const key_interface>& lower_ptr, std::shared_ptr<const key_interface>& upper_ptr, size_t start, size_t limit, std::function<bool(const object&)> where) = 0;
};

typedef std::unordered_map<std::string, std::shared_ptr<key_store_interface>> keys_stores_t;

} // end of namespace

#endif
