pushd cdrom\root
mkpsxiso demo.xml -y
if %errorlevel% neq 0 pause & exit /b %errorlevel%
popd

catflap run cdrom/root/main.exe
pause