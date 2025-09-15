@echo off
SETLOCAL ENABLEEXTENSIONS

rem attion variable scope is lost when nesting if's
rem helpfull for string http://www.dostips.com/DtTipsStringManipulation.php

rem Get the source file path from the first command line argument
set "arg1=%~1"
set "dir=%~1"
set "found=0"
set "search=%~1"
set "scriptdir=%~dp0"

rem Check if argument is provided
if "%arg1%"=="" (
    echo No source file in command line.
    set "arg1=local"
)

echo Command line parameter: "%arg1%"

rem Check for different file suffixes
IF EXIST "%arg1%-latest.osm.pbf" (
    set "arg1=%arg1%-latest.osm.pbf"
    set "found=1"
    goto checkdone
)

IF EXIST "%arg1%.osm.pbf" (
    set "arg1=%arg1%.osm.pbf"
    set "found=1"
    goto checkdone
)

IF EXIST "%arg1%.pbf" (
    set "arg1=%arg1%.pbf"
    set "found=1"
    goto checkdone
)

IF EXIST "%arg1%.osm" (
    set "arg1=%arg1%.osm"
    set "found=1"
    goto checkdone
)

echo Trying substring from "%arg1%"

rem Check if full file path exists
IF EXIST "%arg1%" (
    echo File specified completely in command line.
    goto test1
)

:checkdone
if "%found%"=="0" (
    echo Import file not found.
    goto ExitNotFound
) else (
    echo Starting import with file "%arg1%" into "%dir%"
)

rem Create destination directory if it does not exist
IF NOT EXIST "%dir%" (
    mkdir "%dir%"
)

rem -------------------------------
rem Display the command for debugging
echo Import command:
echo "%scriptdir%bin\Import.exe" --typefile "%scriptdir%stylesheets\route.ost" -e 17 --destinationDirectory "%dir%" "%arg1%"
rem -------------------------------

rem Execute import
call "%scriptdir%bin\Import.exe" --typefile "%scriptdir%stylesheets\route.ost" -e 17 --destinationDirectory "%dir%" "%arg1%"
call "%scriptdir%bin\Import.exe" --typefile "%scriptdir%stylesheets\route.ost" -s 22 --delete-temporary-files true --delete-debugging-files true --delete-analysis-files true --delete-report-files true --destinationDirectory "%dir%" "%arg1%"

goto NoErrorExit

:test1
rem Extract last 15 characters of filename
set "search=%search:~-15%"
echo Last 15 chars: "%search%"
if "%search%"=="-latest.osm.pbf" (
    set "found=1"
    set "dir=%dir:-latest.osm.pbf=%"
    goto checkdone
)

rem Extract last 8 characters of filename
set "search=%search:~-8%"
echo Last 8 chars: "%search%"
if "%search%"==".osm.pbf" (
    set "found=1"
    set "dir=%dir:.osm.pbf=%"
    goto checkdone
)

rem Extract last 4 characters of filename
set "search=%search:~-4%"
echo Last 4 chars: "%search%"
if "%search%"==".pbf" (
    set "found=1"
    set "dir=%dir:.pbf=%"
    goto checkdone
)

if "%search%"==".osm" (
    set "found=1"
    set "dir=%dir:.osm=%"
    goto checkdone
)

goto checkdone

:ExitNotFound
echo Import source not found.

:NoErrorExit
pause
