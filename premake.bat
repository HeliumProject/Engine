@echo off

pushd %~dp0Dependencies\premake
if not exist bin\release\premake5.exe nmake -f Bootstrap.mak windows
popd

"%~dp0Dependencies\premake\bin\release\premake5.exe" %*
