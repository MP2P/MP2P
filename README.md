# MP2P 

[![TeamCity Build Status](https://img.shields.io/teamcity/http/42portal.com/teamcity/s/Mp2p_Build.svg?style=flat?label=TeamCity)](https://42portal.com/teamcity/viewType.html?buildTypeId=Mp2p_Build)
[![Join the chat at https://gitter.im/Dubrzr/MP2P](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/Dubrzr/MP2P?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

Open source Cloud system & Protocol

# Details

This project is divided into three different parts:

* The 'Utils' library
* The 'Server' application
* The 'Client' application

The library implements functions for the MP2P protocol. It is using boost:asio.

Both apps are using the 'utils' library for network communications and file
manipulations.

The project uses Couchbase as a Master-Master replication database system.

## How to?

### The Server application

```
cd src/server; mkdir build && cd build; cmake ..; make;
```

### The Client application

```
cd src/client; mkdir build && cd build; cmake ..; make;
```

## Requirements

* C++ >= 14
* Boost >= 1.55
* libconfig >= 1.4.9

## Details

The Server application includes the Master and also the Storage application.

It listens both for ipv4 & ipv6 connections
(see http://stackoverflow.com/a/1618259).


## Tests

We ~~are~~ will be using the [Catch Unit](https://github.com/philsquared/Catch)
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
