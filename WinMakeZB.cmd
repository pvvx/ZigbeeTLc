@set TLPATH=D:\MCU\TelinkIoTStudio
@set PATH=%TLPATH%\bin;%TLPATH%\opt\tc32\bin;%TLPATH%\mingw\bin;%TLPATH%\opt\tc32\tc32-elf\bin;%PATH%
@set SWVER=_zb0004
@del /Q .\bin
@del /Q .\build
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZMHOC401 USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_MHO_C401"
@if not exist "bin\ZMHOC401%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZCGG1 USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_CGG1"
@if not exist "bin\ZCGG1%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZCGDK2 USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_CGDK2"
@if not exist "bin\ZCGDK2%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZCGG1N USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_CGG1N"
@if not exist "bin\ZCGG1N%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZMHOC401N USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_MHO_C401N"
@if not exist "bin\ZMHOC401N%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=Z03MMC USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_LYWSD03MMC"
@if not exist "bin\Z03MMC%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZMHOC122 USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_MHO_C122"
@if not exist "bin\ZMHOC122%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZTS0201Z3000 USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_TS0201_TZ3000"
@if not exist "bin\ZTS0201Z3000%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZTH03Z USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_TH03Z"
@if not exist "bin\ZTH03Z%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZTH01Z USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_ZTH01"
@if not exist "bin\ZTH01Z%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZTH02Z USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_ZTH02"
@if not exist "bin\ZTH02Z%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=TH03Z USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_ZTH03"
@if not exist "bin\TH03Z%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=LKTMZL02Z USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_LKTMZL02"
@if not exist "bin\LKTMZL02Z%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZTH05 USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_ZTH05"
@if not exist "bin\ZTH05%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZYZTH02 USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_ZYZTH02"
@if not exist "bin\ZYZTH02%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZYZTH01 USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_ZYZTH01"
@if not exist "bin\ZYZTH01%SWVER%.bin" goto :error
make -s -j clean USE_ZB=1
make -s -j VERSION_BIN=%SWVER% PROJECT_NAME=ZG227Z USE_ZB=1 POJECT_DEF="-DBOARD=BOARD_ZG_227Z"
@if not exist "bin\ZG227Z%SWVER%.bin" goto :error
@exit
:error
echo "Error!"

         