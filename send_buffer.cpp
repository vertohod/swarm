#include "send_buffer.h"

namespace swarm
{

bool send_buffer::empty(int socket_id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_buffer.find(socket_id);
    if (it != m_buffer.end()) return it->second.second.empty();
    return true;
}

void send_buffer::push(int socket_id, sptr_cstr data)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_buffer.find(socket_id);
    if (it == m_buffer.end()) {
        auto pr = m_buffer.insert(std::make_pair(socket_id, std::make_pair(false, std::queue<sptr_cstr>())));
        if (pr.second) pr.first->second.second.push(data);
        else throw std::runtime_error("Can't insert structure for keeping datas");
    } else {
        it->second.first = false;
        it->second.second.push(data);
    }
}

void send_buffer::set_flag(int socket_id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_buffer.find(socket_id);
    if (it != m_buffer.end()) {
        if (it->second.second.empty()) it->second.first = true;
    }
}

void send_buffer::unset_flag(int socket_id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_buffer.find(socket_id);
    if (it == m_buffer.end()) {
        m_buffer.insert(std::make_pair(socket_id, std::make_pair(false, std::queue<sptr_cstr>())));
    } else {
        it->second.first = false;
    }
}

bool send_buffer::check_flag(int socket_id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_buffer.find(socket_id);
    if (it != m_buffer.end()) return  it->second.first;
    return true;
}

sptr_cstr send_buffer::pop(int socket_id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_buffer.find(socket_id);
    if (it != m_buffer.end()) {
        if (!it->second.second.empty()) {
            auto data = it->second.second.front();
            it->second.second.pop();
            return data;
        }
    }
    return "";
}

void send_buffer::remove(int socket_id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_buffer.find(socket_id);
    if (it != m_buffer.end()) m_buffer.erase(it);
}

} // end of namespace
