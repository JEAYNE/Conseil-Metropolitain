@ECHO OFF
SET barDir=%~dp0
cd %batDir%

SET PRG=Conseil-Metropolitain.exe

SET   QT=D:\APPS\Qt\6.8.2\mingw_64
SET  GCC=D:\APPS\Qt\Tools\mingw1310_64
SET INST=D:\APPS\Qt\Tools\QtInstallerFramework\4.8

SET DST_BIN=packages\bin\data\bin
SET DST_LIB=packages\lib\data\bin
SET DST_DAT=packages\data\data\donnees
SET DST_DOC=packages\doc\data\documentation

echo.
cd
echo === Creating data directories ===
FOR %%S IN (%DST_BIN% %DST_LIB% %DST_LIB%\platforms %DST_LIB%\styles %DST_DAT% %DST_DOC%) DO (
    echo %%S
    IF not exist "%%S" ( mkdir "%%S" ) ELSE ( del /Q "%%S\*" )
)

echo.
echo === Preparing Qt6* libs ===
FOR %%S IN (Qt6Core Qt6Gui Qt6Widgets Qt6PrintSupport) DO (
  copy "%QT%\bin\%%~S.dll*"                "%DST_LIB%"
)

echo.
echo === Preparing Platforms libs ===
FOR %%S IN (qwindows) DO (
  copy "%QT%\plugins\platforms\%%~S.dll*"  "%DST_LIB%\platforms"
)

echo.
echo === Preparing style libs ===
FOR %%S IN (qmodernwindowsstyle) DO (
  copy "%QT%\plugins\styles\%%~S.dll*"     "%DST_LIB%\styles"
)

echo.
echo === Preparing GCC libs ===
FOR %%S IN (libgcc_s_seh-1 libstdc++-6 libwinpthread-1) DO (
  copy "%GCC%\bin\%%~S.dll*"               "%DST_LIB%"
)

echo.
echo === Preparing Data ===
robocopy "..\data"  "%DST_DAT%" /s

rem echo.
rem echo === Preparing documentation ===
rem copy ..\doc\*.fodt "%DST_DOC%

echo.
echo === Preparing programs %PRG%  ===
copy "..\build\Release\release\%PRG%"   "%DST_BIN%
echo.

tree /F packages

echo.
echo === Creating the installer for %PRG% ===
del *%PRG% 2> nul
echo ...
%INST%\bin\binarycreator.exe --verbose --offline-only --ignore-translations --config config.xml --packages packages setup_%PRG%
echo.
echo Utiliser "setup_%PRG% -d  -g ifw.*=true" pour executer l'installateur en mode debug
echo.
echo === Done ===
dir /B setup_%PRG%
pause
