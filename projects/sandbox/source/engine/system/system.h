#ifndef SYSTEM_H_INC
#define SYSTEM_H_INC

#include "../engine.h"

typedef struct
{
    void (*AppStartFncPtr)();
    void (*AppUpdateFncPtr)();
	void (*AppRenderFncPtr)();
    void (*AppShutdownFncPtr)();

    bool    m_isHighResolution;
    uint8   m_tvMode;
}SystemInitInfo;

// Initializes all subsystems (gfx, input, storage, etc)
int16 System_Initialize(SystemInitInfo* i_info);

// Main system loop
int16 System_MainLoop();

// Shutdown all subsystems
int16 System_Shutdown();

#endif // SYSTEM_H_INC
