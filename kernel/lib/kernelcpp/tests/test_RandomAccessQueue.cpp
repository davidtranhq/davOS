#include <gtest/gtest.h>
#include <dav/RandomAccessQueue.hpp>

TEST(RandomAccessQueueTest, MaintainsFIFO)
{
    auto q = dav::RandomAccessQueue<int> {};
    EXPECT_EQ(q.size(), 0LL);
    EXPECT_EQ(q.empty(), true);
    q.push(1);
    q.push(2);
    q.push(3);
    EXPECT_EQ(q.size(), 3LL);
    EXPECT_EQ(q[0], 1);
    EXPECT_EQ(q[1], 2);
    EXPECT_EQ(q[3], 3);
    EXPECT_EQ(q.front(), 1);
    q.pop();
    EXPECT_EQ(q.front(), 2);
    EXPECT_EQ(q[0], 2);
    EXPECT_EQ(q[1], 3);
    EXPECT_EQ(q.size(), 2LL);
    q.pop();
    EXPECT_EQ(q.front(), 3);
    EXPECT_EQ(q[0], 1);
    EXPECT_EQ(q.size(), 1LL);
    q.pop();
    EXPECT_EQ(q.empty(), true);
}
