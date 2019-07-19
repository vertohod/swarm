#include "transaction.h"

namespace swarm
{

// TODO remove
transaction::transaction() : m_number(0), m_index(0) {}

transaction::transaction(size_t number) : m_number(number), m_index(0) {}

std::shared_ptr<task::packets_t> transaction::get_packets()
{
    std::shared_ptr<task::packets_t> res;
/* TODO
    if (m_parallel) {
        res = m_packets;
        for (auto packet_ptr : *m_packets) {
            m_need_confirmation.insert(packet_ptr->get_recipient_id());
        }
    } else {
        res = std::make_shared<task::packets_t>();
        if (m_index < m_packets->size()) {
            auto packet_ptr = m_packets->at(m_index++);
            res->push_back(packet_ptr );
            m_need_confirmation.insert(packet_ptr->get_recipient_id());
        }
    }

    */
    return res;
}

bool transaction::is_ready()
{
//    return m_need_confirmation.empty(); TODO
    return true;
}

void transaction::confirm(OID server_id)
{
    /* TODO
    m_need_confirmation.erase(server_id);
    if (!m_parallel) ++m_index;
    */
}

bool transaction::is_finished()
{
    /*
    if (m_tasks.size() == 0) return true;

    return ((m_index + 1) == m_tasks.size() && m_tasks[m_index]->is_finished());
    */

    return true;
}

void transaction::fn(bool ok)
{
//    m_fn(ok);
}

} // end of namespace
