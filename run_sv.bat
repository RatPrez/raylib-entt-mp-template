@echo off
setlocal
set ROOT=%~dp0
if exist "%ROOT%build\server\server.exe" (
    "%ROOT%build\server\server.exe" %*
) else (
    "%ROOT%build\server\Debug\server.exe" %*
)
