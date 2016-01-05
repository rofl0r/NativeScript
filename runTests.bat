@echo off
@setlocal 

:: Read input (number of repeats - default is 10)
set repeats=10
set argC=0
for %%x in (%*) do Set /A argC+=1
if %argC% == 1 if %1 GTR 0 set repeats=%1
echo Running each test %repeats% times.

:: Start measuring total time
set start=%time%

:: Run the tests
setlocal enabledelayedexpansion
ECHO %DATE% %TIME% > output.csv
PATH=%PATH%;%~dp0lib\sdl;%~dp0lib\mono

cd scriptSpeed
FOR %%A IN (js lua cs ns) DO (
	FOR %%B IN (
		"expression 2"
		"expression 3"
		"expression 4"
		"expression 5"
		"expression 6"
		"expression 2 opt"
		"expression 3 opt"
		"expression 4 opt"
		"expression 5 opt"
		"expression 6 opt"
		"callback 0"
		"callback 1"
		"callback 2"
		"callback 3"
		"callback 4"
	) DO (
		ECHO. >> ..\output.csv
		ECHO %%A %%~B >> ..\output.csv
		ECHO %%A %%~B
		FOR /L %%C IN (1,1,%repeats%) DO (
			..\Release\scriptSpeed.exe %%A %%~B >> ..\output.csv
			ECHO. >> ..\output.csv
			if !errorlevel! neq 0 exit /b !errorlevel!
		)
	)
)

FOR %%A IN (js lua cs) DO (
	FOR %%B IN (
		"pointSimul allScript"
		"pointSimul allNative"
		"pointSimul allScriptMinCallback"
		"pointSimul scriptToNative"
	) DO (
		ECHO. >> ..\output.csv
		ECHO %%A %%~B >> ..\output.csv
		ECHO %%A %%~B
		FOR /L %%C IN (1,1,%repeats%) DO (
			..\Release\scriptSpeed.exe %%A %%~B >> ..\output.csv
			ECHO. >> ..\output.csv
			if !errorlevel! neq 0 exit /b !errorlevel!
		)
	)
)

FOR %%B IN (
	"cs pointSimul allScriptStruct"
	"cs pointSimul allScriptMinCallbackStruct"
	"cs pointSimul scriptToNativeStruct"
) DO (
	ECHO. >> ..\output.csv
	ECHO %%~B >> ..\output.csv
	ECHO %%~B
	FOR /L %%C IN (1,1,%repeats%) DO (
		..\Release\scriptSpeed.exe %%~B >> ..\output.csv
		ECHO. >> ..\output.csv
		if !errorlevel! neq 0 exit /b !errorlevel!
	)
)


:: Compute total time
set end=%time%
set options="tokens=1-4 delims=:."
for /f %options% %%a in ("%start%") do set start_h=%%a&set /a start_m=100%%b %% 100&set /a start_s=100%%c %% 100&set /a start_ms=100%%d %% 100
for /f %options% %%a in ("%end%") do set end_h=%%a&set /a end_m=100%%b %% 100&set /a end_s=100%%c %% 100&set /a end_ms=100%%d %% 100

set /a hours=%end_h%-%start_h%
set /a mins=%end_m%-%start_m%
set /a secs=%end_s%-%start_s%
set /a ms=%end_ms%-%start_ms%
if %hours% lss 0 set /a hours = 24%hours%
if %mins% lss 0 set /a hours = %hours% - 1 & set /a mins = 60%mins%
if %secs% lss 0 set /a mins = %mins% - 1 & set /a secs = 60%secs%
if %ms% lss 0 set /a secs = %secs% - 1 & set /a ms = 100%ms%
if 1%ms% lss 100 set ms=0%ms%

set /a totalsecs = %hours%*3600 + %mins%*60 + %secs% 
echo Total runtime: %hours%:%mins%:%secs%.%ms% (%totalsecs%.%ms%s)
