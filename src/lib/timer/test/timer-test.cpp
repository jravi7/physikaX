#include "timer/timer.h"

#include <chrono>  // std::chrono::seconds
#include <thread>  // std::this_thread::sleep_for

#include "gtest/gtest.h"

namespace {

using namespace std;
using namespace physika;

float const kEpsilon = 0.1f;

TEST(TimerTest, InitializeTimer)
{
    Timer timer;
    EXPECT_FLOAT_EQ(0.0f, timer.TotalRunningTime());
}

TEST(TimerTest, GetTotalRunningTimeAfterATick)
{
    Timer timer;
    timer.Start();
    this_thread::sleep_for(chrono::seconds(1));
    timer.Tick();
    ASSERT_NEAR(1.0f, timer.TotalRunningTime(), kEpsilon);
}

TEST(TimerTest, GetTotalRunningTimeAfterStop)
{
    Timer timer;
    timer.Start();

    this_thread::sleep_for(chrono::seconds(1));
    timer.Tick();
    ASSERT_NEAR(1.0f, timer.TotalRunningTime(), kEpsilon);

    this_thread::sleep_for(chrono::seconds(1));
    timer.Tick();
    ASSERT_NEAR(2.0f, timer.TotalRunningTime(), kEpsilon);
    timer.Stop();

    this_thread::sleep_for(chrono::seconds(1));
    timer.Tick();
    ASSERT_GT(3.0f, timer.TotalRunningTime());
}

TEST(TimerTest, GetTotalRunningTimeAfterReset)
{
    Timer timer;
    timer.Start();
    this_thread::sleep_for(chrono::seconds(1));
    timer.Tick();
    ASSERT_NEAR(1.0f, timer.TotalRunningTime(), kEpsilon);

    timer.Stop();
    this_thread::sleep_for(chrono::seconds(1));
    timer.Tick();
    ASSERT_GT(2.0f, timer.TotalRunningTime());

    timer.Reset();
    ASSERT_FLOAT_EQ(0.0f, timer.TotalRunningTime());
}

TEST(TimerTest, CallResetBetweenStartAndStop)
{
    Timer timer;
    timer.Start();
    this_thread::sleep_for(chrono::seconds(1));
    timer.Tick();
    ASSERT_NEAR(1.0f, timer.TotalRunningTime(), kEpsilon);

    timer.Reset();
    ASSERT_NE(0.0f, timer.TotalRunningTime());

    timer.Stop();
    this_thread::sleep_for(chrono::seconds(1));
    timer.Tick();
    ASSERT_GE(2.0f, timer.TotalRunningTime());
}

TEST(TimerTest, GetDelta)
{
    Timer timer;
    timer.Start();
    this_thread::sleep_for(chrono::seconds(1));
    timer.Tick();

    ASSERT_NEAR(1.0f, timer.Delta(), kEpsilon);
    ASSERT_NEAR(1.0f, timer.TotalRunningTime(), kEpsilon);

    this_thread::sleep_for(chrono::seconds(2));
    timer.Tick();

    ASSERT_NEAR(2.0f, timer.Delta(), kEpsilon);

    timer.Stop();
    this_thread::sleep_for(chrono::seconds(1));
    timer.Tick();

    ASSERT_NEAR(3.0f, timer.TotalRunningTime(), kEpsilon);
    ASSERT_NEAR(0.0f, timer.Delta(), kEpsilon);

    timer.Reset();

    ASSERT_FLOAT_EQ(0.0f, timer.Delta());
    ASSERT_FLOAT_EQ(0.0f, timer.TotalRunningTime());
}

TEST(TimerTest, CallStartTwice)
{
    Timer timer;
    timer.Start();
    this_thread::sleep_for(chrono::seconds(1));
    timer.Tick();
    timer.Start();
    ASSERT_NEAR(1.0f, timer.Delta(), kEpsilon);
    ASSERT_NEAR(1.0f, timer.TotalRunningTime(), kEpsilon);

    timer.Stop();
    timer.Reset();

    ASSERT_FLOAT_EQ(0.0f, timer.Delta());
    ASSERT_FLOAT_EQ(0.0f, timer.TotalRunningTime());
}

TEST(TimerTest, CallStartStopStartStop)
{
    Timer timer;
    timer.Start();
    this_thread::sleep_for(chrono::seconds(1));
    timer.Tick();

    ASSERT_NEAR(1.0f, timer.Delta(), kEpsilon);
    ASSERT_NEAR(1.0f, timer.TotalRunningTime(), kEpsilon);

    this_thread::sleep_for(chrono::seconds(1));
    timer.Tick();

    ASSERT_NEAR(1.0f, timer.Delta(), kEpsilon);
    ASSERT_NEAR(2.0f, timer.TotalRunningTime(), kEpsilon);

    timer.Stop();

    //! Wait before restarting timer
    this_thread::sleep_for(chrono::seconds(5));

    timer.Start();
    this_thread::sleep_for(chrono::seconds(1));
    timer.Tick();

    ASSERT_NEAR(1.0f, timer.Delta(), kEpsilon);
    ASSERT_NEAR(3.0f, timer.TotalRunningTime(), kEpsilon);

    this_thread::sleep_for(chrono::seconds(1));
    timer.Tick();

    ASSERT_NEAR(1.0f, timer.Delta(), kEpsilon);
    ASSERT_NEAR(4.0f, timer.TotalRunningTime(), kEpsilon);

    timer.Stop();

    timer.Reset();

    ASSERT_FLOAT_EQ(0.0f, timer.Delta());
    ASSERT_FLOAT_EQ(0.0f, timer.TotalRunningTime());
}

}  // namespace