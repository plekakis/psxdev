CC_CPE = ccpsx -g -Xo$80100000 -Xm
CC_EXE = ccpsx -g -Xo$80100000 -Xm c:\psx\lib\none3.obj
CC_APDS = ccpsx -g -Xo$80100000 -Xm -DAPD_SAVE c:\psx\lib\autopad.obj
CC_APDL = ccpsx -g -Xo$80100000 -Xm -DAPD_LOAD c:\psx\lib\autopad.obj
CPE2X  = cpe2x

all:	tuto1 tuto2 tuto3 tuto4 tuto5 tuto6
	

for_menu:	tuto1.exe tuto2.exe tuto3.exe tuto4.exe tuto5.exe tuto6.exe
	

tuto1:
	$(CC_CPE) tuto1.c -otuto1.cpe,tuto1.sym

tuto2:
	$(CC_CPE) tuto2.c -otuto2.cpe,tuto2.sym

tuto3:
	$(CC_CPE) tuto3.c -otuto3.cpe,tuto3.sym

tuto4:
	$(CC_CPE) tuto4.c -otuto4.cpe,tuto4.sym

tuto5:
	$(CC_CPE) tuto5.c -otuto5.cpe,tuto5.sym

tuto6:
	$(CC_CPE) tuto6.c -otuto6.cpe,tuto6.sym

# ----------------------------------------------------------------
tuto1.exe:
	$(CC_EXE) tuto1.c -otuto1.cpe,tuto1.sym
	$(CPE2X) tuto1.cpe

tuto2.exe:
	$(CC_EXE) tuto2.c -otuto2.cpe,tuto2.sym
	$(CPE2X) tuto2.cpe

tuto3.exe:
	$(CC_EXE) tuto3.c -otuto3.cpe,tuto3.sym
	$(CPE2X) tuto3.cpe

tuto4.exe:
	$(CC_EXE) tuto4.c -otuto4.cpe,tuto4.sym
	$(CPE2X) tuto4.cpe

tuto5.exe:
	$(CC_EXE) tuto5.c -otuto5.cpe,tuto5.sym
	$(CPE2X) tuto5.cpe

tuto6.exe:
	$(CC_EXE) tuto6.c -otuto6.cpe,tuto6.sym
	$(CPE2X) tuto6.cpe

clean:
	del *.exe
	del *.cpe
	del *.sym
	del *.map
	del *.?~
	del *.??~
