# Lua API++ library
Lua API++ library is a C++ wrapper for [Lua](http://www.lua.org/) API.
The goal of this library is to let you operate Lua using simple expressive syntax.

##Most noticeable features provided by the library:
* automatic stack management;
* natural expressions (indexing, calls, arithmetics, concatenation);
* full support of multi-value returns;
* userdata support;
* easy table creation and handling;
* automatic wrapping of functions;
* support for Lua 5.1 (compatible with LuaJIT) and 5.2, support for 5.3 is planned.

##Most noticeable **unsupported** features:
* debug;
* coroutines;
* string buffers.

The library requires C++11 compatible compiler. It is distributed under terms of MIT license.
