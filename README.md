# MP2P

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

## Tests

We are using the [Catch Unit](https://github.com/philsquared/Catch) test framework.

## How to configure .vimrc with Syntastic so that it detects header locations?

In the src/ folder you will find a .syntastic_cpp_config file containing
compile FLAGS. Here is what you have to add in your .vimrc config file to
handle this configuration:

```
let g:syntastic_c_check_header = 1
let g:syntastic_check_on_open = 1
let g:syntastic_c_compiler = 'clang'
let g:syntastic_cpp_config_file = '.syntastic_cpp_config'
```
