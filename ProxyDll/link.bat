set TARGET="C:\Program Files\RecotteStudio"
set TARGET=%TARGET:"=%

set CURRENT_DIR=%~dp0
openfiles > nul
if errorlevel 1 (
    REM CURRENT_DIR‚ª%1‚É‚È‚é
    PowerShell.exe -Command Start-Process \"%~f0\" -ArgumentList %CURRENT_DIR% -Verb runas
    exit
)
cd %TARGET%
set PROJECT=%1
set PROJECT=%PROJECT:"=%

cmd /c del /q "d3d11.dll"
cmd /c mklink "d3d11.dll" "%PROJECT%..\x64\Debug\ProxyDll.dll"
pause