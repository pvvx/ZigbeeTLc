@set TLPATH=D:\MCU\TelinkIoTStudio
@set PATH=%TLPATH%\bin;%TLPATH%\opt\tc32\bin;%TLPATH%\mingw\bin;%TLPATH%\opt\tc32\tc32-elf\bin;%PATH%
@set SWVER=_v0125
@rem del /Q .\bin
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZMHOC401 POJECT_DEF="-DBOARD=BOARD_MHO_C401"
@if not exist "bin\ZMHOC401%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZCGG1 POJECT_DEF="-DBOARD=BOARD_CGG1"
@if not exist "bin\ZCGG1%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZCGDK2 POJECT_DEF="-DBOARD=BOARD_CGDK2"
@if not exist "bin\ZCGDK2%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZCGG1N POJECT_DEF="-DBOARD=BOARD_CGG1N"
@if not exist "bin\ZCGG1N%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZMHOC401N POJECT_DEF="-DBOARD=BOARD_MHO_C401N"
@if not exist "bin\ZMHOC401N%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=Z03MMC POJECT_DEF="-DBOARD=BOARD_LYWSD03MMC"
@if not exist "bin\Z03MMC%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZMHOC122 POJECT_DEF="-DBOARD=BOARD_MHO_C122"
@if not exist "bin\ZMHOC122%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZTS0201Z3000 POJECT_DEF="-DBOARD=BOARD_TS0201_TZ3000"
@if not exist "bin\ZTS0201Z3000%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZTH03Z POJECT_DEF="-DBOARD=BOARD_TH03Z"
@if not exist "bin\ZTH03Z%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZTH01Z POJECT_DEF="-DBOARD=BOARD_ZTH01"
@if not exist "bin\ZTH01Z%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZTH02Z POJECT_DEF="-DBOARD=BOARD_ZTH02"
@if not exist "bin\ZTH02Z%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=TH03Z POJECT_DEF="-DBOARD=BOARD_ZTH03"
@if not exist "bin\TH03Z%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=LKTMZL02Z POJECT_DEF="-DBOARD=BOARD_LKTMZL02"
@if not exist "bin\LKTMZL02Z%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZTH05 POJECT_DEF="-DBOARD=BOARD_ZTH05"
@if not exist "bin\ZTH05%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZYZTH02 POJECT_DEF="-DBOARD=BOARD_ZYZTH02"
@if not exist "bin\ZYZTH02%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZYZTH01 POJECT_DEF="-DBOARD=BOARD_ZYZTH01"
@if not exist "bin\ZYZTH01%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZG227Z POJECT_DEF="-DBOARD=BOARD_ZG_227Z"
@if not exist "bin\ZG227Z%SWVER%.bin" goto :error
python3 make_z\zb_bin_ota.py bin\ZTS0201Z3000%SWVER%.bin bin\TS0201z%SWVER% -m0x1141 -i0xd3a3 -v0x01983001 -s"Tuya to ZigbeeTlc"
python3 make_z\zb_bin_ota.py bin\ZTH01Z%SWVER%.bin bin\TH01Zz%SWVER% -m0x1141 -i0xd3a3 -v0x01993001 -s"Tuya to ZigbeeTlc"
python3 make_z\zb_bin_ota.py bin\ZTH02Z%SWVER%.bin bin\TH02Zz%SWVER% -m0x1141 -i0xd3a3 -v0x01993001 -s"Tuya to ZigbeeTlc"
python3 make_z\zb_bin_ota.py bin\ZTH03Z%SWVER%.bin bin\TH03Zz%SWVER% -m0x1141 -i0xd3a3 -v0x01993001 -s"Tuya to ZigbeeTlc"
python3 make_z\zb_bin_ota.py bin\TH03Z%SWVER%.bin bin\TH03Zz%SWVER% -m0x1286 -i0x0202 -v0x10993607 -s"Sonoff to ZigbeeTlc"
python3 make_z\zb_bin_ota.py bin\LKTMZL02Z%SWVER%.bin bin\LKTMZL02z%SWVER% -m0x1141 -i0xd3a3 -v0x01993001 -s"Tuya to ZigbeeTlc"
python3 make_z\zb_bin_ota.py bin\ZYZTH02%SWVER%.bin bin\ZYZTH02z%SWVER% -m0x1002 -i0xd3a3 -v0x66993001 -s"Tuya to ZigbeeTlc"
python3 make_z\zb_bin_ota.py bin\ZG227Z%SWVER%.bin bin\ZG227Zz%SWVER% -m0x1286 -i0x0203 -v0x10983001 -s"Tuya to ZigbeeTlc"
@exit
:error
echo "Error!"

         