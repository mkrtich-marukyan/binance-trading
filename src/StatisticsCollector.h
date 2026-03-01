#pragma once
#include <thread>
#include <string>
#include <mutex>
#include "ThreadSafeMessageQueue.h"
#include "TradingMessageParser.h"
#include "Statistics.h"

class StatisticsCollector
{
public:
    explicit StatisticsCollector(ThreadSafeMessageQueue& queue, const std::string& symbol);
    ~StatisticsCollector();
    void start();
    void stop();
    bool isRunning() const;

    void resetStatistics();
    Statistics getStatistics();
    Statistics getAndResetStatistics();

private:
    void run();
    void updateStatistics(const MessageData& data);
    
private:
    ThreadSafeMessageQueue& m_msgQueue;
    TradingMessageParser m_parser;
    std::atomic<bool> m_running;
    std::thread m_worker;
    Statistics m_statistics;
    std::mutex m_mutex;
};
