@ECHO off

FOR /R d:\tmp\___flash\voip\distorted\ %%G IN (*.ogg) DO (
    echo %%G
    ..\tgvoipcall\build\opusfile-reader\Debug\opusfile-reader-test.exe %%G %%G.wav
    rem c:\Users\Alexander\Downloads\opus-tools-0.2-opus-1.3.1\opusinfo.exe %%G
    rem c:\Users\Alexander\Downloads\opus-tools-0.2-opus-1.3.1\opusdec.exe %%G %%G.wav
)
