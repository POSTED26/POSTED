@echo off

mkdir ..\build
pushd ..\build

cl -Zi ..\code\win32_posted.cpp user32.lib Gdi32.lib

popd