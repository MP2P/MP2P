#!/bin/bash

#This script will re-compile the 'storage-release' and re-run it on all the RPis.
#It must be run when there is a push on the 'master' branch.

#FIXME: Fill in with the RPi's root password.
PWD=

IP0="rpi.juliendubiel.net"
IP1="rpi.juliendubiel.net"
IP2="rpi.juliendubiel.net"

PORT0="4201"
PORT1="4202"
PORT2="4203"

CMDS='cd ../home/mp2p/MP2P && git pull --rebase && mkdir build;
cd build && cmake ../ && make storage-release && (killall storage-release;
nohup ./storage-release &>>output &)'

echo Trying to connect to Raspberry Py 0...
sshpass -p $PWD ssh root@$IP0 -p $PORT0 $CMDS &
echo
echo Trying to connect to Raspberry Py 1...
sshpass -p $PWD ssh root@$IP1 -p $PORT1 $CMDS &
echo
echo Trying to connect to Raspberry Py 2...
sshpass -p $PWD ssh root@$IP2 -p $PORT2 $CMDS
echo DONE
