REM ================= PSYQ.INI GENERATION =============

(
echo [ccpsx]
echo stdlib=libgs.lib libgte.lib libgpu.lib libspu.lib libsnd.lib libetc.lib libapi.lib libsn.lib libc.lib libcd.lib libcard.lib libmath.lib
echo compiler_path=%PSX_PATH%
echo assembler_path=%PSX_PATH%
echo linker_path=%PSX_PATH%
echo library_path=%LIBRARY_PATH%
echo c_include_path=%C_INCLUDE_PATH%
echo cplus_include_path=%C_PLUS_INCLUDE_PATH%
) > %PSX_PATH%\PSYQ.INI