#include <limits.h>
#include "../src/client.cpp"
#include "gtest/gtest.h"
#include <utility>
typedef std::pair<double,double> Pair;  

namespace
{
    TEST(ClientTest, CountBasic)
    {
        Client c;
        c.process(5);
        c.process(7);
        EXPECT_EQ(c.count,2);
    }

    TEST(ClientTest, SumBasic)
    {
        Client c;
        c.process(5);
        c.process(7);
        EXPECT_EQ(c.sum,12);
    }

    TEST(ClientTest, AvgBasic)
    {
        Client c;
        c.process(5);
        double avg = c.process(7).first;
        EXPECT_EQ(avg,6);
    }

}