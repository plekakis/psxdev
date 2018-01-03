/*
 * $PSLibId: Run-time Library Release 4.4$
 */
/*
 *          Sample Program: H2000 Boot Module
 *
 *      Copyright (C) 1995 by Sony Computer Entertainment Inc.
 *          All rights Reserved
 *
 *   Version    Date
 *  ------------------------------------------
 *  1.10        May,19,1995 yoshi
 *  1.11        Aug,15,1997 yoshi moved the description to readme file.
 */

main()
{
	_96_remove();
	_96_init();
	LoadExec("cdrom:\\PSX.EXE;1", 0x801fff00, 0);
		/* File name, stack pointer, stack size */
}




