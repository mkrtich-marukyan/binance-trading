#include <gtest/gtest.h>
#include "../src/Statistics.h"

TEST(StatisticsTest, Update)
{
    Statistics stats;

    stats.update(100.0, 2.0, false);  // buy
    stats.update(110.0, 1.0, true);   // sell
    stats.update(110.1, 0.5, false);  // buy
    stats.update(108.0, 0.8, false);  // buy
    stats.update(99.9, 1.6, true);   // sell

    EXPECT_TRUE(stats.isValid());
    EXPECT_EQ(stats.trades(), 5);
    EXPECT_DOUBLE_EQ(stats.totalVolume(), 100.0*2.0 + 110.0*1.0 + 110.1*0.5 + 108.0*0.8 + 99.9*1.6);
    EXPECT_DOUBLE_EQ(stats.minPrice(), 99.9);
    EXPECT_DOUBLE_EQ(stats.maxPrice(), 110.1);
    EXPECT_EQ(stats.buyerInitiated(), 3);
    EXPECT_EQ(stats.sellerInitiated(), 2);
}

TEST(StatisticsTest, Reset)
{
    Statistics stats;

    // some updates and then reset
    stats.update(100.0, 2.0, false);
    stats.update(110.0, 1.0, true);
    
    stats.reset();

    EXPECT_FALSE(stats.isValid());
    EXPECT_EQ(stats.trades(), 0);
    EXPECT_DOUBLE_EQ(stats.totalVolume(), 0.0);
    EXPECT_DOUBLE_EQ(stats.minPrice(), std::numeric_limits<double>::max());
    EXPECT_DOUBLE_EQ(stats.maxPrice(), std::numeric_limits<double>::lowest());
    EXPECT_EQ(stats.buyerInitiated(), 0);
    EXPECT_EQ(stats.sellerInitiated(), 0);
}

TEST(StatisticsTest, ToString)
{
    Statistics stats;

    stats.update(77.77777, 2.0, false);
    stats.update(111.11111, 1.0, true);
    
    EXPECT_TRUE(stats.isValid());
    EXPECT_EQ(stats.toString(), " trades=2 totalvolume=266.67 min=77.78 max=111.11 buy=1 sell=1");
}
