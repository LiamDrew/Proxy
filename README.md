# HTTP_Proxy

An HTTP Proxy with a response cache to facilitate faster HTTP response service.

## Features
- Uses TCP sockets to connect with the desired HTTP server
- Caches recently fetched files to serve them faster in the future without fetching from the server again.

## Installation
- There is no package for this repo. You will have to download or clone it.

## Configuration
- On Linux, you may need to update the Makefile to include the `lnsl` flag
- On MacOS, this should build out of the box

## Usage

To compile:
```bash
make
```

To run:
```bash
./a.out [Port Number]
```
(Specify the port number you want to use to host your proxy.)


## Examples
To see my sample usage of this proxy, you can run my testing script:
```bash
cd clients
chmod +x tests.sh
./tests.sh
```

Numerous tests that rely on locally hosted python servers are included in the testing script, but are commented out.
You are encouraged to configure the existing python servers to run on specific port numbers, and use these tests as a starting point.

I also encourage using Wireshark to observe the operation of the Proxy.

## Limitations
- Only capable of caching HTTP responses, not HTTPS responses.


