// Copyright 2009 Mark Leone (markleone@gmail.com).  All rights reserved.
// Licensed under the terms of the MIT License.
// See http://www.opensource.org/licenses/mit-license.php.

#ifndef UT_TIMER_H
#define UT_TIMER_H

#if defined(__APPLE__)
#include <mach/mach.h>
#include <mach/mach_time.h>
#elif defined(__linux__)
#include <time.h>
#include <stdint.h>
#elif defined(_MSC_VER)
#include <windows.h>
#endif

/// High-resolution timer class
class UtTimer {
private:
    uint64_t mStart;
    uint64_t mElapsed;
    
public:
    /// Construct timer
    UtTimer() : mElapsed(0) { }

    /// Start timer
    void Start() { mStart = GetTicks(); }

    /// Stop timer
    void Stop() { mElapsed += GetTicks() - mStart; }

    /// Reset timer
    void Reset() { mElapsed = 0; }

    /// Get elapsed time in seconds.  Calls GetFreq, which is not cheap.
    double GetElapsed() const { return (double) mElapsed / GetFreq(); }

    /// Get the number of ticks from the performance counter.
    static uint64_t GetTicks() 
    {
        #if defined(__APPLE__ )
        return mach_absolute_time();
        #elif defined(__linux__)
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return (uint64_t) ts.tv_sec * 1e9 + (uint64_t) ts.tv_nsec;
        #elif defined(_MSC_VER)
        LARGE_INTEGER ticks;
        QueryPerformanceCounter(&ticks);
        return ticks.QuadPart;
        #else
        return 0;
        #endif
    }

    /// Get the performance counter frequency
    static double GetFreq()
    {
        #if defined(__APPLE__ )
        mach_timebase_info_data_t tb;
        mach_timebase_info(&tb);    
        return 1e9 * (double) tb.numer / (double) tb.denom;
        #elif defined(__linux__)
        return 1e9; // clock_gettime() returns nanoseconds
        #elif defined(_MSC_VER)
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        return (double) freq.QuadPart;
        #else
        return 0;
        #endif
    }
};

#endif // ndef UT_TIMER_H
