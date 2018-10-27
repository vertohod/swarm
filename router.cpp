#include "router.h"

namespace swarm
{

OID router::get_route(OID server_id)
{
    auto it = m_routes.find(server_id);
    if (it == m_routes.end()) return 0;

    auto step_it = it->second.begin();
    if (step_it == it->second.end()) return 0;

    return step_it->second;
}

void router::add_route(std::shared_ptr<std::list<OID>> route_ptr)
{
    if (route_ptr->size() == 0) return;

    auto gateaway_id = route_ptr->back();
    unsigned int steps = route_ptr->size() - 1;

    for (auto id : *route_ptr) {
        m_routes[id].insert(std::make_pair(steps, gateaway_id));
        --steps;
    }
}

void router::del_route(OID server_id)
{
    routes_t routes_temp;

    for (auto& pr : m_routes) {
        if (pr.first == server_id) continue;
        gateaway_t gateaway_temp;
        for (auto& gateaway_pr : pr.second) {
            if (gateaway_pr.second == server_id) continue;
            gateaway_temp.insert(std::make_pair(gateaway_pr.first, gateaway_pr.second));
        }
        if (gateaway_temp.size() > 0) {
            routes_temp[pr.first].swap(gateaway_temp);
        }
    }

    m_routes.swap(routes_temp);
}

} // end of namespace
