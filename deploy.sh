#!/bin/bash

export PATH=$PATH:/home/build/inkbox/kernel/toolchain/armv7l-linux-musleabihf-cross/bin
cd src/
armv7l-linux-musleabihf-g++ touch_emulate.cpp -o touch_emulate 
armv7l-linux-musleabihf-strip touch_emulate
mv touch_emulate ../touch_emulate.bin
cd ../

servername="root@192.168.2.2"
passwd="root"

sshpass -p $passwd ssh $servername "bash -c \"rm /touch_emulate.bin\""
sshpass -p $passwd scp touch_emulate.bin $servername:/
