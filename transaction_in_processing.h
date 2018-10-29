#ifndef TRANSACTION_IN_PROCESSING_H
#define TRANSACTION_IN_PROCESSING_H

namespace swarm
{

class transactions_in_processing
{
private:
    std::unordered_map<size_t, std::shared_ptr<transaction>> m_transactions;
    // finish_time, transaction_id
    std::map<time_tt, size_t> m_timeouts;

public:
    transactions_in_processing();
    void add_transaction(std::shared_ptr<transaction> transaction_ptr);
    std::shared_ptr<transaction> get_transaction(size_t tr_id);
    void remove_transaction(size_t tr_id);
    void remove_transaction(std::shared_ptr<transaction> transaction_ptr);

private:
    void thread_function(); // отслеживает транзакции, не выполненные до истечении времени
};


} // end of namespace

#endif
