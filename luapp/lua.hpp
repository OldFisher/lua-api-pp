/*
* This file is part of Lua API++ library (https://github.com/OldFisher/lua-api-pp)
* distributed under MIT License (http://opensource.org/licenses/MIT).
* See license.txt for details.
* (c) 2014 OldFisher
*/

#ifndef LUA_HPP_INCLUDED
#define LUA_HPP_INCLUDED

#include <stdexcept>
#include <new>
#include <iterator>
#include <type_traits>
#include <utility>
#include <functional>
#include <algorithm>
#include <string>


#ifdef LUAPP_COMPATIBILITY_V51
#define LUAPP_API_VERSION 51
#endif

#ifdef LUAPP_COMPATIBILITY_V52
#define LUAPP_API_VERSION 52
#endif

#ifdef LUAPP_COMPATIBILITY_V53
#define LUAPP_API_VERSION 53
#endif

#ifndef LUAPP_API_VERSION
#define LUAPP_API_VERSION 53
#endif



//! @file
//! @brief Main include file for Lua API++ library (no other files need to be included).

#ifdef LUAPP_NONDISCARDABLE_ALL
#define LUAPP_NONDISCARDABLE_INDEX
#define LUAPP_NONDISCARDABLE_ARITHMETICS
#define LUAPP_NONDISCARDABLE_CONCAT
#endif	// LUAPP_NONDISCARDABLE_ALL


#include "lua_basetypes.hxx"
#include "lua_lazy.hxx"
#include "lua_indexer.hxx"
#include "lua_call.hxx"
#include "lua_closure.hxx"
#include "lua_operations.hxx"
#include "lua_valueset.hxx"
#include "lua_value.hxx"
#include "lua_table.hxx"


//! Every thing in Lua API++ library is contained inside this namespace.
namespace lua {

	//! Pointer to Lua API++ compatible function.
	using LFunction = Retval (*)(Context&);

	//! @cond
	namespace _ {
		int LFunctionWrapper(LFunction f, lua_State* s) noexcept;
	}
	//! @endcond

	//! @brief Wrapper for @ref LFunction that creates a proper Lua-compatible C function.
	//! @details <code>template\<@ref lua::LFunction "LFunction" F\> mkcf</code>\n
	//! This template converts @ref lua::LFunction "Lua API++ compatible function" to a proper Lua-compatible C function (mkcf is short for "make C function").
	//! The wrapper creates @ref lua::Context "Context" object and passes it to wrapped function, then converts returned @ref lua::Retval "Retval" to number of returned values.
	//! All exceptions that come from wrapped function are intercepted and converted to Lua runtime errors.
	//! Example: @code{.cpp}
	//! Retval someFunction(Context& context) {return context.ret();}
	//! CFunction cf = mkcf<someFunction>;
	//! @endcode
	//! Note the lack of round parentheses when using mkcf template.
	//! @note Unlike @ref Context::closure "closure" function, mkcf creates individual wrapper for each LFunctions. It doesn't reserve first upvalue for internal use.
	template<LFunction F>
	int mkcf(lua_State* l)
	{
		return _::LFunctionWrapper(F, l);
	}

}

#include "lua_context.hxx"
#include "lua_wrap.hxx"
#include "lua_impl.hxx"
#include "lua_state.hxx"


//! @def LUAPP_USERDATA(type, class_name)
//! @brief Userdata type to name string binder.
//! @param type Userdata type being registered. Must be copyable or moveable.
//! @param class_name String identifier.
//! @details Use this macro to register some type as a userdata type. After registration this type will be recognised as userdata and can be
//! implicitly converted to Lua value and explicitly converted to from Lua value. The storage for the object is allocated by Lua.
//! @note After registering userdata type with this macro, do not forget to assign its metatable during environment setup. For types that require destruction be sure to set <code>__gc</code> metamethod.
//! @note Use this macro outside of namespaces, functions, classes etc.
#define LUAPP_USERDATA(type, class_name) namespace lua { template <> struct UserData<type> {typedef void enabled; static constexpr const char* const classname = class_name;}; }


//! @def LUAPP_ARG_CONVERT(src_type, body)
//! @brief Argument conversion binder.
//! @param src_type Native type of function argument. Must not be supported native type or userdata.
//! @param body The body of conversion function with curved braces.
//! @details This macro is used to define a conversion routine to transform Lua value into native value that will be used as an argument to a wrapped function
//! passed to @ref lua::Context::wrap "Context::wrap" function.\n
//! Provide the type for the argument to be converted to from Lua value and the body for function receiving <code>const Valref& val</code> parameter and returning <code>src_type</code>. Example:
//! @code{.cpp}
//! void arbitraryFunction(const std::vector<char>& x, int y);
//! LUAPP_ARG_CONVERT(std::vector<char>, {const char* arg = val; return std::vector<char>(arg, arg + strlen(arg));} )
//! @endcode
//! @note Use this macro outside of namespaces, functions, classes etc.
//! @see lua::Context::wrap
#define LUAPP_ARG_CONVERT(src_type, body) namespace lua { namespace _ { namespace wrap { template<> inline src_type argCvt<src_type>(const ::lua::Valref& val) body }}}



//! @def LUAPP_RV_CONVERT(rv_type, body)
//! @brief Return value conversion binder.
//! @param rv_type Native type of function argument. Must not be supported native type or userdata.
//! @param body The body of conversion function with curved braces.
//! @details This macro is used to define a conversion routine to transform native value returned by arbitrary C++ function into a Lua value that will be returned by wrapper function created by
//! @ref lua::Context::wrap "Context::wrap" function.\n
//! Provide the type for the return value to be converted to from Lua value and the body for function receiving arguments <code>(rv_type&& val, lua::Context& context</code> parameter. Example:
//! @code{.cpp}
//! std::vector<char> otherArbitraryFunction();
//! LUAPP_RV_CONVERT(std::vector<char>, {return context.ret(val.data());}
//! @endcode
//! @note Use this macro outside of namespaces, functions, classes etc.
//! @see lua::Context::wrap
#define LUAPP_RV_CONVERT(rv_type, body) namespace lua { namespace _ { namespace wrap { template<> inline lua::Retval rvCvt<rv_type>(rv_type val, lua::Context& context) body }}}


#ifdef LUAPP_HEADER_ONLY
#define LUAPP_HEADER_ONLY_FLAG
#include "impl.cpp"
#undef LUAPP_HEADER_ONLY_FLAG
#endif


#endif // LUA_HPP_INCLUDED
