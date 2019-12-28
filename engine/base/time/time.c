#include "time.h"

uint16 g_timeHsyncToSecond;
fixed8_24 g_invTimeHsyncToSecond;

///////////////////////////////////////////////////
int16 Time_Initialize(uint8 i_tvMode)
{
	int32 success = 1;

	success &= SetRCnt(RCntCNT1, UINT16_MAX, RCntMdINTR);
	success &= StartRCnt(RCntCNT1);

	// A second is after 15625 HSYNCs for NTSC and after 15733 HSYNCs for PAL
	g_timeHsyncToSecond = (i_tvMode == MODE_NTSC) ? 15625 : 15733;
	g_invTimeHsyncToSecond = F32toFP8_24( 1.0f / (float)g_timeHsyncToSecond );

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
TimeMoment Time_FromSeconds(fixed8_24 i_seconds)
{
	return MulFP8_24(i_seconds, g_timeHsyncToSecond);
}

///////////////////////////////////////////////////
TimeMoment Time_FromMilliseconds(fixed8_24 i_milliseconds)
{
	return Time_FromSeconds(i_milliseconds / 1000);
}

///////////////////////////////////////////////////
fixed8_24 Time_ToSeconds(TimeMoment i_moment)
{
	return i_moment * g_invTimeHsyncToSecond;
}

///////////////////////////////////////////////////
fixed8_24 Time_ToMilliseconds(TimeMoment i_moment)
{
	return Time_ToSeconds(i_moment) * 1000;
}

///////////////////////////////////////////////////
int16 Time_Shutdown()
{
	return E_OK;
}