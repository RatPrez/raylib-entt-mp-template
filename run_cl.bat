@echo off
setlocal
set ROOT=%~dp0
if exist "%ROOT%build\client\client.exe" (
    "%ROOT%build\client\client.exe" %*
) else (
    "%ROOT%build\client\Debug\client.exe" %*
)
