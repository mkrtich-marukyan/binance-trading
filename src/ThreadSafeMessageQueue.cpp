#include "ThreadSafeMessageQueue.h"

void ThreadSafeMessageQueue::push(std::string msg)
{
    std::lock_guard lock(m_mutex);
    m_queue.push(std::move(msg));
}

std::optional<std::string> ThreadSafeMessageQueue::try_pop()
{
    std::lock_guard lock(m_mutex);
    if (m_queue.empty())
    {
	return std::nullopt;
    }

    std::string msg = std::move(m_queue.front());
    m_queue.pop();
    return msg;
}
