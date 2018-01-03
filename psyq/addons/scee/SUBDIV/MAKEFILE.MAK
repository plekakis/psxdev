main.cpe:	main.obj subdiv.obj
		-2 ccpsx /g @main.lnk
cflags = -c -O4 -mgpopt -G256 -comments-c++
aflags = /l

deps =		main.h

main.obj:	main.c $(deps)
		ccpsx	$(cflags) main.c

subdiv.obj:	subdiv.mip $(deps)
		asmpsx $(aflags) subdiv.mip,subdiv.obj

debug:
		resetps
		-2 run c:\psx\bin\snpatch.cpe
		-3 dbugpsx /emain main
