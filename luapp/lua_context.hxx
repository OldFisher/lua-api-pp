/*
* This file is part of Lua API++ library (https://github.com/OldFisher/lua-api-pp)
* distributed under MIT License (http://opensource.org/licenses/MIT).
* See license.txt for details.
* (c) 2014 OldFisher
*/

#ifndef LUA_CONTEXT_HXX_INCLUDED
#define LUA_CONTEXT_HXX_INCLUDED




namespace lua {


	//! @cond
	namespace _ {

		int LFunctionUWrapper(lua_State*) noexcept;

		namespace wrap {

			template<typename, typename ...>
			Retval call(Context&);

			template<typename, typename, typename ...>
			Retval memberCall(Context&);

			template<typename, typename ...>
			Retval callv(Context&);

			template<typename, typename, typename ...>
			Retval memberCallv(Context&);

			//! Envelope for wrapping member function pointers as raw userdata
			template<typename T>
			struct Envelope {

				T data;

				template<typename S>
				Envelope(S&& src):
					data(std::forward<S>(src))
				{
				}
			};
		}


	}
	//! @endcond

	class State;

	//! @brief Access point to Lua context.
	//! @details This object is passed to @ref lua::LFunction "compatible functions". It gives access to function's arguments,
	//! global variables and so on. Context is used to access general Lua functions such as garbage collector or memory monitoring,
	//! and to return values or report errors.
	class Context final {

		// Friendship is magic!
		friend class ::lua::Valref;
		friend class ::lua::_::tValue;

		friend bool ::lua::Valref::operator == (const ::lua::Valref&) const noexcept;
		friend bool ::lua::Valref::operator != (const ::lua::Valref&) const noexcept;
		friend bool ::lua::Valref::operator <  (const ::lua::Valref&) const noexcept;
		friend bool ::lua::Valref::operator >  (const ::lua::Valref&) const noexcept;
		friend bool ::lua::Valref::operator <= (const ::lua::Valref&) const noexcept;
		friend bool ::lua::Valref::operator >= (const ::lua::Valref&) const noexcept;

		// Lazy and its policies and their utils
		template<typename> friend class ::lua::_::Lazy;
		friend class ::lua::_::lazyPolicyNondiscardable;
		template<typename> friend class ::lua::_::lazyImmediateValue;
		template<typename...> friend class ::lua::_::lazySeries;
		template<typename> friend class ::lua::_::lazyConstIndexer;
		friend class ::lua::_::lazyGlobalIndexer;
		template<typename, typename> friend class ::lua::_::lazyTempIndexer;
		template<typename> friend class ::lua::_::lazyRawIndexer;
		friend class ::lua::_::lazyExtConstUpvalue;
		template<typename> friend class ::lua::_::lazyExtTempUpvalue;
		template<typename, typename ...> friend class ::lua::_::lazyCall;
		template<typename, typename ...> friend class ::lua::_::lazyPCall;
#if(LUAPP_API_VERSION >= 52)
		template<typename, typename, lua::_::Arithmetics> friend class ::lua::_::lazyArithmetics;
		template<typename, lua::_::Arithmetics> friend class ::lua::_::lazyArithmeticsUnary;
#endif	// V52+
		template<typename, typename> friend class ::lua::_::lazyConcat;
		friend class ::lua::_::lazyConcatSelector;
#if(LUAPP_API_VERSION >= 52)
		template<typename> friend class ::lua::_::lazyLenTemp;
#endif	// V52+
		friend class ::lua::_::lazyMT;
		template<typename> friend class ::lua::_::lazyMtTemp;
#if(LUAPP_API_VERSION >= 53)
		friend class ::lua::_::lazyLinked;
		friend class ::lua::_::lazyConstIntIndexer;
		template<typename> friend class ::lua::_::lazyLinkedTemp;
		template<typename> friend class ::lua::_::lazyTempIntIndexer;
#endif	// V53+
		template<typename ...> friend class ::lua::_::lazyTableArray;
		template<typename ...> friend class ::lua::_::lazyTableRecords;

		friend class ::lua::Valset;
		friend class ::lua::Value;
		friend class ::lua::Table;

		friend class ::lua::State;

		friend int ::lua::_::LFunctionWrapper(::lua::LFunction, lua_State*) noexcept;


#ifndef DOXYGEN_ONLY
		class Registry {
			friend class ::lua::Context;

			explicit Registry(Context& c):
				context(c)
			{
			}

		public:

			template<typename ValueType>
			RegistryKey store(ValueType&& value)
			{
				context.push(std::forward<ValueType>(value));
				return context.makeReference();
			}


			_::Lazy<_::lazyConstIndexer<const char*>> operator [] (const char* index) noexcept;

			_::Lazy<_::lazyConstIndexer<const char*>> operator [] (const std::string& index) noexcept
			{
				return (*this)[index.c_str()];
			}

			template<size_t N>
			_::Lazy<_::lazyConstIndexer<const char*>> operator [] (const char (&index)[N]) noexcept
			{
				return (*this)[static_cast<const char*>(index)];
			}

			_::Lazy<_::lazyRawIndexer<int>> operator [] (RegistryKey index) noexcept;

		private:
			// data
			Context& context;
		};
#endif // DOXYGEN_ONLY


		Context(lua_State *l) noexcept;

	public:

#ifdef DOXYGEN_ONLY
	//! @brief Type of @ref lua::Context::registry "registry accessor".
	//! @details This class is private. You cannot create Registry objects.
	class Registry{
		public:

			//! @name Userdata metatables
			//! @{

			//! @brief Access to metatable associated with type description string.
			//! @param index Type description string as defined in @ref LUAPP_USERDATA.
			//! @note Using @ref lua::Context::mt "Context::mt" function is a better way to manipulate userdata metatables.
			//! @see LUAPP_USERDATA
			Temporary operator [] (const char* index) noexcept;

			//! @brief Access to metatable associated with type description string.
			//! @param index Type description string as defined in @ref LUAPP_USERDATA.
			//! @note Using @ref lua::Context::mt "Context::mt" function is a better way to manipulate userdata metatables.
			//! @see LUAPP_USERDATA
			Temporary operator [] (std::string& index) noexcept;
			//! @}


			//! @name References
			//! @{

			//! @brief Store an object into registry.
			//! @return Index to access stored object via @ref lua::Context::Registry::operator[](RegistryKey) "indexation operator".
			RegistryKey store(Valobj val);

			//! @brief Access to stored objects.
			//! @param index An index returned by @ref lua::Context::Registry::store "store" function.
			Temporary operator [] (RegistryKey index) noexcept;
			//! @}
	};
#endif // DOXYGEN_ONLY

		//! @brief Type for @ref lua::Context::initializeExplicitly "initializeExplicitly" constant.
		struct InitializeExplicitly{};

		//! @brief Indicator constant used in explicit context creation.
		//! @see lua::Context::Context(lua_State*, const InitializeExplicitly&);
		static constexpr InitializeExplicitly initializeExplicitly{};

		//! @name Life cycle
		//! @{

		//! @brief Explicit construction.
		//! @details Sometimes it is necessary to use Lua API++ features
		//! outside of @ref lua::LFunction "compatible function". You may construct
		//! a Context object explicitly using this constructor as follows:
		//! @code{.cpp}
		//! Context c(luaStatePtr, Context::initializeExplicitly);
		//! @endcode
		//! @warning This operation is considered unsafe. Be careful with stack manipulations and do not create duplicates of Context object.
		Context(lua_State* s, const InitializeExplicitly&):
			Context(s)
		{
		}

		//! @brief Copying is prohibited.
		Context(const Context&) = delete;

		//! @brief Assignment to Context is prohibited.
		Context& operator = (const Context&) = delete;
		//! @}

		//! @name Returning values
		//! @{

		//! @brief Return arbitrary number of values.
		//! @details Pass values to be returned from the function to this function in <code><b>return</b></code> operator.
		//! @note After calling this function, automatic stack management stops functioning in order to preserve return values.
		//! @warning Use this function only in <code><b>return</b></code> operator!
		template<typename ... ValueTypes>
		Retval ret(ValueTypes&& ... values)
		{
			const size_t oldtop = getTop();
#ifdef LUAPP_WATCH_STACK
			currentStackSize = oldtop;
#endif // LUAPP_WATCH_STACK
			try {
				masspush(std::forward<ValueTypes>(values)...);
			} catch(std::exception&) {
				pop(getTop() - oldtop);
				throw;
			}
			returning = true;
			return Retval(getTop() - oldtop);
		}

		//! @cond
		//! Return nothing.
		// No need to document this function, because it's simply a shortcut for multi-arg one.
		Retval ret() noexcept
		{
#ifdef LUAPP_WATCH_STACK
			currentStackSize = getTop();
#endif // LUAPP_WATCH_STACK
			returning = true;
			return Retval(0);
		}
		//! @endcond

		//! @brief Return a single value set.
		//! @details Special case: all values that reside on stack after the set are dropped,
		//! and the values belonging to the set are used directly as multiple return values, without copying.
		//! @note After calling this function, automatic stack management stops functioning in order to preserve return values.
		//! @warning Use this function only in <code><b>return</b></code> operator!
		Retval ret(const Valset& vs) noexcept
		{
#ifdef LUAPP_WATCH_STACK
			currentStackSize = getTop();
#endif // LUAPP_WATCH_STACK
			pop(getTop() + 1 - (vs.size() + vs.Idx));
			returning = true;
			return Retval(vs.size());
		}

		//! @}

		//! @name Error handling
		//! @{

#ifdef DOXYGEN_ONLY
		//! @brief Create a value that describes current call context.
		Temporary where();

		//! @brief Report an error to Lua. This function never returns.
		//! @details Recommended use: @code{.cpp}return context.error(msg); @endcode
		Retval error(Valobj msg);

		//! @overload
		//! @brief Report error to Lua. This function never returns.
		//! @details Recommended use: @code{.cpp}return context.error(); @endcode
		//! Default message will be the result of @ref lua::Context::where "where" function.
		Retval error();
#else	// Not DOXYGEN_ONLY

		_::Lazy<_::lazyWhere> where()
		{
			return _::Lazy<_::lazyWhere>(const_cast<Context&>(*this));
		}

		template<typename MsgType> Retval error(MsgType&& msg)
		{
			ipush(std::forward<MsgType>(msg));
			return doerror();
		}

		Retval error()
		{
			push(where());
			return doerror();
		}
#endif	// DOXYGEN_ONLY
		//! @}

		//! @name Memory control
		//! @{

		//! @brief Perform full garbage collection cycle.
		void gcCollect() noexcept;

		//! @brief Stop automatic garbage collection.
		void gcStop() noexcept;

		//! @brief Resume automatic garbage collection.
		void gcResume() noexcept;

#ifdef DOXYGEN_ONLY
		//! @brief Check if automatic garbaghe collection is running @lv52.
		bool gcIsRunning() const noexcept;
#else	// Not Doxygen
#if(LUAPP_API_VERSION >= 52)
		bool gcIsRunning() const noexcept;
#endif	// V52+
#endif // DOXYGEN_ONLY

		//! @brief Query allocated memory amount.
		//! @return Number of bytes allocated by Lua.
		size_t queryMemoryTotal() const noexcept;
		//! @}

		//! @name Function handling
		//! @{

#ifdef DOXYGEN_ONLY
		//! @brief Create a closure from @ref lua::CFunction "C function".
		template<typename ... UpvalueTypes> Temporary closure (CFunction fn, UpvalueTypes&& ... upvalues_) noexcept;

		//! @brief Create a closure from @ref LFunction.
		//! @details All closures created by this function share common wrapper.
		//! @warning The wrapper is using first upvalue (index 1) internally. Provided upvalues (if any) start at index 2.
		//! This holds true for automatically promoted LFunctions too.
		template<typename ... UpvalueTypes> Temporary closure (CFunction fn, UpvalueTypes&& ... upvalues_) noexcept;

		//! @brief Create a wrapped Lua-compatible function from generic C/C++ function.
		//! @details This function will create a Lua function wrapper that converts Lua arguments into native values,
		//! calls the C function with those arguments and converts the return value back into Lua value.
		//! @sa LUAPP_ARG_CONVERT
		//! @sa LUAPP_RV_CONVERT
		template<typename ReturnValueType, typename ... ArgTypes>
		Temporary wrap(ReturnValueType (*fn)(ArgTypes...)) noexcept;

		//! @brief Create a wrapped Lua-compatible function from member function.
		//! @details This function will create a Lua function wrapper that converts Lua arguments into native values,
		//! calls the C function with those arguments and converts the return value back into Lua value. First argument
		//! is expected to be convertible to Host.
		//! @sa LUAPP_ARG_CONVERT
		//! @sa LUAPP_RV_CONVERT
		template<typename Host, typename ReturnValueType, typename ... ArgTypes>
		Temporary wrap(ReturnValueType (Host::*fn)(ArgTypes...)) noexcept;

		//! @brief Create a wrapped Lua-compatible function from generic C++ function discarding the call result.
		//! @details This function will create a Lua function wrapper that converts Lua arguments into native values,
		//! calls the C function with those arguments, discards call result and returns nothing.
		//! @sa LUAPP_ARG_CONVERT
		template<typename ReturnValueType, typename ... ArgTypes>
		Temporary vwrap(ReturnValueType (*fn)(ArgTypes...)) noexcept;

		//! @brief Create a wrapped Lua-compatible function from member function discarding the call result.
		//! @details This function will create a Lua function wrapper that converts Lua arguments into native values,
		//! calls the C function with those arguments and converts the return value back into Lua value. First argument
		//! is expected to be convertible to Host.
		//! @sa LUAPP_ARG_CONVERT
		template<typename Host, typename ReturnValueType, typename ... ArgTypes>
		Temporary vwrap(ReturnValueType (Host::*fn)(ArgTypes...)) noexcept;

		//! @brief Compile a string into a chunk.
		//! @pre chunkText != nullptr
		//! @throw std::runtime_error on compilation failure (syntax errors). Exception object will contain error description.
		Temporary chunk(const char* chunkText) noexcept;

		//! @brief Compile a string into a chunk.
		//! @throw std::runtime_error on compilation failure (syntax errors). Exception object will contain error description.
		//! @overload
		Temporary chunk(const std::string& chunkText) noexcept;

		//! @brief Load a file as chunk.
		//! @pre fileName != nullptr
		//! @throw std::runtime_error on compilation failure (file missing or syntax errors). Exception object will contain error description.
		Temporary load(const char* fileName) noexcept;

		//! @brief Load a file as chunk.
		//! @throw std::runtime_error on compilation failure (file missing or syntax errors). Exception object will contain error description.
		//! @overload
		Temporary load(const std::string& fileName) noexcept;
#else	// Not DOXYGEN_ONLY

		template<typename ... UVTypes>
		_::Lazy<_::lazyClosure<UVTypes...>> closure (CFunction fn, UVTypes&& ... up_values) noexcept
		{
			return _::Lazy<_::lazyClosure<UVTypes...>>(*this, fn, std::forward<UVTypes>(up_values)...);
		}

		template<typename ... UVTypes>
		_::Lazy<_::lazyClosure<_::wrap::Envelope<LFunction>, UVTypes...>> closure (LFunction fn, UVTypes&& ... up_values) noexcept
		{
			return _::Lazy<_::lazyClosure<_::wrap::Envelope<LFunction>, UVTypes...>>(*this, _::LFunctionUWrapper, fn, std::forward<UVTypes>(up_values)...);
		}

		template<typename ReturnValueType, typename ... ArgTypes>
		_::Lazy<_::lazyClosure<_::wrap::Envelope<ReturnValueType(*)(ArgTypes...)>>> wrap(ReturnValueType (*fn)(ArgTypes...)) noexcept
		{
			return _::Lazy<_::lazyClosure<_::wrap::Envelope<decltype(fn)>>>(*this, mkcf<_::wrap::call<ReturnValueType, ArgTypes...>>, fn);
		}

		template<typename Host, typename ReturnValueType, typename ... ArgTypes>
		_::Lazy<_::lazyClosure<_::wrap::Envelope<ReturnValueType(Host::*)(ArgTypes...)>>> wrap(ReturnValueType (Host::*fn)(ArgTypes...)) noexcept
		{
			return _::Lazy<_::lazyClosure<_::wrap::Envelope<decltype(fn)>>>(*this, mkcf<_::wrap::memberCall<Host, ReturnValueType, ArgTypes...>>, fn);
		}

		template<typename Host, typename ReturnValueType, typename ... ArgTypes>
		_::Lazy<_::lazyClosure<_::wrap::Envelope<ReturnValueType(Host::*)(ArgTypes...) const>>> wrap(ReturnValueType (Host::*fn)(ArgTypes...) const) noexcept
		{
			return _::Lazy<_::lazyClosure<_::wrap::Envelope<decltype(fn)>>>(*this, mkcf<_::wrap::memberCall<Host, ReturnValueType, ArgTypes...>>, fn);
		}

		template<typename ... ArgTypes>
		_::Lazy<_::lazyClosure<_::wrap::Envelope<void (*)(ArgTypes...)>>> wrap(void (*fn)(ArgTypes...)) noexcept
		{
			return _::Lazy<_::lazyClosure<_::wrap::Envelope<decltype(fn)>>>(*this, mkcf<_::wrap::callv<void, ArgTypes...>>, fn);
		}

		template<typename Host, typename ... ArgTypes>
		_::Lazy<_::lazyClosure<_::wrap::Envelope<void (Host::*)(ArgTypes...)>>> wrap(void (Host::*fn)(ArgTypes...)) noexcept
		{
			return _::Lazy<_::lazyClosure<_::wrap::Envelope<decltype(fn)>>>(*this, mkcf<_::wrap::memberCallv<Host, void, ArgTypes...>>, fn);
		}

		template<typename Host, typename ... ArgTypes>
		_::Lazy<_::lazyClosure<_::wrap::Envelope<void (Host::*)(ArgTypes...) const>>> wrap(void (Host::*fn)(ArgTypes...) const) noexcept
		{
			return _::Lazy<_::lazyClosure<_::wrap::Envelope<decltype(fn)>>>(*this, mkcf<_::wrap::memberCallv<Host, void, ArgTypes...>>, fn);
		}

		template<typename ReturnValueType, typename ... ArgTypes>
		_::Lazy<_::lazyClosure<_::wrap::Envelope<ReturnValueType(*)(ArgTypes...)>>> vwrap(ReturnValueType (*fn)(ArgTypes...)) noexcept
		{
			return _::Lazy<_::lazyClosure<_::wrap::Envelope<decltype(fn)>>>(*this, mkcf<_::wrap::callv<ReturnValueType, ArgTypes...>>, fn);
		}

		template<typename Host, typename ReturnValueType, typename ... ArgTypes>
		_::Lazy<_::lazyClosure<_::wrap::Envelope<ReturnValueType(Host::*)(ArgTypes...)>>> vwrap(ReturnValueType (Host::*fn)(ArgTypes...)) noexcept
		{
			return _::Lazy<_::lazyClosure<_::wrap::Envelope<decltype(fn)>>>(*this, mkcf<_::wrap::memberCallv<Host, ReturnValueType, ArgTypes...>>, fn);
		}

		template<typename Host, typename ReturnValueType, typename ... ArgTypes>
		_::Lazy<_::lazyClosure<_::wrap::Envelope<ReturnValueType(Host::*)(ArgTypes...) const>>> vwrap(ReturnValueType (Host::*fn)(ArgTypes...) const) noexcept
		{
			return _::Lazy<_::lazyClosure<_::wrap::Envelope<decltype(fn)>>>(*this, mkcf<_::wrap::memberCallv<Host, ReturnValueType, ArgTypes...>>, fn);
		}

		_::Lazy<_::lazyChunk> chunk(const char* chunkText) noexcept
		{
			return _::Lazy<_::lazyChunk>(*this, chunkText);
		}

		_::Lazy<_::lazyFileChunk> load(const char* fileName) noexcept
		{
			return _::Lazy<_::lazyFileChunk>(*this, fileName);
		}

		_::Lazy<_::lazyChunk> chunk(const std::string& chunkText) noexcept
		{
			return _::Lazy<_::lazyChunk>(*this, chunkText.c_str());
		}

		_::Lazy<_::lazyFileChunk> load(const std::string& fileName) noexcept
		{
			return _::Lazy<_::lazyFileChunk>(*this, fileName.c_str());
		}

#endif	// DOXYGEN_ONLY

		//! @brief Run a command string.
		//! @pre command != nullptr
		//! @throw std::runtime_error on compilation or execution failure. Exception object will contain error description.
		void runString(const char* command)
		{
			Value c = chunk(command);
			c();
		}

		//! @brief Run a command string.
		//! @throw std::runtime_error on compilation or execution failure. Exception object will contain error description.
		//! @overload
		void runString(const std::string& command)
		{
			Value c = chunk(command);
			c();
		}

		//! @brief Execute file.
		//! @pre fileName != nullptr
		//! @throw std::runtime_error on compilation or execution failure. Exception object will contain error description.
		void runFile(const char* fileName)
		{
			Value f = load(fileName);
			f();
		}

		//! @brief Execute file.
		//! @throw std::runtime_error on compilation or execution failure. Exception object will contain error description.
		//! @overload
		void runFile(const std::string& fileName)
		{
			Value f = load(fileName);
			f();
		}
		//! @}


		//! @name Metatables
		//! @{

#ifdef DOXYGEN_ONLY
		//! @brief Metatable accessor for user data type.
		//! @details Use this function to retrieve or set metatable for registered userdata. The userdata type is specified explicitly with this function.
		//! @note This is a better alternative to using @ref lua::Context::registry "registry" with type description strings.
		//! @sa LUAPP_USERDATA
		template<typename UD> Temporary mt () noexcept;
#else	// Not DOXYGEN_ONLY
		template<typename UD>
		_::Lazy<_::lazyConstIndexer<const char*>> mt () noexcept
		{
			return registry[UserData<typename _::strip<UD>::type>::classname];
		}
#endif	// DOXYGEN_ONLY
		//! @}



		//! @name General information
		//! @{

#ifdef DOXYGEN_ONLY
		//! Version number (major * 100 + minor) @lv52.
		int getVersion() const noexcept;
#else	// Not DOXYGEN_ONLY
#if(LUAPP_API_VERSION >= 52)
		int getVersion() const noexcept;
#endif	// V52+
#endif	// DOXYGEN_ONLY
		//! @}

		//! @name Direct Lua API interaction
		//! @{

		//! @brief Access to raw state pointer via implicit conversion.
		operator ::lua_State* () const noexcept
		{
			return L;
		}

		//! @brief Current stack size.
		size_t getTop() const noexcept;
		//! @}

	private:
		// data
		lua_State * L;
		bool returning = false;

	public:

		//! @name Accessor objects and accompanying functions
		//! @{

#ifdef DOXYGEN_ONLY
		//! @brief Global variable accessor.
		//! @details Indexed with strings (const char* and std::string).
		GlobalIndexer global;

		//! @brief Upvalue accessor.
		//! @details Indexed with 1-based integer indices.
		UpvalueIndexer upvalues;
#else	// Not DOXYGEN_ONLY
		_::globalIndexer global;
		_::uvIndexer upvalues;
#endif	// DOXYGEN_ONLY

		//! @brief Function arguments.
		//! @details @sa lua::Valset
		const Valset args;

		//! @brief Check for function arguments and count.
		//! @details Checks @ref lua::Context::args "args" for existence of arguments of specified types.
		//! The function checks for presence of arguments convertible to types specified in template parameters.
		//! If "amount" is bigger than the number of template parameters, the function also checks that <code>args.size() >= amount</code>.
		//! Example: <code> c.checkArgs<double, string>(4); </code> checks that 4 arguments were passed to the function,
		//! first of which is a number and the second is a string.
		//! @note <code><b>void</b></code> is acceptable in template parameter list as a "placeholder" type, matching any type of value.
		//! @param amount Minimum amount of arguments that must be present on the stack.
		//! @return <code><b>true</b></code> if required amount of arguments of required types is present, <code><b>false</b></code> otherwise.
		//! @see requireArgs
		template<typename ... ArgTypes>
		bool checkArgs(size_t amount = 0) noexcept
		{
			if(args.size() < std::max(sizeof ... (ArgTypes), amount))
				return false;
			return checkArg<ArgTypes..., void>(0);
		}

		//! @brief Check for function arguments and count and report an error in case of failure.
		//! @details Checks @ref lua::Context::args "args" for existence of arguments of specified types.
		//! The function checks for presence of arguments convertible to types specified in template parameters.
		//! If "amount" is bigger than the number of template parameters, the function also checks that args.size() >= amount.
		//! If the specification is not met, Lua error with diagnostic message is raised.
		//! @note <code><b>void</b></code> is acceptable in template parameter list as a "placeholder" type, matching any type of value.
		//! @param amount Minimum amount of arguments that must be present on the stack.
		//! @see checkArgs
		template<typename ... ArgTypes> void requireArgs(size_t amount = 0);

		//! @brief Registry accessor.
		//! @details @li use <code>int key = registry.store(any_value);</code> to get integer key for stored value;
		//! @li index registry with integer keys to access stored values;
		//! @li index registry with type-associated strings to access userdata metatables (or better use @ref lua::Context::mt "mt" function instead).
		Registry registry;
		//! @}

	private:

		// valset block tracking
		const Valset* currentVs = nullptr;

		const Valset* getCurrentVs() const noexcept
		{
			return currentVs;
		}

		const Valset* swapVs(const Valset* newVs) noexcept
		{
			auto rv = currentVs;
			currentVs = newVs;
			return rv;
		}

		// pop tracking
#ifdef LUAPP_WATCH_STACK
		int currentStackSize = 0;
		int getCheckedTop() const noexcept;
#endif // LUAPP_WATCH_STACK

		// utility and friend-only operations

		//! Pop a value from the stack.
		void pop(size_t amount = 1) noexcept;

		//! Single-argument push is explicitly specialized for supported types
		//! @tparam T the type of the value, convertible to any of:
		//! - Nil
		//! - bool
		//! - double (implicit conversion from any numeric type)
		//! - const char*
		//! - std::string
		//! - CFunction
		//! - LFunction
		//! - closure
		//! - LightUserData
		//! - Value
		//! - element of Valset
		//! - Valset
		//! - Table
		//! - field
		//! - result(s) of a call
		//! - result of an indexing operation
		//! - chunk

		//! Single-argument push behaves exactly like push for literals,
		//! but forces lazies to push exactly one value
		template<typename T> void ipush(T&& val) noexcept
		{
			push(std::forward<T>(val));
		}

		void ipush(const Valset&) noexcept = delete;
		void ipush(Valset&&) = delete;

		template<typename Policy> void ipush(const _::Lazy<Policy>& l)
		{
			l.pushSingle();
		}

		//! Actual push specializations
		void push(const Nil&) noexcept;
		void push(bool) noexcept;

#if(LUAPP_API_VERSION <= 52)
		void push(int val) noexcept
		{
			push(double(val));
		}

		void push(unsigned int val) noexcept
		{
			push(double(val));
		}

		void push(long long val) noexcept
		{
			push(double(val));
		}

		void push(unsigned long long val) noexcept
		{
			push(double(val));
		}

#else	// V53+
		void push(int val) noexcept
		{
			push(static_cast<long long>(val));
		}

		void push(unsigned int val) noexcept
		{
			push(static_cast<unsigned long long>(val));
		}

		void push(long long val) noexcept;
		void push(unsigned long long val) noexcept;
#endif
		void push(float val) noexcept
		{
			push(double(val));
		}

		void push(double) noexcept;
		void push(const char*) noexcept;
		void push(CFunction) noexcept;
		void push(LightUserData) noexcept;

		void push(const std::string& str)  noexcept
		{
			push(str.c_str());
		}

		template<size_t N> void push(const char (&str)[N]) noexcept
		{
			push(static_cast<const char*>(str));
		}

		template<typename Policy>
		void push(_::Lazy<Policy>&& l)
		{
			l.push();
		}

		void push(LFunction f)
		{
			push(closure(f));
		}

		void push(const Valref& cVal) noexcept
		{
			duplicate(cVal.index);
		}

		void push(const Table& tVal) noexcept
		{
			push(static_cast<const Valref&>(tVal));
		}

		void push(const Valset& vs) noexcept
		{
			std::for_each(
				vs.cbegin(),
				vs.cend(),
				[this] (const Valref& v)
				{
					push(v);
				}
			);
		}

		//! Userdata-push (copy)
		template<typename UDT>
		void push(const UDT& ud, typename UserData<typename _::strip<UDT>::type>::enabled * = nullptr) noexcept
		{
			new (allocateUD(sizeof(typename _::strip<UDT>::type))) typename _::strip<UDT>::type(ud);
			setupUD(UserData<typename _::strip<UDT>::type>::classname);
		}

		//! Userdata-push (move)
		template<typename UDT>
		void push(UDT&& ud, typename UserData<typename _::strip<UDT>::type>::enabled * = nullptr) noexcept
		{
			new (allocateUD(sizeof(typename _::strip<UDT>::type))) typename _::strip<UDT>::type(std::move(ud));
			setupUD(UserData<typename _::strip<UDT>::type>::classname);
		}

		//! Enveloped function ptr push
		template<typename T>
		void push(const _::wrap::Envelope<T>& fptr)
		{
			if(sizeof(T) == sizeof(LightUserData))
				push(reinterpret_cast<LightUserData>(fptr.data));
			else
				*reinterpret_cast<T*>(allocateUD(sizeof(T))) = fptr.data;
		}

		//! Mass-push all arguments
		template<typename VT, typename ... OVT>
		void masspush(VT&& v, OVT&& ... ov)
		{
			push(std::forward<VT>(v));
			masspush(std::forward<OVT>(ov)...);
		}

		//! Terminal mass-push
		void masspush() const noexcept
		{
		}

		void push(Value&&) = delete;
		void push(Table&&) = delete;
		void push(Valset&&) = delete;

		//! autowrap and push generic C function
		template<typename RV, typename ... Args>
		void push(RV (*fn)(Args...))
		{
			push(wrap(fn));
		}

		//! autowrap and push member function
		template<typename Host, typename RV, typename ... Args>
		void push(RV (Host::*fn)(Args...))
		{
			push(wrap(fn));
		}

		//! autowrap and push const member function
		template<typename Host, typename RV, typename ... Args>
		void push(RV (Host::*fn)(Args...) const)
		{
			push(wrap(fn));
		}

		//! duplicate a stack value and return its index
		size_t duplicate(size_t index) noexcept;

		void remove(size_t index);

		//! Make an actual call
		void doCall(size_t oldtop, size_t retnum = size_t(-1)) noexcept;

		//! Actually create closure
		void makeClosure(CFunction f, size_t oldtop) noexcept;

#if(LUAPP_API_VERSION >= 52)
		//! perform arithmetic operation
		void doArith(_::Arithmetics op) noexcept;
#endif	// V52+

		//! perform comparison operation
		bool doCompare(int idx1, int idx2, _::Comparison op) noexcept;

		//! perform concatenation
		void doConcat(size_t oldtop) noexcept;

		//! Create a temporary stack value (always on stack top)
		_::tValue makeTemp() noexcept
		{
			return _::tValue(*this, getTop());
		}

		//! Allocate userdata on the top
		void* allocateUD(size_t size) noexcept;

		//! Set userdata metatable
		void setupUD(const char* mtrefname) noexcept;

		//! break execution and raise Lua error
		Retval doerror() const;

		//! Store top value in the registry
		RegistryKey makeReference();

		//! Check for argument types recursively
		template<typename T, typename ... OtherArgTypes>
		bool checkArg (size_t idx) noexcept
		{
			if(args[idx].is<typename std::conditional<std::is_void<T>::value, Value, T>::type>())
				return checkArg<OtherArgTypes ...>(idx + 1);
			else
				return false;
		}

		//! Require argument type recursively
		template<typename T, typename ... OtherArgTypes> void requireArg(size_t idx);
	};

	template<>
	inline bool Context::checkArg<void> (size_t) noexcept
	{
		return true;
	}


	template<>
	inline void Context::requireArg<void> (size_t)
	{
	}

}


#endif // LUA_CONTEXT_HXX_INCLUDED
