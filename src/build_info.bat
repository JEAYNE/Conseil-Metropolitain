@ECHO OFF

rem QtCreator: Projects > Build > Build Steps > "Add build Step" > Custom Project Step
rem   |Command: %{sourceDir}/build_info.bat
rem   |Arguments avec Qt 6.8:  "%{ActiveProject:Name}" "Qt %{Qt:Version} ~ %{Compiler:Name} ~ %{ActiveProject:BuildConfig:Type}"
rem   |         avant Qt 6.8: "%{CurrentProject:Name}" "Qt %{Qt:Version} ~ %{Compiler:Name} ~ %{CurrentBuild:Type}"
rem   |Working dir: %{sourceDir}
rem Puis le faire monter en premiere position (avant qmake)
rem A faire pour chaque profils: "release", "profile", "debug"

rem ----------------- memo --------------------------------------------
rem %~dp0 - path of this BAT file
rem %~d2  - disk name of the second argument on command line
rem %~p2  - path of the second argument
rem %~n2  - file name of the second argument
rem %~x2  - file extension from the second argument
rem %NAME:~8,4% - starting at position 8 extract 4 characters (0 based)
rem ----------------- memo --------------------------------------------
rem 'echo' preserve all heading and trailing spaces
rem There is no simple method to trim these spaces

set BATDIR=%~dp0
cd %BATDIR%

set NOWDATE=%DATE%
set NOWTIME=%TIME%
rem date: 01/34/6789 time: 01:34:67.90

rem ////////////// build info /////////////////

set FILE=build_info.h
echo %FILE% created at %NOWDATE% %NOWTIME%
echo // begin: File %FILE% auto generated > %FILE%

echo #define COMPILE_YYYY  %NOWDATE:~6,4% >> %FILE%

CALL :setval COMPILE_YY    %NOWDATE:~8,2%
CALL :setval COMPILE_MONTH %NOWDATE:~3,2%
CALL :setval COMPILE_DAY   %NOWDATE:~0,2%

rem do not generate hh, mm, ss because this make git crazy ;-) 
rem CALL :setval COMPILE_hh    %NOWTIME:~0,2%
rem CALL :setval COMPILE_mm    %NOWTIME:~3,2%
rem CALL :setval COMPILE_ss    %NOWTIME:~6,2%

echo #define PROJECT_NAME  %1 >> %FILE%
echo #define COMPILE_ENV   %2 >> %FILE%

FOR /F %%G IN ('git rev-parse --abbrev-ref HEAD') DO (
  echo #define GIT_BRANCH %%G >> %FILE%
  set branch=%%G
)

echo #if __has_include("build_info_head.h") >> %FILE%
echo #  include "build_info_head.h"         >> %FILE%
echo #else                                  >> %FILE%
echo #  define GIT_HEAD   undefined         >> %FILE%
echo #endif                                 >> %FILE%

echo // end. >> %FILE%

set status=clean
FOR /F %%G IN ('git status --porcelain') DO (
  IF NOT [%%G] == [] set status=Dirty
)
rem 'status --porcelain' doesn't detect if a push is required
rem we do that by listing the commits 'ahead'
FOR /F %%G IN ('git log origin/%branch%..%branch%') DO (
  IF NOT [%%G] == [] set status=dirty
)

rem ////////////// git head /////////////////
rem This file must be excluded by .gitignore
set FILE=build_info_head.h
echo %FILE% created at %NOWDATE% %NOWTIME%
echo // begin: File %FILE% auto generated > %FILE%
FOR /F %%G IN ('git rev-parse HEAD') DO (
  echo #define GIT_HEAD %%G >> %FILE%
  set sha=%%G
)
echo #define GIT_STATUS %status% >> %FILE%
echo // end. >> %FILE%

echo Git: %branch%@%sha% is %status%
EXIT /B

:setval
set name=%1
set val2=%2
set first=%val2:~0,1%
IF %first% EQU 0 (
  set val=%val2:~1,1%
) ELSE (
  set val=%val2%
)
echo #define %name%2  %val2% >> %FILE%
echo #define %name%   %val%  >> %FILE%
EXIT /B
