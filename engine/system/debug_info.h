#ifndef DEBUG_INFO_H_INC
#define DEBUG_INFO_H_INC

#if !CONFIG_FINAL
typedef struct
{
	uint16 m_cpuStartTime;
	uint16 m_cpuEndTime;
	uint16 m_cpuEndTimeVSync;
	uint16 m_gpuStartTime;
	uint16 m_gpuEndTime;
	uint16 m_framesPerSecond;
}DebugPanelTimings;

typedef struct
{
	DebugPanelTimings	m_timings;
	GfxPrimCounts		m_gfxPrimCounts;
}DebugPanelInfo;

void Debug_DrawAll(DebugPanelInfo* i_info);
#endif // !CONFIG_FINAL

#endif // DEBUG_INFO_H_INC
