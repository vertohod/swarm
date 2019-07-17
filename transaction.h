#ifndef TRANSACTION
#define TRANSACTION


namespace swarm
{

class transaction
{
private:
    const size_t    m_number;
//    function_t      m_fn;
    time_tt         m_timeout;

    std::vector<std::shared_ptr<task>>  m_tasks;
    size_t                              m_index;

public:
    transaction(size_t number);
    std::shared_ptr<task::packets_t> get_packets();
    bool is_finished();
    void fn(bool ok);

    create_inorder_task(std::shared_ptr<task::packets_t>, std::vector<std::pair<OID, OID>>);
    create_parallel_task(std::shared_ptr<task::packets_t>, std::vector<std::pair<OID, OID>>);
};

} // end of namespace

#endif
