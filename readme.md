# Lua API++ library
Lua API++ library is a C++ wrapper for [Lua](http://www.lua.org/) API.
The goal of this library is to let you operate Lua using simple expressive syntax.

##Current status
With the final release of Lua 5.3 the work on compatibility profile has started.
The 5.3 profile will be added in the next release and it will be made the default profile.

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
