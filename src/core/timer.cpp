#include "core/timer.h"

#include <Windows.h>
namespace physika::core {

Timer::Timer() : mPreviousTime{ 0 }, mTotalRunningTime{ 0.0f }, mStopped{ true }
{
    int64_t frequency;
    QueryPerformanceCounter((LARGE_INTEGER*)&mPreviousTime);
    QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);  // 1/T
    mSecondsPerCount = (float)1 / frequency;                // 1/F
}

float Timer::TotalRunningTime()
{
    return mTotalRunningTime;
}

float Timer::Delta()
{
    return mDelta;
}

void Timer::Start()
{
    if (mStopped) {
        int64_t currentTime;
        QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
        mPreviousTime = currentTime;
        mStopped      = false;
    }
}

void Timer::Stop()
{
    if (mStopped) {
        return;
    }
    mStopped = true;
    int64_t currentTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
}

void Timer::Reset()
{
    if (!mStopped) {
        return;
    }
    mTotalRunningTime = 0.0f;
    mDelta            = 0.0f;
    mPreviousTime     = 0;
}

void Timer::Tick()
{
    if (mStopped) {
        mDelta = 0.0f;
        return;
    }
    int64_t currentTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

    int64_t delta = 0;
    delta         = currentTime - mPreviousTime;

    mDelta = (float)(delta)*mSecondsPerCount;
    mTotalRunningTime += mDelta;
    mPreviousTime = currentTime;
}

}  // namespace physika::core