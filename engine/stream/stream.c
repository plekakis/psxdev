#include "stream.h"

///////////////////////////////////////////////////
int16 Stream_Initialize()
{
	int32 err = CdInit();
	VERIFY_ASSERT(err == 1, "Stream_Initialize: CdInit failure");

#if ASSERT_ENABLED
	CdSetDebug(2);
#endif // ASSERT_ENABLED

	return E_OK;
}

///////////////////////////////////////////////////
int16 Stream_Shutdown()
{
	return E_OK;
}

///////////////////////////////////////////////////
int16 Stream_Update()
{
	return E_OK;
}