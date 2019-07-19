#ifndef SEND_BUFFER_H
#define SEND_BUFFER_H

#include <unordered_map>
#include <queue>
#include <mutex>

#include <types.h>

namespace swarm
{

class send_buffer
{
private:
    std::unordered_map<int, std::pair<bool, std::queue<sptr_cstr>>> m_buffer;
    std::mutex                                                      m_mutex;

public:
    bool empty(int socket_id);
    void push(int socket_id, sptr_cstr data);
    void set_flag(int socket_id);
    void unset_flag(int socket_id);
    bool check_flag(int socket_id);
    sptr_cstr pop(int socket_id);
    void remove(int socket_id);
};

} // end of namespace

#endif
