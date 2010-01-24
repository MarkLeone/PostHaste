#include "util/UtTimer.h"
#include <gtest/gtest.h>
#include <iostream>
#include <math.h>

class TestUtTimer : public testing::Test { };

TEST_F(TestUtTimer, TestGetFreq) 
{
    double freq = UtTimer::GetFreq();
    EXPECT_TRUE(freq > 0.0);
    // std::cout << "TestGetFreq: " << freq << std::endl;
}

TEST_F(TestUtTimer, TestGetTicks) 
{
    uint64_t ticks = UtTimer::GetTicks();
    EXPECT_TRUE(ticks > 0);
    // std::cout << "TestGetTicks: " << ticks << std::endl;
}

TEST_F(TestUtTimer, TestTimer) 
{
    UtTimer timer;
    timer.Start();
    double x;
    for (int i = 0; i < 100; ++i)
        x += sin(i);
    timer.Stop();
    EXPECT_TRUE(timer.GetElapsed() > 0.0); 
    // std::cout << "TestTimer: " << timer.GetElapsed() << std::endl;
}

int main(int argc, char **argv) 
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
