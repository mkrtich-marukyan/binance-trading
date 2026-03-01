#pragma once
#include <thread>
#include <string>
#include "ThreadSafeMessageQueue.h"

class BinanceWsApi
{
public:
    explicit BinanceWsApi(ThreadSafeMessageQueue& queue, const std::string& symbol);
    ~BinanceWsApi();
    void start();
    void stop();
    
    bool isRunning() const;

private:
    void run();

private:
    ThreadSafeMessageQueue& m_msgQueue;
    std::string m_symbol;
    std::atomic<bool> m_running;
    std::thread m_worker;
};
