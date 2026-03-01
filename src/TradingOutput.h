#pragma once
#include <string>
#include <fstream>
#include <chrono>
#include "Statistics.h"

class TradingOutput
{
public:
    explicit TradingOutput(const std::string& fileName);
    ~TradingOutput();
    
    void clear();
    void addTimestamp();
    void addStatistics(const std::string& symbol, const Statistics& stat);
    
    static std::string formatTimestamp(std::chrono::system_clock::time_point tp);
    
private:
    std::string m_fileName;
    std::ofstream m_file;
};
