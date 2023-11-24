@set SWVER=_v0112
@del /Q .\bin
wsl make -s clean
wsl make -s VERSION_BIN=%SWVER% PROJECT_NAME=Z03MMC POJECT_DEF="-DBOARD=BOARD_LYWSD03MMC"
@if not exist "bin\Z03MMC%SWVER%.bin" goto :error
wsl make -s clean
wsl make -s VERSION_BIN=%SWVER% PROJECT_NAME=ZCGDK2 POJECT_DEF="-DBOARD=BOARD_CGDK2"
@if not exist "bin\ZCGDK2%SWVER%.bin" goto :error
wsl make -s clean
wsl make -s VERSION_BIN=%SWVER% PROJECT_NAME=ZMHOC122 POJECT_DEF="-DBOARD=BOARD_MHO_C122"
@if not exist "bin\ZMHOC122%SWVER%.bin" goto :error
wsl make -s clean
wsl make -s VERSION_BIN=%SWVER% PROJECT_NAME=ZMHOC401N POJECT_DEF="-DBOARD=BOARD_MHO_C401N"
@if not exist "bin\ZMHOC122%SWVER%.bin" goto :error
wsl make -s clean
wsl make -s VERSION_BIN=%SWVER% PROJECT_NAME=ZTS0201Z3000 POJECT_DEF="-DBOARD=BOARD_TS0201_TZ3000"
@if not exist "bin\ZTS0201Z3000%SWVER%.bin" goto :error
@exit
:error
echo "Error!"

         