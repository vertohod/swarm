#ifndef INDEX_H
#define INDEX_H

#include <types.h>

namespace swarm
{

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
