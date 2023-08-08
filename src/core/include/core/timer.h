#pragma once
#include <stdint.h>  // int64_t
namespace physika::core {

/**
 * @brief Represent a timer object which can start/stop a timer
 *        and sample time at user-defined intervals
 *
 */
class Timer
{
public:
    /**
     * @brief Construct a new Timer object
     *
     */
    Timer();

    /**
     * @brief Returns the total duration in seconds
     *        for every Tick between pairs of Start
     *        and Stop calls.
     */
    float TotalRunningTime();

    /**
     * @brief Call this function to sample time
     *        between intervals.
     */
    void Tick();

    /**
     * @brief Starts counting of running time.
     *
     */
    void Start();

    /**
     * @brief Stops counting of running time.
     */
    void Stop();

    /**
     * @brief Resets the timer object and
     *        sets all counters to zero
     *        Note: Reset can be called only
     *        after the timer is stopped else it
     *        would silently fail.
     */
    void Reset();

    /**
     * @brief Returns the duration in
     *        seconds between intervals
     */
    float Delta();

private:
    bool mStopped;

    float mDelta;
    float mTotalRunningTime;
    float mSecondsPerCount;

    int64_t mStartTime;
    int64_t mPreviousTime;
};
}  // namespace physika::core