@echo off

rem Remove temporary directories.
for /d %%x in ("CMake-*") do rd /s /q "%%x"

rem Remove temporary files.
for %%x in ("*.tmp") do rm /s /q "%%x"
