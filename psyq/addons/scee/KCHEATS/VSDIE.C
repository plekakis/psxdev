/* ************************************************************************

 (c)1995 Visual Sciences Ltd

 Exception handler for PSX.

 Ver		Date		Author			Desc
----------------------------------------------------------------------------
 0.1		09/08/95	Brian Marshall	Initial Version. 				*/

#include "vsdie.h"

#include <r3000.h>
#include <asm.h>
#include <kernel.h>
#include <stddef.h>
#include <libsn.h>
#include <libgte.h>
#include <libgpu.h>

typedef struct tagEXCEPTIONDUMP
{
	DWORD rzero;
	DWORD rat;
	DWORD rv0;
	DWORD rv1;
	DWORD ra0;
	DWORD ra1;
	DWORD ra2;
	DWORD ra3;
	DWORD rt0;
	DWORD rt1;
	DWORD rt2;
	DWORD rt3;
	DWORD rt4;
	DWORD rt5;
	DWORD rt6;
	DWORD rt7;
	DWORD rs0;
	DWORD rs1;
	DWORD rs2;
	DWORD rs3;
	DWORD rs4;
	DWORD rs5;
	DWORD rs6;
	DWORD rs7;
	DWORD rt8;
	DWORD rt9;
	DWORD rk0;
	DWORD rk1;
	DWORD rgp;
	DWORD rsp;
	DWORD rfp;
	DWORD rra;
	DWORD rhi;
	DWORD rlo;
	DWORD rsr;
	DWORD rca;
	DWORD repc;
}EXCEPTIONDUMP;

extern EXCEPTIONDUMP Registers;

static DWORD g_ExEvent;
static DWORD g_ExEvent2;
static DWORD g_ExEvent3;
static DWORD g_ExEvent4;

/*	Exception Types...	*/

static char*	g_aExceptionTypes[] = 
	{
		"External Interrupt.",
		"TLB Modification Exception.",
		"TLB Miss (Load or Fetch).",
		"TLB Miss (Store).",
		"Address Error Exception (Load or Fetch).",
		"Address Error Exception (Store).",
		"Bus Error (Fetch).",
		"Bus Error (Load or Store).",
		"Syscall.",
		"BreakPoint.",
		"Reserved Instruction.",
		"Coprocessor Unusable.",
		"Arithmetic Overflow.",
		"Unknown Exception.",
		"Unknown Exception.",
		"Unknown Exception."
	};

static DISPENV g_ExDispEnv;
static DRAWENV g_ExDrawEnv;

static char g_aExTemp[50];

static void ExceptionHandler(void);

/*	Function to hook the Vector. Call as early in your code as possible.. */

void
_EX_Init(void)
{
	InstallExceptionHandler();
}	/* End _EX_Init	*/

/*	Exception Quit (Currently does nothing...) */

void
_EX_Quit(void)
{
}	/* End _EX_Quit	*/

/*	The Exception Handler Function.
	Called by the Assembler when the Exception Happens.
	The register state at the time of the exception in in the global
	structure Registers.	*/

void
CExceptionHandler(void)
{
	DWORD Cause;
	DWORD Pc;
	DWORD Type;
	DWORD *pStack;
	DWORD i;
	static RECT bg = {0, 0, 640, 256};

	ResetGraph(0);
	SetGraphDebug(0);
	FntLoad(640, 0);
	FntOpen(0, 0, 640, 240, 0, 1024);
	SetDefDrawEnv(&g_ExDrawEnv, 0, 0, 640, 240);
	SetDefDispEnv(&g_ExDispEnv, 0, 0, 640, 240);
	ClearImage(&bg, 128, 0, 0);
	PutDrawEnv(&g_ExDrawEnv);
	PutDispEnv(&g_ExDispEnv);

	Cause = Registers.rca;
	Pc = Registers.repc;
	Type = Cause;
	Type &= 0x1f;

	/*	Check the Type....	*/

	FntPrint("\n\n\n  Visual Sciences Exception Handler!\n");
	FntPrint("        (You know you've messed up...)\n\n");
	sprintf(g_aExTemp, "    Type : %s\n\n", g_aExceptionTypes[Type]);
	FntPrint(g_aExTemp);
	sprintf(g_aExTemp, "    At   : %08x", Pc);
	FntPrint(g_aExTemp);
	if((Cause & 0x80000000) == 0x80000000)
	{
		FntPrint("      in Branch Delay Slot.\n\n");
	}
	else
	{
		FntPrint("\n\n");
	}

	FntPrint("  Registers:\n\n");
	sprintf(g_aExTemp, "    ZERO %08x AT   %08x V0   %08x V1   %08x\n", Registers.rzero, Registers.rat, Registers.rv0, Registers.rv1);
	FntPrint(g_aExTemp);
	sprintf(g_aExTemp, "    A0   %08x A1   %08x A2   %08x A3   %08x\n", Registers.ra0, Registers.ra1, Registers.ra2, Registers.ra3);
	FntPrint(g_aExTemp);
	sprintf(g_aExTemp, "    T0   %08x T1   %08x T2   %08x T3   %08x\n", Registers.rt0, Registers.rt1, Registers.rt2, Registers.rt3);
	FntPrint(g_aExTemp);
	sprintf(g_aExTemp, "    T4   %08x T5   %08x T6   %08x T7   %08x\n", Registers.rt4, Registers.rt5, Registers.rt6, Registers.rt7);
	FntPrint(g_aExTemp);
	sprintf(g_aExTemp, "    S0   %08x S1   %08x S2   %08x S3   %08x\n", Registers.rs0, Registers.rs1, Registers.rs2, Registers.rs3);
	FntPrint(g_aExTemp);
	sprintf(g_aExTemp, "    S4   %08x S5   %08x S6   %08x S7   %08x\n", Registers.rs4, Registers.rs5, Registers.rs6, Registers.rs7);
	FntPrint(g_aExTemp);
	sprintf(g_aExTemp, "    T8   %08x T9   %08x K0   %08x K1   %08x\n", Registers.rt8, Registers.rt9, Registers.rk0, Registers.rk1);
	FntPrint(g_aExTemp);
	sprintf(g_aExTemp, "    GP   %08x SP   %08x FP   %08x RA   %08x\n", Registers.rgp, Registers.rsp, Registers.rfp, Registers.rra);
	FntPrint(g_aExTemp);
	sprintf(g_aExTemp, "    HI   %08x LO   %08x SR   %08x CA   %08x\n", Registers.rhi, Registers.rlo, Registers.rsr, Registers.rca);
	FntPrint(g_aExTemp);

	ClearImage(&bg, 128, 0, 0);
	FntFlush(-1);			   
	DrawSync(0);	
	VSync(0);
	SetDispMask(1);			   

die:
	goto die;
}	/* End CExcpetionHandler	*/
