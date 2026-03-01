#pragma once
#include <string>

struct MessageData
{
    bool m_isValid{false};
    double m_price{0.0};
    double m_quantity{0.0};
    bool m_sellerInitiated{false};
};

class TradingMessageParser
{
public:
    explicit TradingMessageParser(const std::string& sybmol);
    
    MessageData parsMessage(const std::string& msg) const;
    
private:
    std::string m_symbol;
};
