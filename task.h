#ifndef TASK_H
#define TASK_H

namespace swarm
{

class task
{
public:
    typedef std::vector<std::shared_ptr<packet>> packets_t;

private:
    std::shared_ptr<packets_t>  m_packets;
    const bool                  m_parallel;
    size_t                      m_index; // Используется, если m_parallel = false

    std::unordered_set<OID>     m_need_confirmation;

public:
    task(bool parallel);
    void set_packet(std::shared_ptr<packet> packet_ptr, std::set<std::pair<OID, OID>>& servers);
    std::shared_ptr<packets_t> get_packets();
    void mark_ok(OID oid);
    bool is_ready();
    bool is_finished();
    void confirm(OID server_id);
};

} // end of namespace

#endif
