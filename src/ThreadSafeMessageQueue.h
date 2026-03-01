#pragma once
#include <queue>
#include <string>
#include <mutex>
#include <optional>

class ThreadSafeMessageQueue
{
public:
    void push(std::string msg);
    std::optional<std::string> try_pop();

private:
    std::queue<std::string> m_queue;
    std::mutex m_mutex;
};
