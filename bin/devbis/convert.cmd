@if not exist "..\Z03MMC%1.bin" goto :error
del /Q *.zigbee
python3 ..\..\make_z\zigbee_ota.py z03mmc.bin
python3 ..\..\make_z\zb_bin_ota.py z03mmc.bin z03mmc_v1100 -m0x1141 -i0x020a -v0x09993001 -s"ZigbeeTLc to devbis"
python3 ..\..\make_z\zb_bin_ota.py ..\Z03MMC%1.bin Z03MMC%1 -m0x1141 -i0x0203 -v0x79993001 -s"devbis to ZigbeeTLc"
python3 ..\..\make_z\zb_bin_ota.py ..\Z03MMC%1.bin Z03MMC%1 -m0xDB15 -i0x0203 -v0x79993001 -s"devbis to ZigbeeTLc"
@exit
:error
echo "Error!"
