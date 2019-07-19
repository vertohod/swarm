#include "task.h"

namespace swarm
{

task::task(bool parallel) : m_parallel(parallel), m_index(0)
{
    m_packets = std::make_shared<packets_t>();
}

void task::set_packet(std::shared_ptr<packet> packet_ptr, std::set<std::pair<OID, OID>>& servers)
{
    for (auto& pr : servers) {
        auto packet_new_ptr = std::dynamic_pointer_cast<packet>(packet_ptr->make_copy());
        packet_new_ptr->set_gateaway_id(pr.first);
        packet_new_ptr->set_recipient_id(pr.second);
        m_packets->push_back(packet_new_ptr);
    }
}

} // end of namespace
