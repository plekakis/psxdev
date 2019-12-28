#ifndef TIME_H_INC
#define TIME_H_INC

#include <engine.h>

#define TimeMoment uint32

// Initializes the time subsystem
int16 Time_Initialize(uint8 i_tvMode);

// Reset the internal time counter, should only be used internally.
int16 Time_Reset();

// Get the current time in HSyncs from frame start
TimeMoment Time_Now();

// Convert TimeMoment to seconds
fixed8_24 Time_ToSeconds(TimeMoment i_moment);

// Convert TimeMoment to milliseconds
fixed8_24 Time_ToMilliseconds(TimeMoment i_moment);

// Convert seconds to TimeMoment
TimeMoment Time_FromSeconds(fixed8_24 i_seconds);

// Convert milliseconds to TimeMoment
TimeMoment Time_FromMilliseconds(fixed8_24 i_milliseconds);

// Shutdown the time subsystem
int16 Time_Shutdown();

#endif // TIME_H_INC