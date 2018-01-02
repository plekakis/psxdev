echo Starting compilation...

del mem.map
del main.sym
del main.exe
del main.cpe
cls

ccpsx -O3 -Xo$80010000 ../source/engine/engine_scu.c ../source/game/game_scu.c -omain.cpe,main.sym,mem.map
if %errorlevel% neq 0 pause & exit /b %errorlevel%

cpe2x /ce main.cpe

if %errorlevel% neq 0 pause & exit /b %errorlevel%

echo Compile success! Copying to cd-rom ...

move main.exe ..\cdrom\root

pushd ..\cdrom\root
mkpsxiso demo.xml -y
if %errorlevel% neq 0 pause & exit /b %errorlevel%
popd

echo Finished!