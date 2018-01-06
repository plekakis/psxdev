#ifndef SYSTEM_H_INC
#define SYSTEM_H_INC

#include "../engine.h"

typedef struct
{
    void (*AppStartFncPtr)();
    void (*AppUpdateFncPtr)();
    void (*AppShutdownFncPtr)();

    uint8   m_isHighResolution  : 1;
	uint8	m_isInterlaced		: 1;
    uint8   m_tvMode            : 1;
}SystemInitInfo;

// Initializes all subsystems (gfx, input, storage, etc)
int16 System_Initialize(SystemInitInfo* i_info);

// Main system loop
int16 System_MainLoop();

// Shutdown all subsystems
int16 System_Shutdown();

#endif // SYSTEM_H_INC
