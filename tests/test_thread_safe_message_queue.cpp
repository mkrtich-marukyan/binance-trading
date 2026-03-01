#include <gtest/gtest.h>
#include "../src/ThreadSafeMessageQueue.h"

TEST(ThreadSafeMessageQueueTest, PushAndPop)
{
    ThreadSafeMessageQueue queue;

    queue.push("msg");

    auto msg = queue.try_pop();

    ASSERT_TRUE(msg.has_value());
    EXPECT_EQ(*msg, "msg");
}

TEST(ThreadSafeMessageQueueTest, EmptyQueueTryPop)
{
    ThreadSafeMessageQueue queue;

    auto msg = queue.try_pop();

    EXPECT_FALSE(msg.has_value());
}

TEST(ThreadSafeMessageQueueTest, MessagesOrder)
{
    ThreadSafeMessageQueue queue;

    queue.push("msg1");
    queue.push("msg2");
    EXPECT_EQ(queue.try_pop().value(), "msg1");
    queue.push("msg3");
    EXPECT_EQ(queue.try_pop().value(), "msg2");
    EXPECT_EQ(queue.try_pop().value(), "msg3");
    EXPECT_FALSE(queue.try_pop().has_value());
}

TEST(ThreadSafeMessageQueueTest, MultiThreadPushAndPop)
{
    ThreadSafeMessageQueue queue;

    std::thread push1([&]()
    {
        for(int i = 0; i < 100000; ++i)
        {
            queue.push("msg");
        }
    });

    std::thread push2([&]()
    {
        for(int i = 0; i < 100000; ++i)
        {
            queue.push("msg");
        }
    });
    
    int count = 0;
    std::thread pop([&]()
    {
        // finish if three times nothing to pop
        for(int failedTryPopCount = 0; failedTryPopCount < 2; ++failedTryPopCount)
        {
            while(queue.try_pop())
            {
                ++count;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    });

    push1.join();
    push2.join();
    pop.join();

    EXPECT_EQ(count, 200000);
}
