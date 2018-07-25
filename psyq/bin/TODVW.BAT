@echo off
rem
c:\psxgraph\bin\psreset
c:\psxgraph\bin\psbload -s %1 800c0000 > c:\psxgraph\bin\out
c:\psxgraph\bin\psbload -s %2 800e0000 >> c:\psxgraph\bin\out
c:\psxgraph\bin\psrun -s c:\psxgraph\bin\patchw.exe >> c:\psxgraph\bin\out
c:\psxgraph\bin\psrun -s c:\psxgraph\bin\todview5.exe  >> c:\psxgraph\bin\out


