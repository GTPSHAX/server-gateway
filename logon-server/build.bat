@echo off
setlocal

REM Default config
set CONFIG=Debug
set CONFIG_NAME=debug

REM Cek flag --release
if "%1"=="--release" (
    set CONFIG=Release
    set CONFIG_NAME=release
)

REM Build dengan CMake preset
cmake --preset vs2022-x86-%CONFIG_NAME%
cmake --build out\%CONFIG% --config %CONFIG%

REM Hapus folder bin jika ada
if exist bin\%CONFIG% rmdir /S /Q bin\%CONFIG%

REM Pindahkan folder hasil build ke bin/
move out\%CONFIG%\%CONFIG% bin

echo Build selesai untuk konfigurasi %CONFIG%. Output ada di folder bin\
endlocal