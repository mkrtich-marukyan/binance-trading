#include "TradingMessageParser.h"
#include <nlohmann/json.hpp>

TradingMessageParser::TradingMessageParser(const std::string& sybmol): m_symbol(sybmol)
{
}

MessageData TradingMessageParser::parsMessage(const std::string& msg) const
{
    /* required JSON message format
    {
        "e": "trade",           // (string) Event type
        "s": "BNBBTC",          // (string) Symbol
        "p": "0.001",           // (double) Price
        "q": "100",             // (double) Quantity
        "m": true,              // (boolean)Is the buyer the market maker?
    }
    */
    try
    {
        auto jsonData = nlohmann::json::parse(msg);
                
        if (jsonData.at("e") != "trade")
        {
            return MessageData();
        }
        if (jsonData.at("s") != m_symbol)
        {
            return MessageData();
        }
        
        MessageData data;
        data.m_price = std::stod(jsonData.at("p").get<std::string>());
        data.m_quantity   = std::stod(jsonData.at("q").get<std::string>());
        data.m_sellerInitiated  = jsonData.at("m").get<bool>();
        data.m_isValid = true;
        return data;
    }
    catch (...)
    {
        // never crash on invalid message
        return MessageData();
    }
}
