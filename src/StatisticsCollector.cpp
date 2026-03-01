#include "StatisticsCollector.h"
#include <iostream>

StatisticsCollector::StatisticsCollector(ThreadSafeMessageQueue& queue, const std::string& symbol)
    : m_msgQueue(queue), m_parser(symbol), m_running(false)
{
}

StatisticsCollector::~StatisticsCollector()
{
    if(isRunning())
    {
        stop();
    }
}

void StatisticsCollector::start()
{
    m_running = true;
    m_worker = std::thread(&StatisticsCollector::run, this);
}

void StatisticsCollector::stop()
{
    m_running = false;
    if (m_worker.joinable())
    {
	m_worker.join();
    }
}

bool StatisticsCollector::isRunning() const
{
    return m_running; 
}

void StatisticsCollector::resetStatistics()
{
    std::lock_guard lock(m_mutex);
    m_statistics.reset();
}

Statistics StatisticsCollector::getStatistics()
{
    std::lock_guard lock(m_mutex);
    return m_statistics;
}

Statistics StatisticsCollector::getAndResetStatistics()
{
    std::lock_guard lock(m_mutex);
    Statistics result;
    std::swap(result, m_statistics);
    return result;
}

void StatisticsCollector::run()
{
    while (m_running)
    {
	while (auto msg = m_msgQueue.try_pop())
	{  
	    MessageData data = m_parser.parsMessage(*msg);
	    if(data.m_isValid)
	    {
	        updateStatistics(data);
	    }
	    else
	    {
	        std::cerr << "Invalid message data received." << std::endl;
            }
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void StatisticsCollector::updateStatistics(const MessageData& data)
{
    std::lock_guard lock(m_mutex);
    m_statistics.update(data.m_price, data.m_quantity, data.m_sellerInitiated);
}
