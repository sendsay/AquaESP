echo off
"c:\program files\7-zip\7z.exe" u -tgzip .\html\index .\html\index.html
IF %ERRORLEVEL% GTR 0 goto error
"c:\program files\7-zip\7z.exe" u -tgzip .\html\aqua .\html\aqua.html
IF %ERRORLEVEL% GTR 0 goto error
"c:\program files\7-zip\7z.exe" u -tgzip .\html\light .\html\light.html
IF %ERRORLEVEL% GTR 0 goto error
"c:\program files\7-zip\7z.exe" u -tgzip .\html\style .\html\style.css
IF %ERRORLEVEL% GTR 0 goto error
"c:\program files\7-zip\7z.exe" u -tgzip .\html\style-aqua .\html\style-aqua.css
IF %ERRORLEVEL% GTR 0 goto error
"c:\program files\7-zip\7z.exe" u -tgzip .\html\style-light .\html\style-light.css
IF %ERRORLEVEL% GTR 0 goto error
REM "c:\program files\7-zip\7z.exe" u -tgzip .\html\script .\html\script.js

rename .\html\index.gz index.html.gz
IF %ERRORLEVEL% GTR 0 goto error
rename .\html\aqua.gz aqua.html.gz
IF %ERRORLEVEL% GTR 0 goto error
rename .\html\light.gz light.html.gz
IF %ERRORLEVEL% GTR 0 goto error
rename .\html\style.gz style.css.gz
IF %ERRORLEVEL% GTR 0 goto error
rename .\html\style-aqua.gz style-aqua.css.gz
IF %ERRORLEVEL% GTR 0 goto error
rename .\html\style-light.gz style-light.css.gz
IF %ERRORLEVEL% GTR 0 goto error
REM rename .\html\script.gz script.js.gz

move /Y .\html\index.html.gz .\data
IF %ERRORLEVEL% GTR 0 goto error
move /Y .\html\aqua.html.gz .\data
IF %ERRORLEVEL% GTR 0 goto error
move /Y .\html\light.html.gz .\data
IF %ERRORLEVEL% GTR 0 goto error
move /Y .\html\style.css.gz .\data
IF %ERRORLEVEL% GTR 0 goto error
move /Y .\html\style-aqua.css.gz .\data
IF %ERRORLEVEL% GTR 0 goto error
move /Y .\html\style-light.css.gz .\data
IF %ERRORLEVEL% GTR 0 goto error

REM move /Y .\html\script.js.gz .\data

C:\Users\professional\.platformio\penv\Scripts\platformio.exe run --target erase
IF %ERRORLEVEL% GTR 0 goto error
C:\Users\professional\.platformio\penv\Scripts\platformio.exe run --target uploadfs
IF %ERRORLEVEL% GTR 0 goto error
C:\Users\professional\.platformio\penv\Scripts\platformio.exe run --target upload --target monitor
IF %ERRORLEVEL% GTR 0 goto error
EXIT

:error
ECHO *******************************************
ECHO ****************** ERROR ******************
ECHO *******************************************
pause
EXIT
