@echo off
setlocal EnableExtensions
rem ---------------------------------------------------------------------------
rem Builds the Liana firmware and the LittleFS image, then uploads both.
rem
rem   deploy.bat serial [COMx] [env]      upload over the COM port (auto-detected when omitted)
rem   deploy.bat ota ip-or-host [env]   upload over WiFi (ArduinoOTA); the host is required:
rem                                       the IP or liana-xxxxxx.local (printed to the COM port at startup)
rem
rem   env defaults to nodemcuv2, see platformio.ini for the list (esp32dev, d1_mini, ...).
rem   OTA password: the firmware default is used unless OTA_PASSWORD is set in the environment.
rem
rem OTA notes: the firmware starts its OTA listener only after POST /ota (the "OTA" button of the
rem web interface), and the unit reboots after every upload. So the script arms OTA before the
rem firmware upload and once more before the file system upload, waiting for the unit to come
rem back in between. The firmware goes first so that a file system of a new layout is written
rem for the firmware that understands it.
rem ---------------------------------------------------------------------------

cd /d "%~dp0"

set "METHOD=%~1"
set "TARGET=%~2"
set "ENV=%~3"
if "%ENV%"=="" set "ENV=nodemcuv2"
if not defined OTA_PASSWORD set "OTA_PASSWORD=123OtA!_456"

if /i "%METHOD%"=="serial" goto build
if /i "%METHOD%"=="ota" goto build
echo Usage:
echo   deploy.bat serial [COMx] [env]
echo   deploy.bat ota ip-or-host [env]
echo Examples:
echo   deploy.bat serial COM5
echo   deploy.bat ota 192.168.0.31
echo   deploy.bat ota liana-a1b2c3.local esp32dev
exit /b 2

:build
where pio >NUL 2>&1 || (
  if exist "%USERPROFILE%\.platformio\penv\Scripts" set "PATH=%USERPROFILE%\.platformio\penv\Scripts;%PATH%"
)
where pio >NUL 2>&1 || (echo pio was not found in PATH, install PlatformIO Core first & exit /b 1)

echo === [1/4] Building firmware (%ENV%)
call pio run -e %ENV% || goto fail
echo === [2/4] Building the file system image
call pio run -e %ENV% -t buildfs || goto fail

if /i "%METHOD%"=="ota" goto ota

:serial
set "PORTARG="
if not "%TARGET%"=="" set "PORTARG=--upload-port %TARGET%"
echo === [3/4] Uploading firmware over the COM port %TARGET%
call pio run -e %ENV% -t upload %PORTARG% || goto fail
echo === [4/4] Uploading the file system over the COM port %TARGET%
call pio run -e %ENV% -t uploadfs %PORTARG% || goto fail
goto done

:ota
if "%TARGET%"=="" (echo deploy.bat ota needs the IP or the liana-xxxxxx.local name of the unit & exit /b 2)
set "PLATFORMIO_UPLOAD_PROTOCOL=espota"
set "PLATFORMIO_UPLOAD_PORT=%TARGET%"
set "PLATFORMIO_UPLOAD_FLAGS=--auth=%OTA_PASSWORD%, --port=8266"
call :arm || goto fail
echo === [3/4] Uploading firmware over OTA to %TARGET%
call pio run -e %ENV% -t upload || goto fail
call :waitup || goto fail
call :arm || goto fail
echo === [4/4] Uploading the file system over OTA to %TARGET%
call pio run -e %ENV% -t uploadfs || goto fail
goto done

:arm
echo === Arming OTA on %TARGET% (POST /ota)
curl -s -f --connect-timeout 5 -o NUL -X POST "http://%TARGET%/ota" || (echo Could not reach http://%TARGET%/ota & exit /b 1)
ping -n 3 127.0.0.1 >NUL
exit /b 0

:waitup
echo === Waiting for %TARGET% to reboot
set /a TRIES=0
:waitloop
ping -n 4 127.0.0.1 >NUL
curl -s -f --connect-timeout 5 -o NUL "http://%TARGET%/api/state" && exit /b 0
set /a TRIES+=1
if %TRIES% geq 30 (echo %TARGET% did not come back within 2 minutes & exit /b 1)
goto waitloop

:fail
echo.
echo *** deploy FAILED
exit /b 1

:done
echo.
echo === Done: firmware and file system uploaded (%ENV%, %METHOD% %TARGET%)
exit /b 0
