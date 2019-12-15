#ifndef SYSTEM_H_INC
#define SYSTEM_H_INC

#include <engine.h>

typedef struct
{
    void (*AppStartFncPtr)();
    void (*AppUpdateFncPtr)();
	void (*AppPreRenderFncPtr)();
	void (*AppRenderFncPtr)();
    void (*AppShutdownFncPtr)();

	// These are considered to be PAL resolutions. Available ones are:
	// Low resolution:  256x256, 320x256, 368x256, 512x256, 640x256
	// High resolution: 256x512, 320x512, 368x512, 512x512, 640x512
	//
	// in NTSC systems, the equivelant NTSC resolution will be selected.
	uint32  m_displayWidth;
	uint32  m_displayHeight;
	uint32	m_sysStackSizeInBytes;
	uint32  m_gfxScratchSizeInBytes;
	uint32  m_coreScratchSizeInBytes;
	uint32  m_coreStackSizeInBytes;
	uint8	m_refreshMode;
    uint8   m_tvMode;
}SystemInitInfo;

// Initializes all subsystems (gfx, input, storage, etc)
int16 System_Initialize(SystemInitInfo* i_info);

// Main system loop
int16 System_MainLoop();

// Shutdown all subsystems
int16 System_Shutdown();

#endif // SYSTEM_H_INC
