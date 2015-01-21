# Lua API++ library
Lua API++ library is a C++ wrapper for [Lua](http://www.lua.org/) API.
The goal of this library is to let you operate Lua using simple expressive syntax.

##Current status
Lua 5.3 support is added.

##Most noticeable features provided by the library:
* automatic stack management;
* natural expressions (indexing, calls, arithmetics, concatenation);
* full support of multi-value returns;
* userdata support;
* easy table creation and handling;
* automatic wrapping of functions;
* support for Lua 5.1 (compatible with LuaJIT), 5.2 and 5.3.

##Information
The library comes fully [documented](http://oldfisher.github.io/index.html). Dive head-in with [motivational example](http://oldfisher.github.io/motivational_example.html),
get acquainted with [basic concepts](http://oldfisher.github.io/basic_concepts.html) or see what's new in the [changelog](http://oldfisher.github.io/changelog.html).

##Most noticeable **unsupported** features:
* debug;
* coroutines;
* string buffers.

The library requires C++11 compatible compiler. It is distributed under terms of MIT license.
