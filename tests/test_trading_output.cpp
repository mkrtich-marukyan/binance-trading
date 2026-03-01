#include <gtest/gtest.h>
#include <chrono>
#include "../src/TradingOutput.h"

TEST(TradingOutputTest, OutputTimestampFromat)
{
    using namespace std::chrono;
    sys_seconds tp = sys_days{year{2026}/3/1} + 4h + 8min + 16s;
    std::string timestamp = TradingOutput::formatTimestamp(tp);
    EXPECT_EQ(timestamp, "2026-03-01T04:08:16Z");
}
