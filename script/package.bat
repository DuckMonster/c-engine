pushd ..
@echo off
echo -- BUILD START --
msbuild build.proj -nologo -property:Config=Release+ReleaseServer -verbosity:m
echo -- BUILD COMPLETE --
echo.
echo.
echo -- PACKAGING --
echo.
echo -- Copy --
mkdir pack-temp

rem Copy exe
xcopy bin\release\*.exe pack-temp\
rem Copy resources
xcopy /e res\* pack-temp\res\
rem Copy configuration
xcopy config.dat pack-temp\

echo.
echo -- Zip --
rem Get the filename for the zip
set year=%DATE:~12,2%
set month=%DATE:~7,2%
set day=%DATE:~4,2%
set hour=%TIME:~,2%
set minute=%TIME:~3,2%

set file_path=out\game_%year%%month%%day%_%hour%%minute%.zip

rem Make folder, and delete any currently existing files
if exist %file_path% del %file_path%
if not exist out mkdir out

rem Zip that bad boy!
pushd pack-temp
7z a -tzip ..\%file_path% *
popd
rmdir /s /q pack-temp
echo.
echo -- PACKAGE DONE --

popd
pause