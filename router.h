#ifndef ROUTER_H
#define ROUTER_H

#include <unordered_map>
#include <memory>
#include <list>
#include <set>

#include "types.h"

namespace swarm
{

class router
{
private:
    typedef std::set<std::pair<unsigned int, OID>> gateaway_t;
    typedef std::unordered_map<OID, gateaway_t> routes_t;
    routes_t m_routes;

public:
    OID get_route(OID server_id);
    void add_route(std::shared_ptr<std::list<OID>> route);
    void del_route(OID server_id);
};

} // end of namespace

#endif
