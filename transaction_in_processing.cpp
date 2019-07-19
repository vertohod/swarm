#include "transaction_in_processing.h"

#include <memory>

namespace swarm
{

transactions_in_processing::transactions_in_processing()
{
}

void transactions_in_processing::add_transaction(std::shared_ptr<transaction> transaction_ptr)
{
}

std::shared_ptr<transaction> transactions_in_processing::get_transaction(size_t tr_id)
{
    return std::make_shared<transaction>();
}

void transactions_in_processing::remove_transaction(size_t tr_id)
{
}

void transactions_in_processing::remove_transaction(std::shared_ptr<transaction> transaction_ptr)
{
}

void transactions_in_processing::thread_function()
{
}


} // end of namespace
