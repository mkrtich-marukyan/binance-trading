#include "BinanceTradingApp.h"
#include <chrono>

BinanceTradingApp::BinanceTradingApp(const std::vector<std::string>& symbols, unsigned int provideStatisticsTime, const std::string& outFileName)
    : m_provideStatisticsTime(provideStatisticsTime), m_output(outFileName)
{
    // create trading staff for every trending pair
    for (const std::string& symbol : symbols) 
    {
        TradingPair trading;
        trading.m_symbol = symbol;
        trading.m_queue = std::make_unique<ThreadSafeMessageQueue>();
        trading.m_stat = std::make_unique<StatisticsCollector>(*trading.m_queue.get(), symbol);
        trading.m_api = std::make_unique<BinanceWsApi>(*trading.m_queue.get(), symbol);
        
        m_tradingPairs.push_back(std::move(trading));
    }
}

BinanceTradingApp::~BinanceTradingApp()
{
    for (auto it = m_tradingPairs.begin(); it != m_tradingPairs.end(); ++it) 
    {
        if(it->m_api->isRunning())
        {
            it->m_api->stop();
        }
        if(it->m_stat->isRunning())
        {
            it->m_stat->stop();
        }
    }
    m_tradingPairs.clear();
}

void BinanceTradingApp::exec()
{
    if(m_tradingPairs.empty())
    {
        // nothing to execute
        return;
    }
    
    auto startTrading = [](std::vector<TradingPair>& tradingPairs) 
    {
        for (auto it = tradingPairs.begin(); it != tradingPairs.end(); ++it) 
        {
            it->m_stat->start();
            it->m_api->start();
        }
    };
    
    auto stopTrading = [](std::vector<TradingPair>& tradingPairs) 
    {
        for (auto it = tradingPairs.begin(); it != tradingPairs.end(); ++it) 
        {
            it->m_api->stop();
            it->m_stat->stop();
        }
    };
    
    auto checkAndRestartIfNeeded = [](TradingPair& tradingPair) 
    {
        if(!tradingPair.m_api->isRunning() || !tradingPair.m_stat->isRunning())
        {
            tradingPair.m_api->stop();
            tradingPair.m_stat->stop();
            tradingPair.m_stat->start();
            tradingPair.m_api->start();
        }
    };
    
    // clean the output file before starting
    m_output.clear();
    
    // start trading and fix the time to reconnect after 23h
    startTrading(m_tradingPairs);
    auto startTime = std::chrono::steady_clock::now();
    
    // main thread loop
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(m_provideStatisticsTime));
        
        m_output.addTimestamp();
        for (auto it = m_tradingPairs.begin(); it != m_tradingPairs.end(); ++it) 
        {
            Statistics st = it->m_stat->getAndResetStatistics();
            // output current statistics if they are valid
            if(st.isValid())
            {
                m_output.addStatistics(it->m_symbol, st);
            }
            
            checkAndRestartIfNeeded(*it);
        }
        
        // reconnect before 24h limit
        if (std::chrono::steady_clock::now() - startTime > std::chrono::hours(23))
        {
            stopTrading(m_tradingPairs);
            m_output.clear();
            startTrading(m_tradingPairs);
            startTime = std::chrono::steady_clock::now();
        }
    }
}
