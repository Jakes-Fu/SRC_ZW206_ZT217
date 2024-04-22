@echo off 
setlocal EnableDelayedExpansion

set bin_patch=%1
set zip_patch=%2
set def_path=%3
set axf_path=%4
set version=version
set algorithm=0
set appaddr=0
set block=0
set wosun=0
set size=0
set lusun_ver=0
set wosun_ver=0
set luckspar_ver=0
set app_ver=0
set ram_size=0
set dl_size=0
set cmiot=oneos.bin
set info_file=cmiot_info.txt
set fromelf=fromelf.exe

if not "x!bin_patch:%fromelf%=!"=="x%bin_patch%" (
    %bin_patch% --bin -o %cmiot% %axf_path%
) else (
    echo N
)

for /f "tokens=2 delims==" %%a in ('wmic path win32_operatingsystem get LocalDateTime /value') do (
    set datetime=%%a
)
set d=%datetime:~0,4%%datetime:~4,2%%datetime:~6,2%
set h=%time:~0,2%
set h=%h: =0%
set t=%h%%time:~3,2%%time:~6,2%

call:findstr1 CMIOT_FIRMWARE_VERSION %def_path% version
call:findstr1 CMIOT_FOTA_LUSUN_VERSION %def_path% lusun_ver
call:findstr1 CMIOT_FOTA_WOSUN_VERSION %def_path% wosun_ver
call:findstr1 CMIOT_FOTA_LUCKSPAR_VERSION %def_path% luckspar_ver
call:findstr2 CMIOT_FOTA_ALGORITHM %def_path% algorithm
call:findstr2 CMIOT_FLASH_APP_ADDR %def_path% appaddr
call:findstr2 CMIOT_DEFAULT_SECTOR_SIZE %def_path% block
call:findstr2 CMIOT_UPDATE_SIZE %def_path% size
call:findstr2 CMIOT_FOTA_AVAILABLE_RAM_SIZE %def_path% ram_size

set /A addr=appaddr
call:hexfun %addr% addr1
set "addr1=00000000%addr1%"
set "addr1=%addr1:~-8,8%"

set /A size=size/1024
set /A dl_size=size
set /A ram_size=ram_size/1024
set /A block=block/1024
echo cmiot fota used size=%size%
set /A size=size-block
echo cmiot fota delta size=%size%

findstr "CMIOT_USING_CMS" %def_path% > nul
if %errorlevel% equ 0 (
    call %~dp0\cms_info.bat
    if !errorlevel! neq 0 (
        GOTO :EOF
    )
    GOTO:pack
)

findstr  "CMIOT_FOTA_SERVICE" %def_path% > %info_file%

if "%algorithm%"=="0" (
    echo #define CMIOT_FOTA_SDK_VER "%lusun_ver%" >> %info_file%
) else if "%algorithm%"=="3" (
    echo #define CMIOT_FOTA_SDK_VER "%luckspar_ver%" >> %info_file%
) else (
    echo #define CMIOT_FOTA_SDK_VER "%wosun_ver%" >> %info_file%
)

call:findstr1 CMIOT_FOTA_OS_VERSION %def_path% app_ver
echo %app_ver%
echo #define CMIOT_FOTA_APP_VER "%app_ver%" >> %info_file%

if "%algorithm%"=="1" (set /A wosun=0) else (set /A wosun=1)
if "%algorithm%"=="0" (echo lusun) else if "%algorithm%"=="3" (
    echo #define CMIOT_FOTA_AVAILABLE_RAM_SIZE "%ram_size%" >> %info_file%
    echo #define CMIOT_FOTA_DOWNLOAD_PART_SIZE "%dl_size%" >> %info_file%
) else if "%algorithm%"=="4" (echo Full upgrade) else (
    echo #define CMIOT_FOTA_AVAILABLE_RAM_SIZE "%ram_size%" >> %info_file%
    echo #define CMIOT_FOTA_SDK_MAX_MCU_NUM "" >> %info_file%
    echo #define CMIOT_FOTA_FILENAME1 "%cmiot%-D" >> %info_file%
    echo #define CMIOT_FOTA_ADDRESS1   "0x%addr1%" >> %info_file%
    echo #define CMIOT_FOTA_REGION_LEN   "%size%" >> %info_file%
    echo #define CMIOT_FOTA_BLOCK_LEN "%block%" >> %info_file%
    echo #define CMIOT_FOTA_PATCH_FORMAT   "%wosun%" >> %info_file%
    echo #define CMIOT_BUILD_TIME   "%d%_%t%" >> %info_file%
)

echo version=%version% >> %info_file%

:pack
echo+%zip_patch%|findstr "7z.exe" 
if %errorlevel% equ 0 (
    %zip_patch% a -tzip %version%_%d%_%t%.zip %info_file% ./%cmiot%
) else (
    %zip_patch% a -ep1 -o+ -inul  -iback %version%_%d%_%t%.zip %info_file% ./%cmiot%
) 
del %cmiot%
del %info_file%
echo complete
GOTO:EOF

:findstr1
    for /f tokens^=^2^ delims^=^"^= %%i in ('findstr "%1" %2') do set "%3=%%i"
GOTO:EOF

:findstr2
    for /f "tokens=2,3" %%i in ('findstr "%1" %2') do if %%i==%1 set /A "%3=%%j"
GOTO:EOF

:hexfun
    set str=
    set code=0123456789ABCDEF
    set "var=%1"
    :again
    set /a tra=%var%%%16
    call,set tra=%%code:~%tra%,1%%
    set /a var/=16
    call:set_YU %var%
    set str=%tra%%str%
    if %var% geq 16 goto again
    set "%2=%ret%%str%"
GOTO:EOF

:set_YU
    set ret=
    if "%1" == "10" set ret=A
    if "%1" == "11" set ret=B
    if "%1" == "12" set ret=C
    if "%1" == "13" set ret=D
    if "%1" == "14" set ret=E
    if "%1" == "15" set ret=F
    if %1 lss 10 set ret=%1
GOTO:EOF
