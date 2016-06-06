@echo off
cd ..
mkdir chunks
cd nbt/
echo Converting nbt files to YAML
FOR /D %%G IN ("region/r.*") DO (
	FOR %%i IN ("region/%%G/*.nbt") DO call :loop %%G %%i
)
ping 192.0.2.2 -n 1 -w 5000 > nul
echo Finished conversion
echo Making list of chunks
DIR "../chunks/"*.nbt /b > ../chunk.lst
rmdir /s /q region
goto :eof

:loop
call :checkinstances
if %INSTANCES% LSS 5 (
    start /wait /b python nbt2yaml -n region/%~1/%~2 > ../chunks/%~1.%~2
    goto :eof
)
rem wait a second, can be adjusted with -w (-n 2 because the first ping returns immediately;
rem otherwise just use an address that's unused and -n 1)
echo Waiting for instances to close ...
ping -n 2 ::1 >nul 2>&1
rem jump back to see whether we can spawn a new process now
goto loop
goto :eof

:checkinstances
rem this could probably be done better. But INSTANCES should contain the number of running instances afterwards.
for /f "usebackq" %%t in (`tasklist /fo csv /fi "imagename eq python.exe"^|find /c /v ""`) do set INSTANCES=%%t
goto :eof