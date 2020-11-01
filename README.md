# Callblocker Modem

This is a stripped down version of the callblocker from https://github.com/pamapa/callblocker
It only keeps the analog connection part and is meant to be used as part of a bigger project.

It's used to connect to an analog modem and trigger a URL when a call is received.
Depending on the value returned it will block the call or allow it.

## Hardware
(From the original project)

Supported (tested) analog modems
- USRobotics: USR5637 (56K USB Faxmodem)
- Zoom: Model 3095 (V.92 56K USB Mini External Modem)


## Build

You need gcc installed.
Then run 
```bash
./build.sh
```

## Setup
- attach the USB modem to the Raspberry Pi
- use `dmesg` to find the device name `/dev/<name>`
- run out/callblockerd {deviceName}
  e.g. ./callblockerd '/dev/ttyACM0'
