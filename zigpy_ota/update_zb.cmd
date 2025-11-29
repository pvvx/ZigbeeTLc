set BASE_URL="https://github.com/pvvx/ZigbeeTLc/raw/refs/heads/master/bin_zb"
echo [] > index.json
for %%a in (../bin_zb/1141-02??-0???3001-*.zigbee) do (
start /wait node scripts/add.js ../bin_zb/%%a %BASE_URL%
)
copy /Y index.json ..\bin_zb\index%1.json

del *.zigbee
del index.json



