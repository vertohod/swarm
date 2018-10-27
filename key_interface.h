#ifndef KEY_INTERFACE_H
#define KEY_INTERFACE_H

#include <memory>
#include <vector>

namespace swarm
{

class record;
class object;

struct key_interface
{
private:
    std::shared_ptr<record> m_record;

public:
    key_interface();
    key_interface(std::shared_ptr<record>& record_ptr);
    virtual ~key_interface();

    std::shared_ptr<record> get_record() const;
    virtual const object* get_object() const = 0;

    virtual const std::string& tp() const = 0;
    virtual const std::string& table_name() const = 0;
    virtual size_t hash() const = 0;
    virtual bool operator==(const key_interface&) const = 0;
    virtual bool operator<(const key_interface&) const = 0;
};

} // end of namespace

#endif
