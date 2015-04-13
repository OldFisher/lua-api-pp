/*
* This file is part of Lua API++ library (https://github.com/OldFisher/lua-api-pp)
* distributed under MIT License (http://opensource.org/licenses/MIT).
* See license.txt for details.
* (c) 2014 OldFisher
*/

#include "luainc.h"

#ifdef LUAPP_HEADER_ONLY
#define LUAPP_HO_INLINE inline
#else	// LUAPP_HEADER_ONLY
#define LUAPP_HO_INLINE
#endif // LUAPP_HEADER_ONLY


#ifndef LUAPP_HEADER_ONLY_FLAG
#include "lua.hpp"
	constexpr lua::Context::InitializeExplicitly lua::Context::initializeExplicitly;
#endif


#include <algorithm>

#if defined(LUAPP_HEADER_ONLY_FLAG) || !defined(LUAPP_HEADER_ONLY)

namespace lua {

	namespace _ {

//### mkcf ##################################################################################################################
		LUAPP_HO_INLINE int LFunctionWrapper(LFunction f, lua_State* s) noexcept
		{
//			if(!f)
//				return lua_error("Attempt to call null pointer as a Lua function");
			try {
				Context S(s);
				return f(S).rvamount;
			} catch(std::exception& e) {
				return luaL_error(s, (std::string("Lua function terminated with an exception: ") + e.what()).c_str());
			} catch(...) {
				return luaL_error(s, "Lua function terminated with an unknown exception");
			}
		}



		LUAPP_HO_INLINE int LFunctionUWrapper(lua_State* s) noexcept
		{
			LFunction f = reinterpret_cast<LFunction>(lua_touserdata(s, lua_upvalueindex(1)));
			if(!f)
				return luaL_error(s, "LFunction wrapper: function pointer upvalue at index 1 is invalid");
			try {
				Context S(s, Context::initializeExplicitly);
				return f(S).rvamount;
			} catch(std::exception& e) {
				return luaL_error(s, (std::string("Lua function terminated with an exception: ") + e.what()).c_str());
			} catch(...) {
				return luaL_error(s, "Lua function terminated with an unknown exception");
			}
		}


//### call and pcall ########################################################################################################

		LUAPP_HO_INLINE void lazyCallUtils::call(lua_State* L, size_t oldtop, int rvAmount) noexcept
		{
			const size_t top = lua_gettop(L);
			const int argnum = top - oldtop - 1;
			lua_call(L, argnum, rvAmount);
		}




		LUAPP_HO_INLINE bool lazyPCallUtils::pcall(lua_State* L, size_t oldtop, int rvAmount) noexcept
		{
			const size_t top = lua_gettop(L);
			const int argnum = top - oldtop - 1;
#if(LUAPP_API_VERSION >= 52)
			return lua_pcall(L, argnum, rvAmount, 0) == LUA_OK;
#else
			return lua_pcall(L, argnum, rvAmount, 0) == 0;
#endif
		}



//### Closures and chunks ###################################################################################################

		LUAPP_HO_INLINE void lazyClosureUtils::makeClosure(lua_State* L, CFunction fn, size_t uvnum) noexcept
		{
			lua_pushcclosure(L, fn, uvnum);
		}


		LUAPP_HO_INLINE void lazyChunk::push(Context& S)
		{
#if(LUAPP_API_VERSION >= 52)
			if(luaL_loadstring(S, ChunkText) != LUA_OK)
#else
			if(luaL_loadstring(S, ChunkText) != 0)
#endif
			{
				std::string errmsg = lua_tostring(S, -1);
				lua_pop(S, 1);
				throw std::runtime_error("Lua: could not load a chunk, the error message is: " + errmsg);
			}
		}


		LUAPP_HO_INLINE void lazyFileChunk::push(Context& S)
		{
#if(LUAPP_API_VERSION >= 52)
			if(luaL_loadfile(S, FileName) != LUA_OK)
#else
			if(luaL_loadfile(S, FileName) != 0)
#endif

			{
				std::string errmsg = lua_tostring(S, -1);
				lua_pop(S, 1);
				throw std::runtime_error("Lua: could not load a chunk, the error message is: " + errmsg);
			}
		}




//### C function wrappers ###################################################################################################

		namespace wrap {

			LUAPP_HO_INLINE LightUserData getRawReserve(lua_State* L)
			{
				return lua_touserdata(L, lua_upvalueindex(1));
			}

		}




//### Length and where ######################################################################################################

#if(LUAPP_API_VERSION >= 52)
		LUAPP_HO_INLINE void lua::_::lazyLen::pushLength(lua_State* L, int idx)
		{
			lua_len(L, idx);
		}


		LUAPP_HO_INLINE void lua::_::lazyLen::push(lua::Context& S)
		{
			pushLength(S, Idx);
		}
#endif


		LUAPP_HO_INLINE void lua::_::lazyWhere::push(lua::Context& S)
		{
			luaL_where(S, 0);
		}




//### Indexing operations ###################################################################################################

		// const indexer

		LUAPP_HO_INLINE void lazyConstIndexerUtils::extractValue(lua_State* L, int tableref) noexcept
		{
			lua_gettable(L, tableref);
		}


		LUAPP_HO_INLINE void lazyConstIndexerUtils::writeValue(lua_State* L, int tableref) noexcept
		{
			lua_settable(L, tableref);
		}

#if(LUAPP_API_VERSION >= 53)
		LUAPP_HO_INLINE void lazyConstIndexerUtils::extractValuei(lua_State* L, int tableref, long long index) noexcept
		{
			lua_geti(L, tableref, index);
		}


		LUAPP_HO_INLINE void lazyConstIndexerUtils::writeValuei(lua_State* L, int tableref, long long index) noexcept
		{
			lua_seti(L, tableref, index);
		}
#endif	// V53+

		// global indexer

		LUAPP_HO_INLINE void lazyGlobalIndexer::push(lua::Context& S)
		{
			lua_getglobal(S, nameref);
		}


		LUAPP_HO_INLINE void lazyGlobalIndexer::writeValue(lua_State* L) noexcept
		{
			lua_setglobal(L, nameref);
		}


		// upvalue indexer

		LUAPP_HO_INLINE lua::Valref lua::_::uvIndexer::operator [] (size_t index) noexcept
		{
			return lua::Valref(S, lua_upvalueindex(index));
		}



		// temporary indexer helper

		LUAPP_HO_INLINE void lazyTempIndexerUtils::extractValue(lua_State* L) noexcept
		{
			lua_gettable(L, -2);
			lua_remove(L, -2);
		}


		LUAPP_HO_INLINE void lazyTempIndexerUtils::writeValue(lua_State* L) noexcept
		{
			lua_settable(L, -3);
			lua_pop(L, 1);
		}

#if(LUAPP_API_VERSION >= 53)
		LUAPP_HO_INLINE void lazyTempIndexerUtils::extractValuei(lua_State* L, long long index) noexcept
		{
			lua_geti(L, -1, index);
			lua_remove(L, -2);
		}


		LUAPP_HO_INLINE void lazyTempIndexerUtils::writeValuei(lua_State* L, long long index) noexcept
		{
			lua_seti(L, -2, index);
			lua_pop(L, 1);
		}
#endif	// V53+

		// metatable accessor

		LUAPP_HO_INLINE void lazyMT::doPush(lua::Context& S, int index) noexcept
		{
			if(!lua_getmetatable(S, index))
				S.push(lua::nil);
		}


		LUAPP_HO_INLINE void lazyMT::doAssign(lua_State* L, int index) noexcept
		{
			lua_setmetatable(L, index);
		}

#if(LUAPP_API_VERSION >= 53)

		// linked value accessor

		LUAPP_HO_INLINE void lazyLinked::doPush(lua::Context& c, int index) noexcept
		{
			if(!lua_isuserdata(c, index))	// Safe to call only on userdata
				c.push(lua::nil);
			else
				lua_getuservalue(c, index);		// Produces nil if nothing was linked
		}


		LUAPP_HO_INLINE void lazyLinked::doAssign(lua_State* L, int index) noexcept
		{
			if(lua_isuserdata(L, index))	// Safe to call only on userdata
				lua_setuservalue(L, index);
			else
				lua_pop(L, 1);
		}
#endif	// V53+

		// raw indexer

		LUAPP_HO_INLINE void lazyRawIndexerUtils::read(lua_State* L, int tIdx)
		{
			lua_rawget(L, tIdx);
		}


		LUAPP_HO_INLINE void lazyRawIndexerUtils::write(lua_State* L, int tIdx)
		{
			lua_rawset(L, tIdx);
		}


		LUAPP_HO_INLINE void lazyRawIndexer<int>::push(lua::Context& S)
		{
			lua_rawgeti(S, tableref, Idx);
		}


		LUAPP_HO_INLINE void lazyRawIndexer<int>::set(lua_State* L, int tIdx, int idx)
		{
			lua_rawseti(L, tIdx, idx);
		}

#if(LUAPP_API_VERSION >= 52)
		LUAPP_HO_INLINE void lazyRawIndexer<lua::LightUserData>::push(lua::Context& S)
		{
			lua_rawgetp(S, tableref, Idx);
		}


		LUAPP_HO_INLINE void lazyRawIndexer<lua::LightUserData>::set(lua_State* L, int tIdx, lua::LightUserData idx)
		{
			lua_rawsetp(L, tIdx, idx);
		}
#endif



		// const ext-upvalue indexer

        LUAPP_HO_INLINE void lazyExtConstUpvalue::doPush(lua_State* s, int closureRef, size_t index)
        {
        	const auto rv = lua_getupvalue(s, closureRef, static_cast<int>(index));
        	if(rv == nullptr)
				throw std::runtime_error("Upvalue external read: wrong index (" + std::to_string(index) + ").");
        }

        LUAPP_HO_INLINE void lazyExtConstUpvalue::doAssign(lua_State* s, int closureRef, size_t index)
        {
        	const auto rv = lua_setupvalue(s, closureRef, static_cast<int>(index));
        	if(rv == nullptr){
				lua_pop(s, 1);
				throw std::runtime_error("Upvalue external write: wrong index (" + std::to_string(index) + ").");
        	}
        }


	}



//### Valref ################################################################################################################

	template<> LUAPP_HO_INLINE bool Valref::cast<bool>() const
	{
		return lua_toboolean(context, index) != 0;
	}



	template<> LUAPP_HO_INLINE bool Valref::to<bool>() const
	{
		return lua_toboolean(context, index) != 0;
	}



	template<> LUAPP_HO_INLINE int Valref::cast<int>() const
	{
#if(LUAPP_API_VERSION >= 52)
		int isnum = 0;
		lua_Number n = lua_tonumberx(context, index, &isnum);
		if(!isnum)
			throw std::runtime_error("Lua: bad cast to number (int)");
#else
		if(!lua_isnumber(context, index))
			throw std::runtime_error("Lua: bad cast to number (int)");
		lua_Number n = lua_tonumber(context, index);
#endif
		return static_cast<int>(n);
	}



	template<> LUAPP_HO_INLINE int Valref::to<int>() const
	{
		return static_cast<int>(lua_tonumber(context, index));
	}



	template<> LUAPP_HO_INLINE unsigned int Valref::cast<unsigned int>() const
	{
#if(LUAPP_API_VERSION >= 52)
		int isnum = 0;
		lua_Number n = lua_tonumberx(context, index, &isnum);
		if(!isnum)
			throw std::runtime_error("Lua: bad cast to number (unsigned int)");
#else
		if(!lua_isnumber(context, index))
			throw std::runtime_error("Lua: bad cast to number (unsigned int)");
		lua_Number n = lua_tonumber(context, index);
#endif
		return static_cast<unsigned int>(n);
	}



	template<> LUAPP_HO_INLINE unsigned int Valref::to<unsigned int>() const
	{
		return static_cast<unsigned int>(lua_tonumber(context, index));
	}



	template<> LUAPP_HO_INLINE long long Valref::cast<long long>() const
	{
#if(LUAPP_API_VERSION >= 52)
		int isnum = 0;
		lua_Number n = lua_tonumberx(context, index, &isnum);
		if(!isnum)
			throw std::runtime_error("Lua: bad cast to number (long long)");
#else
		if(!lua_isnumber(context, index))
			throw std::runtime_error("Lua: bad cast to number (long long)");
		lua_Number n = lua_tonumber(context, index);
#endif
		return static_cast<long long>(n);
	}



	template<> LUAPP_HO_INLINE long long Valref::to<long long>() const
	{
		return static_cast<long long>(lua_tonumber(context, index));
	}



	template<> LUAPP_HO_INLINE unsigned long long Valref::cast<unsigned long long>() const
	{
#if(LUAPP_API_VERSION >= 52)
		int isnum = 0;
		lua_Number n = lua_tonumberx(context, index, &isnum);
		if(!isnum)
			throw std::runtime_error("Lua: bad cast to number (unsigned long long)");
#else
		if(!lua_isnumber(context, index))
			throw std::runtime_error("Lua: bad cast to number (unsigned long long)");
		lua_Number n = lua_tonumber(context, index);
#endif
		return static_cast<unsigned long long>(n);
	}



	template<> LUAPP_HO_INLINE unsigned long long Valref::to<unsigned long long>() const
	{
		return static_cast<unsigned long long>(lua_tonumber(context, index));
	}



	template<> LUAPP_HO_INLINE float Valref::cast<float>() const
	{
#if(LUAPP_API_VERSION >= 52)
		int isnum = 0;
		lua_Number n = lua_tonumberx(context, index, &isnum);
		if(!isnum)
			throw std::runtime_error("Lua: bad cast to number (float)");
#else
		if(!lua_isnumber(context, index))
			throw std::runtime_error("Lua: bad cast to number (float)");
		lua_Number n = lua_tonumber(context, index);
#endif
		return static_cast<float>(n);
	}



	template<> LUAPP_HO_INLINE float Valref::to<float>() const
	{
		return static_cast<float>(lua_tonumber(context, index));
	}



	template<> LUAPP_HO_INLINE double Valref::cast<double>() const
	{
#if(LUAPP_API_VERSION >= 52)
		int isnum = 0;
		lua_Number n = lua_tonumberx(context, index, &isnum);
		if(!isnum)
			throw std::runtime_error("Lua: bad cast to number (double)");
#else
		if(!lua_isnumber(context, index))
			throw std::runtime_error("Lua: bad cast to number (double)");
		lua_Number n = lua_tonumber(context, index);
#endif
		return static_cast<double>(n);
	}



	template<> LUAPP_HO_INLINE double Valref::to<double>() const
	{
		return static_cast<double>(lua_tonumber(context, index));
	}



	template<> LUAPP_HO_INLINE const char* Valref::cast<const char*>() const
	{
		const char* const rv = lua_tostring(context, index);
		if(!rv)
			throw std::runtime_error("Lua: bad cast to C string");
		return rv;
	}



	template<> LUAPP_HO_INLINE const char* Valref::to<const char*>() const
	{
		return lua_tostring(context, index);
	}



	template<> LUAPP_HO_INLINE std::string Valref::cast<std::string>() const
	{
		const char* const rv = lua_tostring(context, index);
		if(!rv)
			throw std::runtime_error("Lua: bad cast to std::string");
		return rv;
	}



	template<> LUAPP_HO_INLINE std::string Valref::to<std::string>() const
	{
		return lua_tostring(context, index);
	}



	template<> LUAPP_HO_INLINE CFunction Valref::cast<CFunction>() const
	{
		CFunction const rv = lua_tocfunction(context, index);
		if(!rv)
			throw std::runtime_error("Lua: bad cast to function");
		return rv;
	}



	template<> LUAPP_HO_INLINE CFunction Valref::to<CFunction>() const
	{
		return lua_tocfunction(context, index);
	}



	template<> bool Valref::is<LightUserData>() const noexcept;

	template<> LUAPP_HO_INLINE LightUserData Valref::cast<LightUserData>() const
	{
		if(!is<LightUserData>())
			throw std::runtime_error("Lua: bad cast to light userdata");
		return lua_touserdata(context, index);
	}



	template<> LUAPP_HO_INLINE LightUserData Valref::to<LightUserData>() const
	{
		return lua_touserdata(context, index);
	}






	template<> LUAPP_HO_INLINE bool Valref::is<Nil> () const noexcept
	{
		return lua_isnoneornil(context, index);
	}

	template<> LUAPP_HO_INLINE bool Valref::is<bool>() const noexcept
	{
		return lua_isboolean(context, index) != 0;
	}

	template<> LUAPP_HO_INLINE bool Valref::is<int>() const noexcept
	{
		return lua_isnumber(context, index) != 0;
	}

	template<> LUAPP_HO_INLINE bool Valref::is<unsigned int>() const noexcept
	{
		return lua_isnumber(context, index) != 0;
	}

	template<> LUAPP_HO_INLINE bool Valref::is<long long>() const noexcept
	{
		return lua_isnumber(context, index) != 0;
	}

	template<> LUAPP_HO_INLINE bool Valref::is<unsigned long long>() const noexcept
	{
		return lua_isnumber(context, index) != 0;
	}

	template<> LUAPP_HO_INLINE bool Valref::is<float>() const noexcept
	{
		return lua_isnumber(context, index) != 0;
	}

	template<> LUAPP_HO_INLINE bool Valref::is<double>() const noexcept
	{
		return lua_isnumber(context, index) != 0;
	}

	template<> LUAPP_HO_INLINE bool Valref::is<const char*>() const noexcept
	{
		return lua_isstring(context, index) != 0;
	}

	template<> LUAPP_HO_INLINE bool Valref::is<std::string>() const noexcept
	{
		return is<const char*>();
	}

	template<> LUAPP_HO_INLINE bool Valref::is<CFunction>() const noexcept
	{
		return lua_iscfunction(context, index) != 0;
	}

	template<> LUAPP_HO_INLINE bool Valref::is<LFunction>() const noexcept
	{
		return lua_isfunction(context, index) != 0;
	}

	template<> LUAPP_HO_INLINE bool Valref::is<LightUserData>() const noexcept
	{
		return lua_islightuserdata(context, index) != 0;
	}

	template<> LUAPP_HO_INLINE bool Valref::is<Table>() const noexcept
	{
		return lua_istable(context, index) != 0;
	}


	template<> LUAPP_HO_INLINE bool Valref::is<Value>() const noexcept
	{
		return true;
	}



	LUAPP_HO_INLINE ValueType Valref::type() const noexcept
	{
		switch(lua_type(context, index))
		{
		case LUA_TNONE:			return ValueType::None;
		case LUA_TNIL:				return ValueType::Nil;
		case LUA_TNUMBER:			return ValueType::Number;
		case LUA_TBOOLEAN:			return ValueType::Boolean;
		case LUA_TSTRING:			return ValueType::String;
		case LUA_TTABLE:			return ValueType::Table;
		case LUA_TFUNCTION:		return is<CFunction>() ?
										ValueType::C_Function :
										ValueType::Function;
		case LUA_TUSERDATA:		return ValueType::UserData;
		case LUA_TTHREAD:			return ValueType::Thread;
		case LUA_TLIGHTUSERDATA:	return ValueType::LightUserdata;
		default:					return ValueType::Any;	// Should never actually happen
		}
	}

	LUAPP_HO_INLINE Valref::operator bool () const {return cast<bool>();}
	LUAPP_HO_INLINE Valref::operator int () const {return cast<int>();}
	LUAPP_HO_INLINE Valref::operator unsigned int() const {return cast<unsigned int>();}
	LUAPP_HO_INLINE Valref::operator long long () const {return cast<long long>();}
	LUAPP_HO_INLINE Valref::operator unsigned long long() const {return cast<unsigned long long>();}
	LUAPP_HO_INLINE Valref::operator float() const {return cast<float>();}
	LUAPP_HO_INLINE Valref::operator double() const {return cast<double>();}
	LUAPP_HO_INLINE Valref::operator const char* () const  {return cast<const char*>();}
	LUAPP_HO_INLINE Valref::operator CFunction () const {return cast<CFunction>();}
	LUAPP_HO_INLINE Valref::operator LightUserData () const {return cast<LightUserData>();}


	LUAPP_HO_INLINE void Valref::replace() noexcept
	{
		lua_replace(context,index);
	}


	LUAPP_HO_INLINE void* Valref::readUserData(const char* classname) const
	{
		if(!isUserData(classname))
			throw std::runtime_error("Lua: cast to user data failed");
		return lua_touserdata(context, index);
	}


	LUAPP_HO_INLINE bool Valref::isUserData(const char* classname) const noexcept
	{
		const bool is_userdata = lua_isuserdata(context, index);
		if(is_userdata)
		{
			const bool got_mt = lua_getmetatable(context, index);
			if(got_mt)
			{
				lua_pushstring(context, classname);
				lua_gettable(context, LUA_REGISTRYINDEX);
				const bool rv = lua_rawequal(context, -1, -2);
				lua_pop(context, 2);
				return rv;
			}
		}
		return false;
	}



#if(LUAPP_API_VERSION < 52)
	LUAPP_HO_INLINE size_t Valref::len() const noexcept
	{
		return lua_objlen(context, index);
	}
#endif	// V51-


	LUAPP_HO_INLINE const char* Valref::readUv(lua_State* s, int closure, size_t index)
	{
		return lua_getupvalue(s, closure, index);
	}


#if(LUAPP_API_VERSION < 52)
	LUAPP_HO_INLINE size_t Valref::retrieveClosureInfo(lua_State* s) noexcept
	{
		lua_Debug ld{};
		lua_getinfo(s, ">u", &ld);
		return ld.nups;
	}
#else // V52+
	LUAPP_HO_INLINE ClosureInfo Valref::retrieveClosureInfo(lua_State* s) noexcept
	{
		lua_Debug ld;
		lua_getinfo(s, ">u", &ld);
		return ClosureInfo{ld.nups, ld.nparams, ld.isvararg != 0};
	}
#endif

//### Context ###############################################################################################################


	LUAPP_HO_INLINE Context::Context(lua_State* l) noexcept:
		L(l), global(*this),
		upvalues(*this),
		args(*this, 1, lua_gettop(l)),
		registry(*this)
	{
	}



#ifdef LUAPP_WATCH_STACK
	LUAPP_HO_INLINE int Context::getCheckedTop() const noexcept
	{
		return returning ? currentStackSize : lua_gettop(L);
	}
#endif // LUAPP_WATCH_STACK

	LUAPP_HO_INLINE size_t Context::getTop() const noexcept
	{
		return lua_gettop(L);
	}



	LUAPP_HO_INLINE void Context::pop(size_t amount) noexcept
	{
#ifdef LUAPP_WATCH_STACK
		currentStackSize -= amount;
#endif // LUAPP_WATCH_STACK
		if(!returning)
			lua_pop(L, amount);
	}



	LUAPP_HO_INLINE size_t Context::duplicate(size_t index) noexcept
	{
		lua_pushvalue(L, index);
		return getTop();
	}




	LUAPP_HO_INLINE void Context::push(const Nil&) noexcept
	{
		lua_pushnil(L);
	}



	LUAPP_HO_INLINE void Context::push(bool val) noexcept
	{
		lua_pushboolean(L, val);
	}



#if(LUAPP_API_VERSION >= 53)
	LUAPP_HO_INLINE void Context::push(long long val) noexcept
	{
		lua_pushinteger(L, val);
	}



	LUAPP_HO_INLINE void Context::push(unsigned long long val) noexcept
	{
		lua_pushinteger(L, val);
	}
#endif



	LUAPP_HO_INLINE void Context::push(double val) noexcept
	{
		lua_pushnumber(L, val);
	}



	LUAPP_HO_INLINE void Context::push(const char* val) noexcept
	{
		lua_pushstring(L, val);
	}



	LUAPP_HO_INLINE void Context::push(CFunction val) noexcept
	{
		lua_pushcfunction(L, val);
	}



	LUAPP_HO_INLINE void Context::push(LightUserData val) noexcept
	{
		lua_pushlightuserdata(L, val);
	}




	LUAPP_HO_INLINE void Context::doCall(size_t oldtop, size_t retnum) noexcept
	{
		lua_call(L, getTop() - oldtop - 1, retnum);
	}



	LUAPP_HO_INLINE void Context::makeClosure(CFunction f, size_t oldtop) noexcept
	{
		lua_pushcclosure(L, f, getTop() - oldtop - 1);
	}



#if(LUAPP_API_VERSION >= 52)
	LUAPP_HO_INLINE int Context::getVersion() const noexcept
	{
		return static_cast<int>(*lua_version(L));
	}
#endif



#if(LUAPP_API_VERSION >= 52)
	LUAPP_HO_INLINE void Context::doArith(_::Arithmetics op) noexcept
	{
		lua_arith(L, static_cast<int>(op));
	}
#endif



	LUAPP_HO_INLINE bool Context::doCompare(int idx1, int idx2, _::Comparison op) noexcept
	{
#if(LUAPP_API_VERSION >= 52)
		return lua_compare(L, idx1, idx2, static_cast<int>(op)) != 0;
#else
		switch(op){
		case _::Comparison::Equal:
			return lua_equal(L, idx1, idx2) != 0;
		case _::Comparison::Less:
			return lua_lessthan(L, idx1, idx2) != 0;
		case _::Comparison::LessEqual:
			return lua_equal(L, idx1, idx2) != 0 || lua_lessthan(L, idx1, idx2) != 0;
		default:
			throw std::runtime_error("Lua comparison: unexpected operation");
		}
#endif
	}



	LUAPP_HO_INLINE void Context::doConcat(size_t oldtop) noexcept
	{
		lua_concat(L, getTop() - oldtop);
	}



	LUAPP_HO_INLINE void* Context::allocateUD(size_t size) noexcept
	{
		return lua_newuserdata(L, size);
	}



	LUAPP_HO_INLINE void Context::setupUD(const char* mtrefname) noexcept
	{
		luaL_getmetatable(L, mtrefname);
		lua_setmetatable(L, -2);
	}



	LUAPP_HO_INLINE void Context::gcCollect() noexcept
	{
		lua_gc(L, LUA_GCCOLLECT, 0);
	}



	LUAPP_HO_INLINE void Context::gcStop() noexcept
	{
		lua_gc(L, LUA_GCSTOP, 0);
	}



	LUAPP_HO_INLINE void Context::gcResume() noexcept
	{
		lua_gc(L, LUA_GCRESTART, 0);
	}


#if(LUAPP_API_VERSION >= 52)
	LUAPP_HO_INLINE bool Context::gcIsRunning() const noexcept
	{
		return lua_gc(L, LUA_GCISRUNNING, 0);
	}
#endif


	LUAPP_HO_INLINE size_t Context::queryMemoryTotal() const noexcept
	{
		return (lua_gc(L, LUA_GCCOUNT, 0) << 10) + lua_gc(L, LUA_GCCOUNTB, 0);
	}



	LUAPP_HO_INLINE Retval Context::doerror() const
	{
		return Retval(lua_error(L));
	}


	LUAPP_HO_INLINE void Context::remove(size_t index)
	{
		if(index > getTop() || index <= args.size())
			throw std::runtime_error("Lua context: attempt to remove invalid stack index (pseudo, non-existent or argument)");
		lua_remove(L, index);
	}



	LUAPP_HO_INLINE _::Lazy<_::lazyConstIndexer<const char*>> Context::Registry::operator [] (const char* index) noexcept
	{
		return _::Lazy<_::lazyConstIndexer<const char*>>(context, LUA_REGISTRYINDEX, index);
	}


	LUAPP_HO_INLINE _::Lazy<_::lazyRawIndexer<int>> Context::Registry::operator [] (RegistryKey index) noexcept
	{
		return _::Lazy<_::lazyRawIndexer<int>>(context, LUA_REGISTRYINDEX, index.value);
	}


	LUAPP_HO_INLINE RegistryKey Context::makeReference()
	{
		return RegistryKey(luaL_ref(L, LUA_REGISTRYINDEX));
	}



//### Table #################################################################################################################

	// Table utils

	LUAPP_HO_INLINE int _::TableUtils::makeNew(lua_State* s, int arrSize, int recSize) noexcept
	{
		lua_createtable(s, arrSize, recSize);
		return lua_gettop(s);
	}


	LUAPP_HO_INLINE void _::TableUtils::setValue(lua_State* S, int tableNum) noexcept
	{
		lua_rawset(S, tableNum);
	}



	LUAPP_HO_INLINE void _::TableUtils::setValue(lua_State* S, int tableNum, int key) noexcept
	{
		lua_rawseti(S, tableNum, key);
	}


	// Table

	LUAPP_HO_INLINE Table::Table(Context& S, size_t arrSize, size_t recSize) noexcept:
		Anchor(S, _::TableUtils::makeNew(S, arrSize, recSize)), raw(Anchor)
	{
	}



	LUAPP_HO_INLINE void Table::beginIteration(lua::Context& S)
	{
		S.masspush(nil, nil);	// Second nil is popped by first call to nextIteration
	}



	LUAPP_HO_INLINE bool Table::nextIteration(const lua::Value& v)
	{
		v.context.pop();
		return lua_next(v.context, v.index) != 0;
	}


//### State #################################################################################################################

	LUAPP_HO_INLINE State::State():
		state(luaL_newstate())
	{
		if(!state)
			throw std::runtime_error("Lua state cannot be created");
		lua_gc(state, LUA_GCSTOP, 0);
		luaL_openlibs(state);
		lua_gc(state, LUA_GCRESTART, 0);
	}



	LUAPP_HO_INLINE State::State(void* (customAllocatorFunction) (void* /*ud*/, void* ptr, size_t oldSize, size_t newSize), void* ud):
		state(lua_newstate(customAllocatorFunction, ud))
	{
		if(!state)
			throw std::runtime_error("Lua state cannot be created");
		lua_gc(state, LUA_GCSTOP, 0);
		luaL_openlibs(state);
		lua_gc(state, LUA_GCRESTART, 0);
	}



	LUAPP_HO_INLINE State::~State() noexcept
	{
		if(state)
			lua_close(state);
	}



	namespace _ {
		static const char* const strangeError = "Error message is not a string";
	}

	LUAPP_HO_INLINE void State::runFile(const char *filename)
	{
		const auto oldtop = lua_gettop(state);
		const bool success = luaL_dofile(state, filename) == 0;
		if(success){
			lua_settop(state, oldtop);
		} else {
			const auto newtop = lua_gettop(state);
			const std::string msg(lua_isstring(state, newtop) ? lua_tostring(state, newtop) : _::strangeError);
			lua_settop(state, oldtop);
			throw std::runtime_error(msg);
		}
	}



	LUAPP_HO_INLINE void State::runString(const char *expression)
	{
		const auto oldtop = lua_gettop(state);
		const bool success = luaL_dostring(state, expression) == 0;
		if(success){
			lua_settop(state, oldtop);
		} else {
			const auto newtop = lua_gettop(state);
			const std::string msg(lua_isstring(state, newtop) ? lua_tostring(state, newtop) : _::strangeError);
			lua_settop(state, oldtop);
			throw std::runtime_error(msg);
		}
	}



	LUAPP_HO_INLINE void State::call(CFunction f)
	{
		const auto oldtop = lua_gettop(state);
		lua_pushcfunction(state, f);
		const bool success = lua_pcall(state, 0, 0, 0) == 0;
		if(success){
			lua_settop(state, oldtop);
		} else {
			const auto newtop = lua_gettop(state);
			const std::string msg(lua_isstring(state, newtop) ? lua_tostring(state, newtop) : _::strangeError);
			lua_settop(state, oldtop);
			throw std::runtime_error(msg);
		}
	}

}

#endif	// defined(LUAPP_HEADER_ONLY_FLAG) || !defined(LUAPP_HEADER_ONLY)

#undef LUAPP_HO_INLINE

