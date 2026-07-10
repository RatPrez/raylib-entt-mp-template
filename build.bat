@echo off
setlocal
set ROOT=%~dp0

set EXTRA=
if defined GNS_ROOT set EXTRA=-DGNS_ROOT=%GNS_ROOT%

cmake -B "%ROOT%build" -S "%ROOT%" -DCMAKE_BUILD_TYPE=Debug %EXTRA%
if errorlevel 1 exit /b 1
cmake --build "%ROOT%build" --config Debug
if errorlevel 1 exit /b 1

copy /Y "%ROOT%build\compile_commands.json" "%ROOT%compile_commands.json" >nul 2>&1

echo.
echo Build complete:
echo   %ROOT%build\server\server.exe
echo   %ROOT%build\client\client.exe
