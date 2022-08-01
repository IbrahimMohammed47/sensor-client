#include <limits.h>
#include "../src/concurrent_queue.cpp"
#include "gtest/gtest.h"
#include <utility>

typedef std::pair<double,double> Pair;  

namespace
{
   TEST(SubscriberQueue, SizeAfterPush)
    {
        ThreadSafeDeque<int> q;
        q.push(1);
        q.push(2);
        q.push(3);
        EXPECT_EQ(q.size(),3);
    }
    TEST(SubscriberQueue, SizeAfterPop)
    {
        ThreadSafeDeque<int> q;
        q.push(1);
        q.push(2);
        q.pop();
        EXPECT_EQ(q.size(),1);
    }
    TEST(SubscriberQueue, ValueOfPop)
    {
        ThreadSafeDeque<int> q;
        q.push(1);
        q.push(2);
        q.push(3);
        int got = q.pop();
        EXPECT_EQ(got,1);
    }
    TEST(SubscriberQueue, SizeAfterRemove)
    {
        ThreadSafeDeque<int> q;
        q.push(1);
        q.push(2);
        q.remove(2);
        EXPECT_EQ(q.size(),1);
    }
    
\
}