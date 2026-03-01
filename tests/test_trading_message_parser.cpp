#include <gtest/gtest.h>
#include "../src/TradingMessageParser.h"

TEST(TradingMessageParserTest, ParsVaildMessage)
{
    TradingMessageParser parser("BNBBTC");
    std::string msg{"{\"e\": \"trade\", \"E\": 1672515782136, \"s\": \"BNBBTC\", \"t\": 12345, \"p\": \"0.001\", \"q\": \"100\", \"T\": 1672515782136, \"m\": true, \"M\": true}"};
    MessageData data;
    EXPECT_NO_THROW(data = parser.parsMessage(msg));
    EXPECT_TRUE(data.m_isValid);
    EXPECT_DOUBLE_EQ(data.m_price, 0.001);
    EXPECT_DOUBLE_EQ(data.m_quantity, 100.0);
    EXPECT_TRUE(data.m_sellerInitiated);
}

TEST(TradingMessageParserTest, ParsUnknownEventMessage)
{
    TradingMessageParser parser("BNBBTC");
    std::string msg{"{\"e\": \"aggTrade\", \"E\": 1672515782136, \"s\": \"BNBBTC\"}"};
    MessageData data;
    EXPECT_NO_THROW(data = parser.parsMessage(msg));
    EXPECT_FALSE(data.m_isValid);
}

TEST(TradingMessageParserTest, ParsIncorrectSymbolMessage)
{
    TradingMessageParser parser("BNBBTC");
    std::string msg{"{\"e\": \"trade\", \"E\": 1672515782136, \"s\": \"BTCUSDT\"}"};
    MessageData data;
    EXPECT_NO_THROW(data = parser.parsMessage(msg));
    EXPECT_FALSE(data.m_isValid);
}

TEST(TradingMessageParserTest, ParsMissingParameterMessage)
{
    TradingMessageParser parser("BNBBTC");
    std::string msg{"{\"e\": \"trade\", \"s\": \"BNBBTC\"}"};
    MessageData data;
    EXPECT_NO_THROW(data = parser.parsMessage(msg));
    EXPECT_FALSE(data.m_isValid);
}

TEST(TradingMessageParserTest, ParsCorruptedMessage)
{
    TradingMessageParser parser("BNBBTC");
    std::string msg{"Corrupted message"};
    MessageData data;
    EXPECT_NO_THROW(data = parser.parsMessage(msg));
    EXPECT_FALSE(data.m_isValid);
}
