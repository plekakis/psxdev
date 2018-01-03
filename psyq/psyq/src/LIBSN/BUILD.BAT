del *.obj

asmpsx /l open,open
asmpsx /l close,close
asmpsx /l lseek,lseek
asmpsx /l creat,creat
asmpsx /l snread,snread
asmpsx /l snmain,snmain
asmpsx /l fsinit,fsinit
asmpsx /l snwrite,snwrite
ccpsx -c -O2 read.c -o read.obj
ccpsx -c -O2 write.c -o write.obj
asmpsx /l sndef,sndef
asmpsx /l purev,purev
asmpsx /l cache,cache

ccpsx -O2 -c -DL_ashldi3     libgcc2.c -o _ashldi3.obj
ccpsx -O2 -c -DL_ashrdi3     libgcc2.c -o _ashrdi3.obj
ccpsx -O2 -c -DL_cmpdi2      libgcc2.c -o _cmpdi2.obj
ccpsx -O2 -c -DL_divdi3      libgcc2.c -o _divdi3.obj
ccpsx -O2 -c -DL_eh          libgcc2.c -o _eh.obj
ccpsx -O2 -c -DL_ffsdi2      libgcc2.c -o _ffsdi2.obj
ccpsx -O2 -c -DL_fixdfdi     libgcc2.c -o _fxdfdi.obj
ccpsx -O2 -c -DL_fixsfdi     libgcc2.c -o _fxsfdi.obj
ccpsx -O2 -c -DL_fixtfdi     libgcc2.c -o _fxtfdi.obj
ccpsx -O2 -c -DL_fixunsdfdi  libgcc2.c -o _fxusdfdi.obj
ccpsx -O2 -c -DL_fixunsdfsi  libgcc2.c -o _fixusdfsi.obj
ccpsx -O2 -c -DL_fixunssfdi  libgcc2.c -o _fxussfdi.obj
ccpsx -O2 -c -DL_fixunssfsi  libgcc2.c -o _fxussfsi.obj
ccpsx -O2 -c -DL_fixunstfdi  libgcc2.c -o _fxustfdi.obj
ccpsx -O2 -c -DL_fixunsxfdi  libgcc2.c -o _fxusxfdi.obj
ccpsx -O2 -c -DL_fixunsxfsi  libgcc2.c -o _fxusxfsi.obj
ccpsx -O2 -c -DL_fixxfdi     libgcc2.c -o _fxxfdi.obj
ccpsx -O2 -c -DL_floatdidf   libgcc2.c -o _fltdidf.obj
ccpsx -O2 -c -DL_floatdisf   libgcc2.c -o _fltdisf.obj
ccpsx -O2 -c -DL_floatditf   libgcc2.c -o _fltditf.obj
ccpsx -O2 -c -DL_floatdixf   libgcc2.c -o _fltdixf.obj
ccpsx -O2 -c -DL_lshldi3     libgcc2.c -o _lshldi3.obj
ccpsx -O2 -c -DL_lshrdi3     libgcc2.c -o _lshrdi3.obj
ccpsx -O2 -c -DL_moddi3      libgcc2.c -o _moddi3.obj
ccpsx -O2 -c -DL_muldi3      libgcc2.c -o _muldi3.obj
ccpsx -O2 -c -DL_negdi2      libgcc2.c -o _negdi2.obj
ccpsx -O2 -c -DL_new_handler libgcc2.c -o _new_handler.obj
ccpsx -O2 -c -DL_op_delete   libgcc2.c -o _op_delete.obj
ccpsx -O2 -c -DL_op_new      libgcc2.c -o _op_new.obj
ccpsx -O2 -c -DL_op_vdel     libgcc2.c -o _op_vdel.obj
ccpsx -O2 -c -DL_op_vnew     libgcc2.c -o _op_vnew.obj
ccpsx -O2 -c -DL_shtab       libgcc2.c -o _shtab.obj
ccpsx -O2 -c -DL_trampoline  libgcc2.c -o _trampoline.obj
ccpsx -O2 -c -DL_ucmpdi2     libgcc2.c -o _ucmpdi2.obj
ccpsx -O2 -c -DL_udivdi3     libgcc2.c -o _udivdi3.obj
ccpsx -O2 -c -DL_udivmoddi4  libgcc2.c -o _udivmoddi4.obj
ccpsx -O2 -c -DL_udiv_w_sdiv libgcc2.c -o _udiv_w_sdiv.obj
ccpsx -O2 -c -DL_umoddi3     libgcc2.c -o _umoddi3.obj
ccpsx -O2 -c -DL_varargs     libgcc2.c -o _varargs.obj
ccpsx -O2 -c -DL__gcc_bcmp   libgcc2.c -o __gcc_bcmp.obj

del ..\libsn.lib
psylib /a ..\libsn *.obj
del *.obj

