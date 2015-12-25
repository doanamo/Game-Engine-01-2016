@echo off

rem Remove existing directory.
rd /s /q "CMake-VS2015" >nul 2>&1

rem Create an empty directory.
mkdir "CMake-VS2015"
cd "CMake-VS2015"

rem Generate solution files.
cmake -G "Visual Studio 14" ./..

rem Prevent console from closing.
pause
