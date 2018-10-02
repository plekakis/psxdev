$currentLoc = Get-Location;

$newPath = "$($currentLoc)\psyq\bin;$($currentLoc)\emu"
$oldPath = [Environment]::GetEnvironmentVariable('path', 'User');

[Environment]::SetEnvironmentVariable('path', "$($newPath);$($oldPath)",'User');

[Environment]::SetEnvironmentVariable('LIBRARY_PATH', "$($currentLoc)\psyq\lib",'User');
[Environment]::SetEnvironmentVariable('COMPILER_PATH', "$($currentLoc)\psyq\bin",'User');
[Environment]::SetEnvironmentVariable('C_PLUS_INCLUDE_PATH', "$($currentLoc)\psyq\include",'User');
[Environment]::SetEnvironmentVariable('C_INCLUDE_PATH', "$($currentLoc)\psyq\include",'User');
[Environment]::SetEnvironmentVariable('C_LIBRARY_PATH', "$($currentLoc)\psyq\lib",'User');
[Environment]::SetEnvironmentVariable('PSYQ_PATH', "$($currentLoc)\psyq\bin",'User');
[Environment]::SetEnvironmentVariable('PSX_PATH', "$($currentLoc)\psyq\bin",'User');
