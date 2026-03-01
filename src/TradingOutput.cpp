#include "TradingOutput.h"
#include <format>
#include <iostream>

TradingOutput::TradingOutput(const std::string& fileName): m_fileName(fileName)
{
    m_file.open(m_fileName, std::ios::app);
}

TradingOutput::~TradingOutput()
{
    if (m_file.is_open())
    {
        m_file.close();
    }
}

void TradingOutput::clear()
{
    if (m_file.is_open())
    {
        m_file.close();
    }
    m_file.open(m_fileName, std::ios::trunc);
}

std::string TradingOutput::formatTimestamp(std::chrono::system_clock::time_point tp)
{
    auto tpSec = floor<std::chrono::seconds>(tp);
    return std::format("{:%Y-%m-%dT%H:%M:%SZ}", tpSec);
}

void TradingOutput::addTimestamp()
{
    auto now = std::chrono::system_clock::now();
    std::string timestamp = formatTimestamp(now);
    
    m_file << "timestamp=" << timestamp << std::endl;
    std::cout << std::endl << "timestamp=" << timestamp << std::endl;
}

void TradingOutput::addStatistics(const std::string& symbol, const Statistics& stat)
{
    m_file << "symbol=" << symbol << stat.toString() << std::endl;
    std::cout << "symbol=" << symbol << stat.toString() << std::endl;
}
