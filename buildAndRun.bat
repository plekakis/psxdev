pushd source
call psymake
if %errorlevel% neq 0 pause & exit /b %errorlevel%
popd

move source\main.exe cdrom\root


pushd cdrom\root
mkpsxiso demo.xml -y
if %errorlevel% neq 0 pause & exit /b %errorlevel%
popd

ePSXe -slowboot -loadbin "cdrom\demo.bin" -nogui
psuse