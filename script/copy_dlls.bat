@echo off
if exist "%1" (
	copy /y %1 %2 > nul

	if %ERRORLEVEL%==0 ( echo DLL's copied )
	if %ERRORLEVEL%==1 ( echo Failed to copy DLL's )

	set errorflag=0
)