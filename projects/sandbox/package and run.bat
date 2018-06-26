pushd cdrom\root
mkpsxiso demo.xml -y
if %errorlevel% neq 0 pause & exit /b %errorlevel%
popd

no$psx.exe cdrom/demo.cue
pause