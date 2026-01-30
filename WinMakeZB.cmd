@set TLPATH=D:\MCU\TelinkIoTStudio
@set PATH=%TLPATH%\bin;%TLPATH%\opt\tc32\bin;%TLPATH%\mingw\bin;%TLPATH%\opt\tc32\tc32-elf\bin;%PATH%
@set SWVER=_zb0013
@del /Q .\bin_zb\*.bin
@del /Q .\bin_zb\*.zigbee
@del /Q .\build
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZMHOC401 USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_MHO_C401" ZNAME="MiaoMiaoCe:MHO-C401-z"
@if not exist "bin_zb\ZMHOC401%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZCGG1 USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_CGG1" ZNAME="Qingping:CGG1-z"
@if not exist "bin_zb\ZCGG1%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZCGDK2 USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_CGDK2" ZNAME="Qingping:CGDK2-z"
@if not exist "bin_zb\ZCGDK2%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZCGG1N USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_CGG1N" ZNAME="Qingping:CGG1N-z"
@if not exist "bin_zb\ZCGG1N%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZMHOC401N USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_MHO_C401N" ZNAME="MiaoMiaoCe:MHO-C401N-z"
@if not exist "bin_zb\ZMHOC401N%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=Z03MMC USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_LYWSD03MMC" ZNAME="Xiaomi:LYWSD03MMC-z"
@if not exist "bin_zb\Z03MMC%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZMHOC122 USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_MHO_C122" ZNAME="MiaoMiaoCe:MHO-C122-z"
@if not exist "bin_zb\ZMHOC122%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=Z06MMC USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_MJWSD06MMC" ZNAME="Xiaomi:MJWSD06MMC-z"
@if not exist "bin_zb\Z06MMC%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZTS0201Z3000 USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_TS0201_TZ3000" ZNAME="Tuya:TS0201-z"
@if not exist "bin_zb\ZTS0201Z3000%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZTH03Z USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_TH03Z" ZNAME="Tuya:TH03Z-z"
@if not exist "bin_zb\ZTH03Z%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZTH01Z USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_ZTH01" ZNAME="Tuya:ZTH01-z"
@if not exist "bin_zb\ZTH01Z%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZTH02Z USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_ZTH02" ZNAME="Tuya:ZTH02-z"
@if not exist "bin_zb\ZTH02Z%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=TH03Z USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_ZTH03" ZNAME="Sonoff:TH03-z"
@if not exist "bin_zb\TH03Z%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=LKTMZL02Z USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_LKTMZL02" ZNAME="Tuya:LKTMZL02-z"
@if not exist "bin_zb\LKTMZL02Z%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZTH05 USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_ZTH05" ZNAME="Tuya:TH05-z"
@if not exist "bin_zb\ZTH05%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZYZTH02 USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_ZYZTH02" ZNAME="Tuya:ZY-ZTH02-z"
@if not exist "bin_zb\ZYZTH02%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZYZTH01 USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_ZYZTH01" ZNAME="Tuya:ZY-ZTH01-z"
@if not exist "bin_zb\ZYZTH01%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZG227Z USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_ZG_227Z" ZNAME="Tuya:ZG-227Z-z"
@if not exist "bin_zb\ZG227Z%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZG303Z USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_ZG303Z" ZNAME="Sonoff:ZG-303Z-z"
@if not exist "bin_zb\ZG303Z%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZBTH01 USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_ZBEACON_TH01" ZNAME="ZBeacon:TH01-z"
@if not exist "bin_zb\ZBTH01%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZB_MC USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_ZB_MC" ZNAME="ZBeacon:MC-z"
@if not exist "bin_zb\ZB_MC%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZTY0201 USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_RSH_HS03" ZNAME="Tuya:TY0201-z"
@if not exist "bin_zb\ZTY0201%SWVER%.bin" goto :error
cd .\zigpy_ota
call update_zb.cmd %SWVER%
cd ..
@exit
:error
echo "Error!"

         