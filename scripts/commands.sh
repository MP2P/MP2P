#!/bin/bash

#Write all the commands you want to execute in the command_list file, or alternatively,
#provide a file as first argument.
#2nd argument: whatever you want, this is just to specify that your commands should be done on every servers at the same time.

IP0="192.168.1.11"
IP1="192.168.1.12"
IP2="192.168.1.13"

PORT0="22"
PORT1="22"
PORT2="22"

read -s -p "Please enter your MP2P password:" PWD
echo
#if [[ $(whoami) != "rusty" ]]; then
  IP0="rpi.juliendubiel.net"
  IP1="rpi.juliendubiel.net"
  IP2="rpi.juliendubiel.net"
  PORT0="4201"
  PORT1="4202"
  PORT2="4203"
#fi

if [ "$#" -eq 2 ] && [ -f "$1" ]; then
  echo Trying to connect to Raspberry Py 0...
  sshpass -p $PWD ssh root@$IP0 -p $PORT0 -T 'sh -s' < $1 &
  echo
  echo Trying to connect to Raspberry Py 1...
  sshpass -p $PWD ssh root@$IP1 -p $PORT1 -T 'sh -s' < $1 &
  echo
  echo Trying to connect to Raspberry Py 2...
  sshpass -p $PWD ssh root@$IP2 -p $PORT2 -T 'sh -s' < $1 &
else
  if [ "$#" -eq 1 ]; then
    if [ -f "$1" ]; then
      echo Trying to connect to Raspberry Py 0...
      sshpass -p $PWD ssh root@$IP0 -p $PORT0 -T 'sh -s' < $1
      echo
      echo Trying to connect to Raspberry Py 1...
      sshpass -p $PWD ssh root@$IP1 -p $PORT1 -T 'sh -s' < $1
      echo
      echo Trying to connect to Raspberry Py 2...
      sshpass -p $PWD ssh root@$IP2 -p $PORT2 -T 'sh -s' < $1
    else
      echo Trying to connect to Raspberry Py 0...
      sshpass -p $PWD ssh root@$IP0 -p $PORT0 -T 'sh -s' < $( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )/command_list &
      echo
      echo Trying to connect to Raspberry Py 1...
      sshpass -p $PWD ssh root@$IP1 -p $PORT1 -T 'sh -s' < $( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )/command_list &
      echo
      echo Trying to connect to Raspberry Py 2...
      sshpass -p $PWD ssh root@$IP2 -p $PORT2 -T 'sh -s' < $( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )/command_list &
    fi
  else
    echo Trying to connect to Raspberry Py 0...
    sshpass -p $PWD ssh root@$IP0 -p $PORT0 -T 'sh -s' < $( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )/command_list
    echo
    echo Trying to connect to Raspberry Py 1...
    sshpass -p $PWD ssh root@$IP1 -p $PORT1 -T 'sh -s' < $( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )/command_list
    echo
    echo Trying to connect to Raspberry Py 2...
    sshpass -p $PWD ssh root@$IP2 -p $PORT2 -T 'sh -s' < $( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )/command_list
  fi
fi
echo DONE
