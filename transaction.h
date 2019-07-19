#ifndef TRANSACTION
#define TRANSACTION

#include <memory>
#include <vector>

#include <types.h>
#include "task.h"

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
    transaction();
    transaction(size_t number);
    std::shared_ptr<task::packets_t> get_packets();
    bool is_ready();
    bool is_finished();
    void fn(bool ok);

    void create_inorder_task(std::shared_ptr<task::packets_t>, std::vector<std::pair<OID, OID>>);
    void create_parallel_task(std::shared_ptr<task::packets_t>, std::vector<std::pair<OID, OID>>);

    void confirm(OID server_id);
};

} // end of namespace

#endif
