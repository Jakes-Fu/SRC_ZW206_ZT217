@title make package
@echo off

::#####################################################################################################
::set target project name
set proj=UWS6121E_WC_1H00_USER
::#####################################################################################################

set build_dir=%proj%_builddir
set stone_img=UWS6121E_%proj%_stone.img
set stone_bin=UWS6121E_%proj%_stone.img.bin

echo %build_dir%

for /f "eol=/ tokens=3" %%i in ('findstr CMIOT_FIRMWARE_VERSION oneos_config.h') do (set version=%%i)

if "%version%" == "" (
    echo ***version is empty***
    echo check oneos_config.h CMIOT_FIRMWARE_VERSION
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
copy ..\..\build\%build_dir%\img\* .\%version%
copy ..\..\make\perl_script\UWS6121E.xml .\%version%\UWS6121E.xml.bin

::rename image files
cd .\%version%
if exist %stone_bin% (
    del %stone_bin%
    echo clear old %stone_bin%
) else (
    echo %stone_bin% 
)
copy %stone_img% %stone_bin%
cd ..

::generate package
set "WinRAR=C:\Program Files\WinRAR\WinRAR.exe"
set "Zip=C:\Program Files\7-Zip\7z.exe"

if exist "%WinRAR%" (
    call ota\cmiot\source\pack\cmiot_pack.bat "%WinRAR%" oneos_config.h %version%
) else (
    if exist "%Zip%" (
        call ota\cmiot\source\pack\cmiot_pack.bat "%Zip%" oneos_config.h .\%version%\
    ) else (
        echo "zip tool not found"
    )
)

:EOF