REM echo off
"c:\program files\7-zip\7z.exe" u -tgzip .\html\index .\html\index.html
"c:\program files\7-zip\7z.exe" u -tgzip .\html\aqua .\html\aqua.html
"c:\program files\7-zip\7z.exe" u -tgzip .\html\style .\html\style.css
"c:\program files\7-zip\7z.exe" u -tgzip .\html\style-aqua .\html\style-aqua.css
REM "c:\program files\7-zip\7z.exe" u -tgzip .\html\script .\html\script.js

rename .\html\index.gz index.html.gz
rename .\html\aqua.gz aqua.html.gz
rename .\html\style.gz style.css.gz
rename .\html\style-aqua.gz style-aqua.css.gz
REM rename .\html\script.gz script.js.gz

move /Y .\html\index.html.gz .\data
move /Y .\html\aqua.html.gz .\data
move /Y .\html\style.css.gz .\data
move /Y .\html\style-aqua.css.gz .\data


REM move /Y .\html\script.js.gz .\data

C:\Users\Professional\.platformio\penv\Scripts\platformio.exe run --target erase
C:\Users\Professional\.platformio\penv\Scripts\platformio.exe run --target uploadfs
C:\Users\Professional\.platformio\penv\Scripts\platformio.exe run --target upload --target monitor
