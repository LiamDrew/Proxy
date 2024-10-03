# HTTP_Proxy

## Table of Contents
1. [Project Overview](#project-overview)
2. [Features](#features)
3. [Installation](#installation)
4. [Configuration](#configuration)
5. [Usage](#usage)
6. [Examples](#examples)
7. [Limitations](#limitations)


## Project Overview
- Implement an HTTP Proxy with a response cache to facilitate faster HTTP response service.

## Features
- Key features of the proxy server.
- Supported HTTP methods and functionalities.

## Installation
- There is no package for this repo. You will have to download or clone it.

## Configuration
- If on Linux, you may need to update the Makefile to include the `lnsl` flag
- On MacOS, this should build out of the box

## Usage
- `make`
- Run `./a.out [Port Number]`
- Port number is the port you want to host your proxy on.

## Examples
- See testing
- Sample configuration files.

## Limitations
- Only capable of caching HTTP responses, not HTTPS responses.


