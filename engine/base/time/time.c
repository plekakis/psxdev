#include "time.h"

uint16 g_timeHsyncToSecond;
float g_invTimeHsyncToSecond;

///////////////////////////////////////////////////
int16 Time_Initialize(uint8 i_tvMode)
{
	int32 success = 1;

	success &= SetRCnt(RCntCNT1, UINT16_MAX, RCntMdINTR);
	success &= StartRCnt(RCntCNT1);

	// A second is after 15625 HSYNCs for NTSC and after 15733 HSYNCs for PAL
	g_timeHsyncToSecond = (i_tvMode == MODE_NTSC) ? 15625 : 15733;
	g_invTimeHsyncToSecond = 1.0f / (float)g_timeHsyncToSecond;

	return success ? E_OK : E_FAILURE;
}

///////////////////////////////////////////////////
int16 Time_Reset()
{
	ResetRCnt(RCntCNT1);
}

///////////////////////////////////////////////////
TimeMoment Time_Now()
{
	return (uint16)GetRCnt(RCntCNT1);
}

///////////////////////////////////////////////////
float Time_ToSeconds(TimeMoment i_moment)
{
	return (float)i_moment * g_invTimeHsyncToSecond;
}

///////////////////////////////////////////////////
float Time_ToMilliseconds(TimeMoment i_moment)
{
	return Time_ToSeconds(i_moment) * 0.001f;
}

///////////////////////////////////////////////////
int16 Time_Shutdown()
{
	return E_OK;
}