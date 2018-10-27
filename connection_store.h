#ifndef CONNECTION_STORE
#define CONNECTION_STORE

#include <unordered_map>
#include <mutex>
#include <list>
#include <map>

#include "shared_mutex.h"
#include "connection.h"
#include "types.h"

namespace swarm
{

class connection_store
{
private:
    typedef std::map<connection, OID, std::owner_less<connection>> connection_user_oid_t;
    typedef std::unordered_multimap<OID, connection> user_oid_connection_t;

    connection_user_oid_t m_connection_user_oid;
    user_oid_connection_t m_user_oid_connection;

    shared_mutex m_mutex;

public:
    void add(connection hdl, OID oid);
    void del(connection hdl);
    void del(OID oid);
    std::list<connection> get_connection(OID oid) const;
    OID get_oid(connection hdl) const;
};

} // end of namespace

#endif
