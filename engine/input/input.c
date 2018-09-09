#include "input.h"

// This will always be 2 (maximum of 2 controllers), as we obviously won't support mutli-tap or anything else than psx controllers
#define NUM_CONTROLLERS ( 2u )

// 2 Controllers
uint8 g_padBuffer[NUM_CONTROLLERS][34];

// Current input mask
uint32 g_padMask[NUM_CONTROLLERS];

uint32 g_pad[NUM_CONTROLLERS];

// Controller connection mask (eg. controllers that state is Stable)
uint32 g_controllerConnectionMask;

// Controller ids (eg. which type of controller this index is. Standard, DualShock, mouse, etc)
int32 ids[NUM_CONTROLLERS];

unsigned char align[6]={0,1,0xFF,0xFF,0xFF,0xFF};
struct {
	unsigned char Button;
	unsigned char Lock;
	unsigned char Motor0,Motor1;
	unsigned char Send;
} hist;

/* Connected Controller Type */
char* padstr[] = {
	"NO CONTROLLER", "MOUSE", "NEGI-CON", "KONAMI-GUN", "STANDARD CONTROLLER",
	"ANALOG STICK",	"NAMCO-GUN", "ANALOG CONTROLLER"
};

///////////////////////////////////////////////////
int16 Input_Initialize()
{
	PadInitDirect(g_padBuffer[0], g_padBuffer[1]);
	PadStartCom();

	g_controllerConnectionMask = 0u;

	memset(&hist, 0, sizeof(hist));
	
	return E_OK;
}

///////////////////////////////////////////////////
int16 Input_Shutdown()
{
	PadStop();
	PadStopCom();
	return E_OK;
}

///////////////////////////////////////////////////
int16 Input_Update()
{
	const uint32 ports[2] = { 0x0, 0x10 };
	uint32 index = 0u;

	// Reset connection mask
	g_controllerConnectionMask = 0u;

	for (index=0; index<NUM_CONTROLLERS; ++index)
	{
		const uint32 port = ports[index];
		const uint8* padi = g_padBuffer[index];
		const uint32 padd = ~((padi[2]<<8) | (padi[3]));
		const int32 state = PadGetState(port);
    
		g_pad[index] = padd;
		
		// TODO: Revisit for correct DualShock support
		if (state != PadStateDiscon)
		{
			ids[index] = PadInfoMode(port, InfoModeCurID, 0);
			
			switch(ids[index])
			{
			case 7:
				if (PadInfoMode(port,InfoModeCurExID,0))
				{
					
				}
				break;
			default:
				hist.Motor0 = 0;
				hist.Motor1 = 0;
				break;
			}
		}
		
		if (state == PadStateFindPad) {
			hist.Send = 0;
		}
	
		if ( hist.Send==0 )
		{
			PadSetAct(port,&hist.Motor0,2);
			if (state == PadStateFindCTP1) {
				hist.Send = 1;
			}
			if (state == PadStateStable) {
				if (PadSetActAlign(port,align)) {
					hist.Send = 1;
				}
			}
		}
		
		// At this point, we may have a connected controller
		// Record it
		// - CTP1 is standard controllers
		// - State stable is for DualShock
		// ... why is it different?
		//
		if ((state == PadStateFindCTP1) || (state == PadStateStable))
		{
			g_controllerConnectionMask |= (1 << index);
		}
	}

	return E_OK;
}

///////////////////////////////////////////////////
uint32 Input_GetConnectionMask()
{
	return g_controllerConnectionMask;
}

///////////////////////////////////////////////////
char* Input_GetControllerId(uint32 i_index)
{
	return padstr[ids[i_index]];
}	

///////////////////////////////////////////////////
uint32 Input_GetPad(uint32 i_index)
{
	return g_pad[i_index];
}