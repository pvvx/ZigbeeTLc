@set TLPATH=D:\MCU\TelinkIoTStudio
@set PATH=%TLPATH%\bin;%TLPATH%\opt\tc32\bin;%TLPATH%\mingw\bin;%TLPATH%\opt\tc32\tc32-elf\bin;%PATH%
@set SWVER=_v0134
@del /Q .\bin\*.bin
@del /Q .\bin\*.zigbee
@del /Q .\build
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZMHOC401 POJECT_DEF="-DBOARD=BOARD_MHO_C401" ZNAME="MiaoMiaoCe:MHO-C401-z"
@if not exist "bin\ZMHOC401%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZCGG1 POJECT_DEF="-DBOARD=BOARD_CGG1" ZNAME="Qingping:CGG1-z"
@if not exist "bin\ZCGG1%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZCGDK2 POJECT_DEF="-DBOARD=BOARD_CGDK2" ZNAME="Qingping:CGDK2-z"
@if not exist "bin\ZCGDK2%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZCGG1N POJECT_DEF="-DBOARD=BOARD_CGG1N" ZNAME="Qingping:CGG1N-z"
@if not exist "bin\ZCGG1N%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZMHOC401N POJECT_DEF="-DBOARD=BOARD_MHO_C401N" ZNAME="MiaoMiaoCe:MHO-C401N-z"
@if not exist "bin\ZMHOC401N%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=Z03MMC POJECT_DEF="-DBOARD=BOARD_LYWSD03MMC" ZNAME="Xiaomi:LYWSD03MMC-z"
@if not exist "bin\Z03MMC%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZMHOC122 POJECT_DEF="-DBOARD=BOARD_MHO_C122" ZNAME="MiaoMiaoCe:MHO-C122-z"
@if not exist "bin\ZMHOC122%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=Z06MMC POJECT_DEF="-DBOARD=BOARD_MJWSD06MMC" ZNAME="Xiaomi:MJWSD06MMC-z"
@if not exist "bin\Z06MMC%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZTS0201Z3000 POJECT_DEF="-DBOARD=BOARD_TS0201_TZ3000" ZNAME="Tuya:TS0201-z"
@if not exist "bin\ZTS0201Z3000%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZTH03Z POJECT_DEF="-DBOARD=BOARD_TH03Z" ZNAME="Tuya:TH03Z-z"
@if not exist "bin\ZTH03Z%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZTH01Z POJECT_DEF="-DBOARD=BOARD_ZTH01" ZNAME="Tuya:ZTH01-z"
@if not exist "bin\ZTH01Z%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZTH02Z POJECT_DEF="-DBOARD=BOARD_ZTH02" ZNAME="Tuya:ZTH02-z"
@if not exist "bin\ZTH02Z%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=TH03Z POJECT_DEF="-DBOARD=BOARD_ZTH03" ZNAME="Sonoff:TH03-z"
@if not exist "bin\TH03Z%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=LKTMZL02Z POJECT_DEF="-DBOARD=BOARD_LKTMZL02" ZNAME="Tuya:LKTMZL02-z"
@if not exist "bin\LKTMZL02Z%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZTH05 POJECT_DEF="-DBOARD=BOARD_ZTH05" ZNAME="Tuya:TH05-z"
@if not exist "bin\ZTH05%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZYZTH02 POJECT_DEF="-DBOARD=BOARD_ZYZTH02" ZNAME="Tuya:ZY-ZTH02-z"
@if not exist "bin\ZYZTH02%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZYZTH01 POJECT_DEF="-DBOARD=BOARD_ZYZTH01" ZNAME="Tuya:ZY-ZTH01-z"
@if not exist "bin\ZYZTH01%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZG227Z POJECT_DEF="-DBOARD=BOARD_ZG_227Z" ZNAME="Tuya:ZG-227Z-z"
@if not exist "bin\ZG227Z%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZG303Z POJECT_DEF="-DBOARD=BOARD_ZG303Z" ZNAME="Sonoff:ZG-303Z-z"
@if not exist "bin\ZG303Z%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZBTH01 POJECT_DEF="-DBOARD=BOARD_ZBEACON_TH01" ZNAME="ZBeacon:TH01-z"
@if not exist "bin\ZBTH01%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZB_MC POJECT_DEF="-DBOARD=BOARD_ZB_MC" ZNAME="ZBeacon:MC-z"
@if not exist "bin\ZB_MC%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZB2TH01 POJECT_DEF="-DBOARD=BOARD_ZBEACON2_TH01" ZNAME="ZBeacon:TH01-2-z"
@if not exist "bin\ZB2TH01%SWVER%.bin" goto :error
make -s -j clean
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZTY0201 POJECT_DEF="-DBOARD=BOARD_RSH_HS03" ZNAME="Tuya:TY0201-z"
@if not exist "bin\ZTY0201%SWVER%.bin" goto :error
@rem
python3 make_z\zb_bin_ota.py bin\ZTS0201Z3000%SWVER%.bin bin\TS0201z%SWVER% -m0x1141 -i0xd3a3 -v0x01983001 -s"Tuya to ZigbeeTlc"
python3 make_z\zb_bin_ota.py bin\ZTH01Z%SWVER%.bin bin\TH01Zz%SWVER% -m0x1141 -i0xd3a3 -v0x01993001 -s"Tuya to ZigbeeTlc"
python3 make_z\zb_bin_ota.py bin\ZTH02Z%SWVER%.bin bin\TH02Zz%SWVER% -m0x1141 -i0xd3a3 -v0x01993001 -s"Tuya to ZigbeeTlc"
python3 make_z\zb_bin_ota.py bin\ZTH03Z%SWVER%.bin bin\TH03Zz%SWVER% -m0x1141 -i0xd3a3 -v0x01993001 -s"Tuya to ZigbeeTlc"
python3 make_z\zb_bin_ota.py bin\TH03Z%SWVER%.bin bin\TH03Zz%SWVER% -m0x1286 -i0x0202 -v0x10993607 -s"Sonoff to ZigbeeTlc"
python3 make_z\zb_bin_ota.py bin\LKTMZL02Z%SWVER%.bin bin\LKTMZL02z%SWVER% -m0x1141 -i0xd3a3 -v0x01993001 -s"Tuya to ZigbeeTlc"
python3 make_z\zb_bin_ota.py bin\ZYZTH02%SWVER%.bin bin\ZYZTH02z%SWVER% -m0x1002 -i0xd3a3 -v0x66993001 -s"Tuya to ZigbeeTlc"
python3 make_z\zb_bin_ota.py bin\ZYZTH01%SWVER%.bin bin\ZYZTH03pz%SWVER% -m0x1002 -i0xd3a3 -v0x66993001 -s"Tuya to ZigbeeTlc"
python3 make_z\zb_bin_ota.py bin\ZG227Z%SWVER%.bin bin\ZG227Zz%SWVER% -m0x1286 -i0x0203 -v0x10983001 -s"Sonoff to ZigbeeTlc"
python3 make_z\zb_bin_ota.py bin\ZG303Z%SWVER%.bin bin\ZG303Zz%SWVER% -m0x1286 -i0x0203 -v0x20973001 -s"Sonoff to ZigbeeTlc"
python3 make_z\zb_bin_ota.py bin\ZBTH01%SWVER%.bin bin\ZBTH01z%SWVER% -m0x1286 -i0x0202 -v0x10933607 -s"Sonoff to ZigbeeTlc"
python3 make_z\zb_bin_ota.py bin\ZB2TH01%SWVER%.bin bin\ZB2TH01z%SWVER% -m0x1286 -i0x0202 -v0x10933607 -s"Sonoff to ZigbeeTlc"
python3 make_z\zb_bin_ota.py bin\ZTY0201%SWVER%.bin bin\ZTY0201z%SWVER% -m0x1141 -i0x0203 -v0x10923001 -s"Tuya to ZigbeeTlc"
@rem
cd .\zigpy_ota
call update.cmd %SWVER%
cd ..
@exit
:error
echo "Error!"

         