/*
* This file is part of Lua API++ library (https://github.com/OldFisher/lua-api-pp)
* distributed under MIT License (http://opensource.org/licenses/MIT).
* See license.txt for details.
* (c) 2014 OldFisher
*/

//! @file
//! @brief Include file for Lua API.
//! @details Used in Lua API++ implementation.
//! Configure it as necessary for specific Lua installations.
//! This file can also be used for direct interactions with Lua API.

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
