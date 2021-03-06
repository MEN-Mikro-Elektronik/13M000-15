@echo off
REM ##########################################################################
REM
REM       file: install.bat
REM       author: thomas.schnuerer@men.de
REM
REM       installer batch for creating a MDIS for QNX system package
REM       containing all lowlevel drivers.
REM       uses only basic DOS batch commands.
REM
REM ##########################################################################

set G_defaultDir=c:\13M000-15
set G_logfile=setup.log
set G_silentMode=""


if "%1"=="" (
set G_MdisInstallPath=%G_defaultDir%
) else (
set G_MdisInstallPath=%1
)

if "%2"=="" (
echo running in interactive mode
) else (
echo running in auto yes mode
set G_silentMode=1
)

if "%G_silentMode%"=="1" goto :NEXT0

cls
echo.
echo  _________________________________________________________________________________________
echo ^|                                                                                         ^|
echo ^| Welcome to the MDIS for QNX System Package installer from MEN Mikro Elektronik GmbH.    ^|
echo ^| This installer will guide you through the installation process and perform required     ^|
echo ^| steps.                                                                                  ^|
echo ^| Usage: install.bat ^<installation path^> [silent]                                         ^|
echo ^|        if 2nd argument is passed batch runs without asking, assuming yes to questions   ^|
echo ^|_________________________________________________________________________________________^|
echo.
echo.
:NEXT0

echo.
echo using install path %G_MdisInstallPath%

if "%G_silentMode%"=="1" goto :NEXT1

set answer=""
set /p answer=Continue installation ([y/n], ENTER=n)?

if "%answer%"=="y" (
  echo ok, installing to %G_MdisInstallPath%
) else (
  goto :END
)
:NEXT1

if "%G_silentMode%"=="1" (
	mkdir %G_MdisInstallPath%
	goto :NEXT2
)

REM ------------------------------------------------------------
REM -- if path doesn't exist, create it. If it exists and is
REM -- nonempty, complain and exit
REM ------------------------------------------------------------
set answer=""
if not exist %G_MdisInstallPath% set /p answer=directory %G_MdisInstallPath% doesnt exist. Create ? [y/n]
if "%answer%"=="y" (
  echo Creating %G_MdisInstallPath% ...
  mkdir %G_MdisInstallPath%
)
:NEXT2

REM TODO check if folder is empty (seems to be an impossible nightmare under plain DOS)...
REM however in 99% of cases the folder was created fresh.


REM ------------------------------------------------------------
REM -- write logfile with all relevant informations for support
REM ------------------------------------------------------------
echo. >> %G_MdisInstallPath%\%G_logfile%
echo --------------------------------------------------------------- >> %G_MdisInstallPath%\%G_logfile%
echo MEN Mikro Elektronik GmbH %G_logfile% for MDIS for QNX System Package >> %G_MdisInstallPath%\%G_logfile%
echo Logfile created: %DATE% %TIME% >> %G_MdisInstallPath%\%G_logfile%
echo. >> %G_MdisInstallPath%\%G_logfile%
echo installed versions: >> %G_MdisInstallPath%\%G_logfile%
REM git describe --dirty --long --tags --always >> %G_MdisInstallPath%\%G_logfile%


REM ------------------------------------------------------------
REM -- copy together our regular MDIS package directory
REM ------------------------------------------------------------

echo Copying MDIS folders to %G_MdisInstallPath% ...
xcopy /S /E /I src %G_MdisInstallPath%\src > NUL
xcopy /S /E /I qnx6 %G_MdisInstallPath%\qnx6 > NUL
echo Copying folder LINUX to %G_MdisInstallPath% ...
xcopy /S /E /I LINUX %G_MdisInstallPath%\LINUX > NUL
echo Copying folder NT to %G_MdisInstallPath% ...
xcopy /S /E /I NT %G_MdisInstallPath%\NT > NUL

REM ------------------------------------------------------------
REM -- copy LL Drivers content, therefore ('dir /B 13*-06')
REM ------------------------------------------------------------
for /F %%i in ('dir /B 13*-06') do (
   cd %%i
   echo Copying Low Level Driver %%i to %G_MdisInstallPath%\src\DRIVERS\MDIS_LL
   xcopy /S /E /I DRIVERS\MDIS_LL\* %G_MdisInstallPath%\src\DRIVERS\MDIS_LL\ > NUL
   xcopy /S /E  INCLUDE\COM\MEN\* %G_MdisInstallPath%\src\INCLUDE\COM\MEN\ > NUL
   if exist LIBSRC (
      xcopy /S /E  LIBSRC\* %G_MdisInstallPath%\src\LIBSRC\ > NUL
   )
   cd ..
)


REM -----------------------------
REM -- copy M077 Drivers content
REM -----------------------------
cd 13M077-40
echo Copying native Driver 13M077-40 to %G_MdisInstallPath%\src\DRIVERS\
xcopy /S /E /I DRIVERS\* %G_MdisInstallPath%\src\DRIVERS\ > NUL
xcopy /S /E INCLUDE\NATIVE\MEN\* %G_MdisInstallPath%\src\INCLUDE\NATIVE\MEN\ > NUL 2>&1
xcopy /S /E /I TOOLS\* %G_MdisInstallPath%\src\TOOLS\ > NUL
cd ..
)

REM ------------------------------------------------------------
REM -- copy XML files of all drivers, therefore ('dir /B 13*')
REM ------------------------------------------------------------
echo Copying XML Files %G_MdisInstallPath%\PACKAGE_DESC ...
mkdir %G_MdisInstallPath%\src\PACKAGE_DESC
for /F %%i in ('dir /B 13*') do (
   cd %%i
   echo Copying Low Level Driver %%i to %G_MdisInstallPath%\src\PACKAGE_DESC
   copy .\PACKAGE_DESC\*.xml %G_MdisInstallPath%\src\PACKAGE_DESC\ > NUL
   cd ..
)

if "%G_silentMode%"=="1" goto :NEXT3
echo.
echo Success! Installation is finished. Your MDIS for QNX System package plus drivers has been installed to %G_MdisInstallPath%.
echo Your next possibilities: create a MDIS project using MDIS wizard
echo (%G_MdisInstallPath%/NT/OBJ/EXE/MEN/I386/FREE/mdiswizqnx.exe) or open an existing MDIS project with it.
echo See also MDIS for QNX User Guide!
echo.
echo If you want you can now delete the current folder.
echo.
:NEXT3

:END
exit /B
