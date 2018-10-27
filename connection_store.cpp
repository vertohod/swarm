#include <stdexcept>

#include "connection_store.h"
#include "log.h"

namespace swarm
{

void connection_store::add(connection hdl, OID oid)
{
    unique_lock<shared_mutex> lock(m_mutex);

    auto it = m_connection_user_oid.find(hdl);
    if (it == m_connection_user_oid.end()) {
        m_connection_user_oid.insert(std::make_pair(hdl, oid));
        m_user_oid_connection.insert(std::make_pair(oid, hdl));
    } else throw std::runtime_error("(connection_store::add) There is the connection in the store already");

    lo::l(lo::DEBUG) << "(connection_store::add), m_connection_user_oid.size(): " << m_connection_user_oid.size() << ", m_user_oid_connection.size(): " << m_user_oid_connection.size();
}

void connection_store::del(connection hdl)
{
    unique_lock<shared_mutex> lock(m_mutex);

    auto it = m_connection_user_oid.find(hdl);
    if (it != m_connection_user_oid.end()) {
        auto pr = m_user_oid_connection.equal_range(it->second);
        for (auto u_it = pr.first; u_it != pr.second; ++u_it) {
            if (!std::owner_less<connection>()(u_it->second, hdl) && !std::owner_less<connection>()(hdl, u_it->second)) {
                m_user_oid_connection.erase(u_it);
                break;
            }
        }
        m_connection_user_oid.erase(it);
    } else throw std::runtime_error("(connection_store::del) Attempt to remove a connection which isn't exist in the store");

    lo::l(lo::DEBUG) << "(connection_store::del (connection)), m_connection_user_oid.size(): " << m_connection_user_oid.size() << ", m_user_oid_connection.size(): " << m_user_oid_connection.size();
}

void connection_store::del(OID oid)
{
    unique_lock<shared_mutex> lock(m_mutex);

    auto pr = m_user_oid_connection.equal_range(oid);
    for (auto u_it = pr.first; u_it != pr.second; ++u_it) {
        auto it = m_connection_user_oid.find(u_it->second);
        if (it != m_connection_user_oid.end()) m_connection_user_oid.erase(it);
    }
    m_user_oid_connection.erase(pr.first, pr.second);

    lo::l(lo::DEBUG) << "(connection_store::del (OID)), m_connection_user_oid.size(): " << m_connection_user_oid.size() << ", m_user_oid_connection.size(): " << m_user_oid_connection.size();
}

std::list<connection> connection_store::get_connection(OID oid) const
{
    shared_lock<shared_mutex> lock(m_mutex);

    std::list<connection> res;
    auto pr = m_user_oid_connection.equal_range(oid);
    for (auto u_it = pr.first; u_it != pr.second; ++u_it) {
        res.push_back(u_it->second);
    }
    return res;
}

OID connection_store::get_oid(connection hdl) const
{
    shared_lock<shared_mutex> lock(m_mutex);

    auto it = m_connection_user_oid.find(hdl);
    if (it == m_connection_user_oid.end()) throw std::runtime_error("(connection_store::get_oid) The connection wasn't found in the store");

    return it->second;
}

} // end of namespace
