#pragma once
#include <vector>
#include <string>
#include <memory>
#include "BinanceWsApi.h"
#include "StatisticsCollector.h"
#include "ThreadSafeMessageQueue.h"
#include "TradingOutput.h"

struct TradingPair
{
    std::string m_symbol;
    std::unique_ptr<ThreadSafeMessageQueue> m_queue;
    std::unique_ptr<StatisticsCollector> m_stat;
    std::unique_ptr<BinanceWsApi> m_api;
};

class BinanceTradingApp
{
public:
    explicit BinanceTradingApp(const std::vector<std::string>& symbols, unsigned int provideStatisticsTime, const std::string& outFileName);
    ~BinanceTradingApp();
    
    void exec();

private:
    unsigned int m_provideStatisticsTime;
    std::vector<TradingPair> m_tradingPairs;
    TradingOutput m_output;
};
