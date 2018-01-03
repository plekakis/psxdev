/*
 * $PSLibId: Runtime Library Release 3.6$
 */
/*************************************************************************

  DTL-H2000 の CD-ROM/CD-Emulator より PSX.EXE を実行させるモジュール
 （patchxした状態でブートさせる方法）
 
	1995/05/19	v1.1	yoshi

==========================================================================

  現在、PCからH2000にプログラム(.CPEファイル)をロードして実行させる場合、
  前もって patchx.cpe を実行する事が必要です。

	（例）
	DOS> run patchx  
	DOS> run main
  
  これに対して、CD-ROM/CD-Emulator からブートプログラム(PSX.EXE)を読みだして
  実行させるには、patchx.cpe の後にこの cdexec.cpe を実行して下さい。

	DOS> run patchx
	DOS> run cdexec

  （注意）
  「resetps 0」は使用しないで下さい。
  「resetps 0」では、patchx されない状態でブートする為、正常に動作しない
  場合があります。

*************************************************************************

   A module for executing PSX.EXE on a CD-ROM/CD-Emulator with DTL-H2000 
   (A way to boot with patchx executed.)

	May 19, 1995	ver. 1.1	yoshi

==========================================================================

  At present, in order to load a program (.cpe file) from PC to carry 
  out, patchx.cpe must be carried out in advance.

	Example:
	DOS> run patchx  
	DOS> run main
  
  On the other hand, in order to read a boot program (PSX.EXE) 
  on the CD-ROM/CD-Emulator to carry out, cdexec.cpe must be carried 
  out after patchx.cpe execution.

	DOS> run patchx
	DOS> run cdexec

  Caution:
   Never use 'resetps 0'.
   PSX.EXE may not work properly with 'resetps 0' because it is booted
   without patchx execution.

**********************************************************************/

main()
{
	_96_remove();
	_96_init();
	LoadExec("cdrom:\\PSX.EXE;1", 0x801fff00, 0);
		/* File name, stack pointer, stack size ::
	   	ファイル名  スタックポインタ  スタックサイズ */
}




