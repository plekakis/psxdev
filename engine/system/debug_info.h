#ifndef DEBUG_INFO_H_INC
#define DEBUG_INFO_H_INC

#if !CONFIG_FINAL

#include "../time/time.h"

typedef struct
{
	TimeMoment	m_cpuStartTime;
	TimeMoment	m_cpuEndTime;
	TimeMoment	m_cpuEndTimeVSync;
	uint16		m_framesPerSecond;
	uint16		m_framesPerSecondVSync;
}DebugPanelTimings;

typedef struct
{
	DebugPanelTimings	m_timings;
	GfxPrimCounts		m_gfxPrimCounts;
}DebugPanelInfo;

void Debug_DrawAll(DebugPanelInfo* i_info);
#endif // !CONFIG_FINAL

#endif // DEBUG_INFO_H_INC
