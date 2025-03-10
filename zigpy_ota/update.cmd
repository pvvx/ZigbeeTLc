echo [] > index.json
for %%a in (../bin/1141-02??-0???3001-*.zigbee) do (
start /wait node scripts/add.js ../bin/%%a
)
copy /Y index.json ..\bin\index%1.json

echo [] > index.json
for %%a in (../bin/1286-0202-10993607-*.zigbee) do (
start /wait node scripts/add.js ../bin/%%a
)
copy /Y index.json ..\bin\th03_tuya2z.json

echo [] > index.json
for %%a in (../bin/1286-0203-10983001-*.zigbee) do (
start /wait node scripts/add.js ../bin/%%a
)
copy /Y index.json ..\bin\zg227z_tuya2z.json

echo [] > index.json
for %%a in (../bin/1141-d3a3-01993001-LKTMZL02*.zigbee) do (
start /wait node scripts/add.js ../bin/%%a
)
copy /Y index.json ..\bin\lktml02_tuya2z.json

echo [] > index.json
for %%a in (../bin/1141-d3a3-01983001-TS0201*.zigbee) do (
start /wait node scripts/add.js ../bin/%%a
)
copy /Y index.json ..\bin\ts0201_tuya2z.json

echo [] > index.json
for %%a in (../bin/1141-d3a3-01993001-TH01*.zigbee) do (
start /wait node scripts/add.js ../bin/%%a
)
copy /Y index.json ..\bin\th01_tuya2z.json

echo [] > index.json
for %%a in (../bin/1141-d3a3-01993001-TH02*.zigbee) do (
start /wait node scripts/add.js ../bin/%%a
)
copy /Y index.json ..\bin\th02_tuya2z.json

echo [] > index.json
for %%a in (../bin/1141-d3a3-01993001-TH03*.zigbee) do (
start /wait node scripts/add.js ../bin/%%a
)
copy /Y index.json ..\bin\th03_tuya2z.json

echo [] > index.json
for %%a in (../bin/1002-d3a3-66993001-ZYZTH02*.zigbee) do (
start /wait node scripts/add.js ../bin/%%a
)
copy /Y index.json ..\bin\zyzth02_tuya2z.json

del *.zigbee
del index.json



