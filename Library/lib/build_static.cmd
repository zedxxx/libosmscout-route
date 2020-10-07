rem @echo off

set root=%~dp0
set build=.build
set src=.libosmscout

cd .\..\..\

if exist %src%\ (
    cd %src%\    
    git fetch --all --verbose
    git clean -d -x --force
    git reset --hard origin/master    
    cd .\..\
) else (
    git clone https://github.com/Framstag/libosmscout %src%
)

if exist %build%\ (
    rmdir /q /s %build%
)

mkdir %build%
cd %build%

cmake .\..\%src% -G "MinGW Makefiles" ^
    "-DBUILD_SHARED_LIBS=OFF" ^
    "-DCMAKE_BUILD_TYPE=Release"
    
cd libosmscout\

mingw32-make

copy /b /y libosmscout.a %root%libosmscout.a
xcopy /i /s /h /e /y .\include %root%..\include

cd ./../../%src%
xcopy /i /s /h /e /y .\libosmscout\include %root%..\include

cd %root%..\..\

if exist %build%\ (
    rmdir /q /s %build%
)

pause