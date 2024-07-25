@title make package
@echo off

set version=%1


if "%version%" == "" (
	echo ***version is empty***
	echo usage  : pack_cmd.bat [version]
	echo example: pack_cmd.bat 1.0.0
	goto EOF
) else (
	echo package source: %version%
)

if exist %version% (
	echo "package source folder %version% exist"
) else (
	md %version%
)

::copy new files
copy ..\..\build\UWS6121E_WC_1H00_NoGNSS_builddir\img\* .\%version%

::rename image files
cd .\%version%
if exist UWS6121E_UWS6121E_WC_1H00_NoGNSS_stone.img.bin (
    del UWS6121E_UWS6121E_WC_1H00_NoGNSS_stone.img.bin
    echo clear old UWS6121E_UWS6121E_WC_1H00_NoGNSS_stone.img.bin
) else (
    echo UWS6121E_UWS6121E_WC_1H00_NoGNSS_stone.img.bin 
)
copy UWS6121E_UWS6121E_WC_1H00_NoGNSS_stone.img UWS6121E_UWS6121E_WC_1H00_NoGNSS_stone.img.bin
cd ..

::generate package
set "WinRAR=C:\Program Files\WinRAR\WinRAR.exe"
set "Zip=C:\Program Files\7-Zip\7z.exe"

if exist "%WinRAR%" (
    call ota\cmiot\pack\cmiot_pack.bat "%WinRAR%" oneos_config.h.txt %version%
) else (
    if exist "%Zip%" (
        ota\cmiot\pack\cmiot_pack.bat "%Zip%" oneos_config.h.txt .\%version%\
    ) else (
        echo "zip tool not found"
    )
)

:EOF