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
