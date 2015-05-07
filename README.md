# MP2P

[![TeamCity Build Status](https://img.shields.io/teamcity/http/42portal.com/teamcity/s/Mp2p_Build.svg?style=flat?label=TeamCity)](https://42portal.com/teamcity/viewType.html?buildTypeId=Mp2p_Build)
[![Join the chat at https://gitter.im/Dubrzr/MP2P](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/Dubrzr/MP2P?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

File transfer application using a custom protocol.

# Details

This project is divided into for different parts:

* The 'Utils' library
* The 'Storage' application
* The 'Master' application
* The 'Client' application

The library implements functions for the MP2P protocol. It is using boost:asio.

All the apps are using the 'utils' library for network communications and file
manipulations.

The project uses Couchbase as a Master-Master replication database system.

## How to?

### Config files

We provide some *.conf.example files in the config folder, you can configure
them to set up your own settings.

### The Master application

```
mkdir -p build && cd build; cmake ..; make master-release -j;
```

### The Storage application

```
mkdir -p build && cd build; cmake ..; make storage-release -j;
```

### The Client application

```
mkdir -p build && cd build; cmake ..; make client-release -j;
```

## Requirements

* C++ >= 14
* Boost >= 1.55
* libconfig >= 1.4.9
* OpenSSL >= 1.0
* libcouchbase >= 2.4.8

## Details

* The Master application is there to make a connection between all the clients
and the storage. It will be making constant database updates/checks in order to
ensure the integrity of the transfers.

* The Storage application is there to listen for packets
and send/receive files.

It listens both for ipv4 & ipv6 connections
(see http://stackoverflow.com/a/1618259).


## Tests

We are using the [Catch Unit](https://github.com/philsquared/Catch)
test framework.

## How to configure .vimrc with Syntastic so that it detects header locations?

In the src/ folder you will find a .syntastic_cpp_config file containing
compile FLAGS. Here is what you have to add in your .vimrc config file to
handle this configuration:

```
let g:syntastic_check_on_open = 1
let g:syntastic_cpp_check_header = 1
let g:syntastic_cpp_compiler = 'clang'
let g:syntastic_cpp_config_file = '.syntastic_cpp_config'
```
