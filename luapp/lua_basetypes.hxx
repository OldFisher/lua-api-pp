/*
* This file is part of Lua API++ library (https://github.com/OldFisher/lua-api-pp)
* distributed under MIT License (http://opensource.org/licenses/MIT).
* See license.txt for details.
* (c) 2014 OldFisher
*/

#ifndef LUA_BASETYPES_HPP_INCLUDED
#define LUA_BASETYPES_HPP_INCLUDED

// Lua++: base lua types and their identification

struct lua_State;


//##############################################################################

namespace lua {



#ifdef DOXYGEN_ONLY
	//! @brief Lua-compatible value.
	//! @details This is not an actual type, it designates any type of value
	//! that is or can become a Lua value: references to Lua stack, temporaries,
	//! native numbers and strings, userdata etc.
	class Valobj{
	private:
	};

	//! @brief Temporary Lua value.
	//! @details This is not an actual type, see the @ref basic_values_temporary "explanation".
	//! @sa lua::Valref
	class Temporary{
	private:
	};
#endif // DOXYGEN_ONLY

	//! @cond
	class Context;
	class Valref;
	class Valset;
	class Value;
	class Table;

	namespace _ {

		//! Mix-in that disallows all forms of operator new.
		class noNew {
			// No new. At all. Seriously, I mean it!
			void* operator new(size_t) = delete;
			void* operator new[](size_t) = delete;
			void* operator new(size_t, void*) = delete;
			void* operator new[](size_t, void*) = delete;
			void* operator new(size_t, const std::nothrow_t&) = delete;
			void* operator new[](size_t, const std::nothrow_t&) = delete;
		};

		//! Mix-in that disallows copy (also prohibits objects from being returned from functions
		class nonCopyable {
		public:
			nonCopyable() = default;
		private:
			// deleted operations
			nonCopyable(const nonCopyable&) = delete;
			nonCopyable& operator = (const nonCopyable&) = delete;
		};

		//! Allows to return values from any functions with RVO, but prevents copy
		class returnableByAny {
		public:
			returnableByAny(const returnableByAny&);
			returnableByAny& operator = (const returnableByAny&) = delete;
		protected:
			returnableByAny() = default;
		};

		//! Allows to return values from friend functions with RVO, but prevents copy
		class returnableByFriends {
		protected:
			returnableByFriends() = default;
			returnableByFriends(const returnableByFriends&);
			returnableByFriends& operator = (const returnableByFriends&) = delete;
		};


		template<bool enable, typename T>
		inline const T& enableIf(const T& arg)
		{
			return static_cast<typename std::enable_if<enable, const T&>::type>(arg);
		}

		template<bool enable, typename T>
		inline T&& enableIf(T&& arg)
		{
			return static_cast<typename std::enable_if<enable, T&&>::type>(arg);
		}

	}
	//! @endcond




	//! @brief Type of @ref lua::nil "nil" value.
	//! @details This type can be used in type checks with function @ref lua::Valref::is "is".
	class Nil final{
	private:
	};



	//! @brief Constant representing nil value.
	//! @note It is possible to check values for emptiness by comparing (equality or inequality) against nil value, but @ref lua::Valref::is "is<Nil>()" is more efficient.
	static constexpr Nil nil{};



	//! @cond
	class Retval;
	namespace _ {
		int LFunctionWrapper(Retval(*)(Context&), lua_State*) noexcept;
		int LFunctionUWrapper(lua_State*) noexcept;
	}
	//! @endcond



	//! @brief Return value for Lua functions.
	//! @details This type is used for returning values from functions. You cannot create Retval directly,
	//! only by using @ref lua::Context::ret "Context::ret" or @ref lua::Context::error "Context::error" functions.
	//! You can, however, delegate its creation to another function like this: @code{.cpp}
	//! Retval delegated(int rv, Context& context){  // Note that this function does not conform to LFunction specification
	//!     return context.ret(rv);
	//! }
	//!
	//! Retval myFunction(Context& context){   // This is a proper LFunction
	//!     return delegated(42, context);   // Returns externally created Retval
	//! }
	//! @endcode
	//! @warning After Retval is created, automatic stack management is shut down, so no other stack-involving actions must be taken
	//! after that happens. The best way to guarantee that is to use any Retval-creating expressions only with <code><b>return</b></code> operator.
	class Retval final: public _::returnableByAny, public _::noNew {
		friend class Context;
		friend int _::LFunctionWrapper(Retval(*)(Context&), lua_State*) noexcept;
		friend int _::LFunctionUWrapper(lua_State*) noexcept;

		explicit Retval(size_t amount) noexcept:
			rvamount(amount)
		{
		}

		const size_t rvamount;
	};



	//! @brief C function callable by Lua (plain Lua API format).
	//! @details @sa lua::mkcf
	using CFunction = int (*)(lua_State*);

	class Context;



	//! @brief Lua-API++ compatible function.
	//! @details @sa lua::mkcf
	using LFunction = Retval (*)(Context&);



	//! @brief Light userdata.
	//! @details This typedef is provided for explicit designation of light userdata.
	typedef void* LightUserData;


	//! @brief Types of Lua values.
	//! @details This enum is used to identify actual types of Lua values both in compile-time and runtime.
	enum class ValueType {
		None = -1,		//!< not a Lua value
		Nil,			//!< absence of data
		Boolean,		//!< boolean
		LightUserdata,	//!< light userdata
		Number,			//!< number
		String,			//!< string
		Table,			//!< table
		Function,		//!< Lua function
		C_Function,		//!< Lua-compatible C function
		UserData,		//!< full userdata
		Thread,			//!< Lua documentation names this object "coroutine", but Lua API names it "thread"
		Any				//!< Lua value with content type indeterminable at the time of query
	};

	//! @cond
	template <typename UserDataType> struct UserData {};

	template struct UserData<int>;
	template struct UserData<unsigned int>;
	template struct UserData<long long>;
	template struct UserData<unsigned long long>;
	template struct UserData<void>;
	template struct UserData<float>;
	template struct UserData<double>;
	template struct UserData<const char*>;
	template struct UserData<std::string>;
	template struct UserData<CFunction>;
	template struct UserData<LFunction>;
	template struct UserData<Value>;
	template struct UserData<Valref>;
	template struct UserData<Table>;

	//! @endcond


	//! @brief Determine Lua type ID from native type.
	//! @details Use this metafunction to determine Lua type ID that corresponds to given native type in compile-time.
	template<typename T> struct TypeID {
	private:
		template<typename> static constexpr ValueType genericTypeId(...) noexcept { return ValueType::None;}
		template<typename T2> static constexpr ValueType genericTypeId(typename UserData<T2>::enabled*) noexcept { return ValueType::UserData;}

	public:
		//! Result of metafunction.
		static constexpr ValueType typeID = genericTypeId<T>(nullptr);
	};

	//! @cond
	template <> struct TypeID<Nil>
	{
		static constexpr ValueType typeID = ValueType::Nil;
	};

	template <> struct TypeID<bool>
	{
		static constexpr ValueType typeID = ValueType::Boolean;
	};

	template <> struct TypeID<LightUserData>
	{
		static constexpr ValueType typeID = ValueType::LightUserdata;
	};

	template <> struct TypeID<int>
	{
		static constexpr ValueType typeID = ValueType::Number;
	};

	template <> struct TypeID<unsigned int>
	{
		static constexpr ValueType typeID = ValueType::Number;
	};

	template <> struct TypeID<long long>
	{
		static constexpr ValueType typeID = ValueType::Number;
	};

	template <> struct TypeID<unsigned long long>
	{
		static constexpr ValueType typeID = ValueType::Number;
	};

	template <> struct TypeID<float>
	{
		static constexpr ValueType typeID = ValueType::Number;
	};

	template <> struct TypeID<double>
	{
		static constexpr ValueType typeID = ValueType::Number;
	};

	template <> struct TypeID<const char*>
	{
		static constexpr ValueType typeID = ValueType::String;
	};

	template <> struct TypeID<std::string>
	{
		static constexpr ValueType typeID = ValueType::String;
	};

	template <> struct TypeID<CFunction>
	{
		static constexpr ValueType typeID = ValueType::C_Function;
	};

	template <> struct TypeID<LFunction>
	{
		static constexpr ValueType typeID = ValueType::Function;
	};

	template <> struct TypeID<Value>
	{
		static constexpr ValueType typeID = ValueType::Any;
	};

	template <> struct TypeID<Valref>
	{
		static constexpr ValueType typeID = ValueType::Any;
	};

	template <> struct TypeID<Table>
	{
		static constexpr ValueType typeID = ValueType::Table;
	};


	//! @endcond


//##############################################################################

	//! @cond
	namespace _ {


#if(LUAPP_API_VERSION >= 52)
		//! Arithmetic operation codes (must match defines from lua.h)
		enum class Arithmetics: int {
#if(LUAPP_API_VERSION == 52)
			Add,
			Sub,
			Multiply,
			Divide,
			Modulo,
			Power,
			UnaryMinus
#else
			Add,
			Sub,
			Multiply,
			Modulo,
			Power,
			Divide,
			IntegerDivide,
			BitwiseAnd,
			BitwiseOr,
			BitwiseXor,
			ShiftLeft,
			ShiftRight,
			UnaryMinus,
			BitwiseNeg
#endif
		};
#endif	// V52+

		//! Comparison operation codes (must match defines from lua.h)
		enum class Comparison: int {
			Equal,
			Less,
			LessEqual
		};


//##############################################################################

		template<typename Policy> class Lazy;



		template<typename UDT> struct strip {
			typedef typename std::remove_cv<typename std::remove_pointer<typename std::remove_reference<UDT>::type>::type>::type type;
		};



		//! Check whether a value can be implicitly converted to T
		template<typename T> struct ValueConvertibleTo {
			typedef typename std::decay<typename strip<T>::type>::type Ts;
			static constexpr const bool value =
				   TypeID<Ts>::typeID != ValueType::None
				&& TypeID<Ts>::typeID != ValueType::Any
				&& TypeID<Ts>::typeID != ValueType::Nil
				&& TypeID<Ts>::typeID != ValueType::Table;
		};

		//! Check if value is an anchor
		template<typename T> struct IsAnchor{
			static constexpr const bool value =
				std::is_base_of<Valref, typename _::strip<T>::type>::value ||
				std::is_same<Table, typename _::strip<T>::type>::value;
		};



		//! Check if type is an instance of _::Lazy with any policy
		template<typename T> struct IsLazy {
			static constexpr const bool value = false;
		};

		template<typename Policy> struct IsLazy<_::Lazy<Policy>> {
			static constexpr const bool value = true;
		};



		//! Check if type has Lua-value semantics and can do arithmetics/concat
		template<typename T> struct HasValueSemantics{
			static constexpr const bool value =
				IsAnchor<typename _::strip<T>::type>::value ||
				IsLazy<typename _::strip<T>::type>::value;
		};


//##############################################################################


		template<typename> class lazyConstIndexer;
		template<typename, typename ...> class lazyCall;
		template<typename, typename ...> class lazyPCall;
		class uvIndexer;
		class lazyExtConstUpvalue;
#if(LUAPP_API_VERSION >= 52)
		class lazyLen;
#endif	// V52+
		class lazyMT;
#if(LUAPP_API_VERSION >= 53)
		class lazyLinked;
		class lazyConstIntIndexer;
#endif	// V53+
		Context& extractContext(const Valref&) noexcept;
		class vsIterator;
		class vsCIterator;

		template<typename, typename> class lazyConcat;
#if(LUAPP_API_VERSION >= 52)
		template<typename, typename, _::Arithmetics> class lazyArithmetics;
		template<typename, _::Arithmetics> class lazyArithmeticsUnary;
#endif	// V52+
	}
	//! @endcond



#if(LUAPP_API_VERSION > 51 || defined(DOXYGEN_ONLY))
	//! @brief Closure properties. @lv52
	//! @see lua::Valref::getClosureInfo
	struct ClosureInfo {
		size_t nUpvalues;		//!< Amount of upvalues.
		size_t nParameters;	//!< Amount of parameters.
		bool variadic;			//!< Whether function accepts variable number of arguments.
	};
#endif


	class Valref;

	//! @name Comparisons (made through Lua)
	//! @{
#ifdef DOXYGEN_ONLY

	//! @relates lua::Valref
	bool operator == (Valobj&& lhs, const Valref& rhs);

	//! @relates lua::Valref
	bool  operator != (Valobj&& lhs, const Valref& rhs);

	//! @relates lua::Valref
	bool operator < (Valobj&& lhs, const Valref& rhs);

	//! @relates lua::Valref
	bool operator > (Valobj&& lhs, const Valref& rhs);

	//! @relates lua::Valref
	bool operator <= (Valobj&& lhs, const Valref& rhs);

	//! @relates lua::Valref
	bool operator >= (Valobj&& lhs, const Valref& rhs);
#else
	template <typename ValueType> typename std::enable_if<!_::IsAnchor<ValueType>::value, bool>::type operator == (ValueType&& lhs, const Valref& rhs);
	template <typename ValueType> typename std::enable_if<!_::IsAnchor<ValueType>::value, bool>::type operator != (ValueType&& lhs, const Valref& rhs);
	template <typename ValueType> typename std::enable_if<!_::IsAnchor<ValueType>::value, bool>::type operator < (ValueType&& lhs, const Valref& rhs);
	template <typename ValueType> typename std::enable_if<!_::IsAnchor<ValueType>::value, bool>::type operator > (ValueType&& lhs, const Valref& rhs);
	template <typename ValueType> typename std::enable_if<!_::IsAnchor<ValueType>::value, bool>::type operator <= (ValueType&& lhs, const Valref& rhs);
	template <typename ValueType> typename std::enable_if<!_::IsAnchor<ValueType>::value, bool>::type operator >= (ValueType&& lhs, const Valref& rhs);
#endif // DOXYGEN_ONLY
	//! @}


	//! @brief Non-owning reference to Lua value.
	//! @details This is a reference to a specific slot on Lua stack. The operations with Lua value inside that slot are performed through this object.
	//! @note This class also covers values that are not actually on the stack but are addressed with pseudo-indices.
	class Valref {

		template<typename> friend class _::Lazy;
		friend class lua::Context;
		friend class lua::Valset;
		friend class lua::Value;
		friend class lua::Table;
		friend class lua::_::uvIndexer;
		friend class lua::_::vsIterator;
		friend class lua::_::vsCIterator;
		friend Context& lua::_::extractContext(const Valref&) noexcept;

		template<typename, typename> friend class lua::_::lazyConcat;

	public:

		//! @name Life cycle
		//! @{

		//! @brief Copy constructor.
		//! @details Copies of @ref lua::Valref "Valref" point to the same stack slot.
		//! Be careful with copying Valref objects just as you would be with normal references.
		//! This copy constructor is enabled so it is possible to pass references as function arguments.
		//! @note Obviously, this operation is not matched in @ref lua::Temporary "Temporary" interface.
		Valref(const Valref&) noexcept = default;

		//! @brief Assignment.
		//! @details Lua values, supported native types or registered userdata can be assigned to a referenced stack slot.
		//! @note Some types of Temporary objects may be assigned to (indexers, metatables and so on).
		//! Assignment to other kinds (like arithmetic results or length query) has no sense and thus cannot be done.
		template<typename ValueType> Valref& operator = (ValueType&& val);
		//! @}

		//! @name Type info and conversion
		//! @{


#ifdef DOXYGEN_ONLY

		//! @brief Explicit checked conversion to supported types.
		//! @details Call this function with type to be converted to.
		//! @throw std::runtime_error if the value type is incompatible.
		//! @tparam T the type to cast to, one of those:
		//! - bool (NB: ANY value can be converted to boolean)
		//! - int
		//! - unsigned int
		//! - float
		//! - double
		//! - const char*
		//! - @ref lua::CFunction "CFunction"
		//! - @ref lua::LightUserData "LightUserData"
		//! - any user data type (see @ref basic_values_user "this section"), returned by reference
		template<typename T> T cast() const;
#else
		template<typename T> typename std::enable_if<TypeID<T>::typeID != ValueType::UserData, T>::type cast() const;

		template<typename UDT> UDT& cast(typename UserData<UDT>::enabled * = nullptr) const
		{
			return *static_cast<UDT*>(readUserData(UserData<UDT>::classname));
		}
#endif // DOXYGEN_ONLY


#ifdef DOXYGEN_ONLY
		//! @brief Safe conversion to supported types @lv52m.
		//! @deprecated use @ref Valref::to "to" function instead.
		//! @return Converted value or fallback value if the conversion cannot be made.
		//! @tparam T the type to cast to. Converts to all types supported by @ref lua::Valref::cast "cast" <b>except user data</b>.
		template<typename T> T optcast(const T& backupValue = T()) const noexcept;

		//! @brief Unchecked conversion to supported types.
		//! @return Conversion result or unspecified value if the conversion cannot be made.
		//! Use when you are sure what the type of value is and don't want additional checks during conversion.
		//! @tparam T the type to cast to. Converts to all types supported by @ref lua::Valref::cast "cast" <b>including user data</b>.
		//! @note For userdata type a reference is returned.
		template<typename T> T to() const noexcept;

		//! @brief Safe conversion to supported types.
		//! @return Converted value or fallback value if the conversion cannot be made.
		//! @tparam T the type to cast to. Converts to all types supported by @ref lua::Valref::cast "cast" <b>except user data</b>.
		template<typename T> T to(const T& backupValue) const noexcept;

#else	// Not DOXYGEN_ONLY
#if(LUAPP_API_VERSION <= 52)
		template<typename T>
		T optcast(const T& backupValue = T()) const noexcept
		{
			return to<T>(backupValue);
		}
#endif	// V52-
		template<typename T> typename std::enable_if<TypeID<T>::typeID != ValueType::UserData, T>::type to() const;

		template<typename UDT> UDT& to(typename UserData<UDT>::enabled * = nullptr) const
		{
			return *static_cast<UDT*>(to<LightUserData>());
		}

		template<typename T> T to(const T& backupValue) const noexcept
		{
			return (std::is_same<T, bool>::value || is<T>()) ? to<T>() : backupValue;
		}

#endif	// DOXYGEN_ONLY

		operator bool () const;			//!< @details Never fails.
		operator int () const;			//!< @throw std::runtime_error if the value type is incompatible. @note Strings may be converted to numbers.
		operator unsigned int() const;	//!< @throw std::runtime_error if the value type is incompatible. @note Strings may be converted to numbers.
		operator long long() const;			//!< @throw std::runtime_error if the value type is incompatible. @note Strings may be converted to numbers.
		operator unsigned long long() const;	//!< @throw std::runtime_error if the value type is incompatible. @note Strings may be converted to numbers.
		operator float() const;			//!< @throw std::runtime_error if the value type is incompatible. @note Strings may be converted to numbers.
		operator double() const;			//!< @throw std::runtime_error if the value type is incompatible. @note Strings may be converted to numbers.
		operator CFunction () const;		//!< @throw std::runtime_error if the value type is incompatible.
		operator LightUserData() const;	//!< @throw std::runtime_error if the value type is incompatible.
		operator const char* () const;	//!< @throw std::runtime_error if the value type is incompatible. @note Numbers are convertible to strings.
		operator std::string () const	//!  @throw std::runtime_error if the value type is incompatible. @note Numbers are convertible to strings.
		{
			return std::string(static_cast<const char*>(*this));
		}

#ifdef DOXYGEN_ONLY
		//! @brief Check if the value is of given type or convertible.
		//! @details Exception: is<bool> checks if value type is actually boolean,
		//! because any value can be converted to bool.
		//! @tparam T the type to check compatibility with, one of those:
		//! - Nil
		//! - bool
		//! - int
		//! - unsigned int
		//! - float
		//! - double
		//! - const char*
		//! - std::string
		//! - @ref lua::CFunction "CFunction" (true if the value is a C function)
		//! - @ref lua::LFunction "LFunction" (true if the value is a function, no matter C or Lua bytecode)
		//! - @ref lua::LightUserData "LightUserData"
		//! - Table
		//! - any user data type (see @ref basic_values_user "this section")
		template<typename T> bool is() const noexcept;
#else	// Not DOXYGEN_ONLY
		template<typename T> typename std::enable_if<TypeID<T>::typeID != ValueType::UserData, bool>::type is() const noexcept;

		template<typename UDT> typename std::enable_if<TypeID<UDT>::typeID == ValueType::UserData, bool>::type is() const noexcept
		{
			return isUserData(UserData<UDT>::classname);
		}
#endif // DOXYGEN_ONLY

		//! @brief Actual type of referenced value.
		ValueType type() const noexcept;

		//! @}



		//! @name Comparisons (made through Lua)
		//! @{

#ifdef DOXYGEN_ONLY
		bool operator == (Valobj&& rhs) const;
		bool operator != (Valobj&& rhs) const;
		bool operator <  (Valobj&& rhs) const;
		bool operator >  (Valobj&& rhs) const;
		bool operator <= (Valobj&& rhs) const;
		bool operator >= (Valobj&& rhs) const;

#else	// Not DOXYGEN_ONLY
		bool operator == (const Valref& rhs) const noexcept;
		bool operator != (const Valref& rhs) const noexcept;
		bool operator <  (const Valref& rhs) const noexcept;
		bool operator >  (const Valref& rhs) const noexcept;
		bool operator <= (const Valref& rhs) const noexcept;
		bool operator >= (const Valref& rhs) const noexcept;

		template <typename ValueType> typename std::enable_if<!_::IsAnchor<ValueType>::value, bool>::type operator == (ValueType&& rhs) const;
		template <typename ValueType> typename std::enable_if<!_::IsAnchor<ValueType>::value, bool>::type operator != (ValueType&& rhs) const;
		template <typename ValueType> typename std::enable_if<!_::IsAnchor<ValueType>::value, bool>::type operator <  (ValueType&& rhs) const;
		template <typename ValueType> typename std::enable_if<!_::IsAnchor<ValueType>::value, bool>::type operator >  (ValueType&& rhs) const;
		template <typename ValueType> typename std::enable_if<!_::IsAnchor<ValueType>::value, bool>::type operator <= (ValueType&& rhs) const;
		template <typename ValueType> typename std::enable_if<!_::IsAnchor<ValueType>::value, bool>::type operator >= (ValueType&& rhs) const;

#endif	// DOXYGEN_ONLY

		//! @}



		//! @name Indexation
		//! @{

#ifdef DOXYGEN_ONLY
		//! @brief Indexation.
		Temporary operator [] (Valobj&& index_) const noexcept;
#else	// Not DOXYGEN_ONLY
		template<typename IndexType>
		_::Lazy<_::lazyConstIndexer<typename std::decay<IndexType>::type>> operator [] (IndexType&& index_) const noexcept
		{
			return _::Lazy<_::lazyConstIndexer<typename std::decay<IndexType>::type>>(context, index, std::forward<IndexType>(index_));
		}

#if(LUAPP_API_VERSION >= 53)
		_::Lazy<_::lazyConstIntIndexer> operator [] (int index_) const noexcept;
		_::Lazy<_::lazyConstIntIndexer> operator [] (unsigned index_) const noexcept;
		_::Lazy<_::lazyConstIntIndexer> operator [] (long long index_) const noexcept;
		_::Lazy<_::lazyConstIntIndexer> operator [] (unsigned long long index_) const noexcept;
#endif	// V53+
#endif	// DOXYGEN_ONLY
		//! @}


		//! @name Function calls
		//! @{

#ifdef DOXYGEN_ONLY
		//! @brief Natural call form.
		//! @details Any @ref lua::Valobj "suitable value" is accepted as an argument.
		//! @sa basic_state_multiret_call
		template<typename ... Args> Temporary operator () (Args&& ... args) const noexcept;

		//! @brief Call method.
		//! @details Any @ref lua::Valobj "suitable value" is accepted as an argument.
		//! @sa basic_state_multiret_call
		template<typename ... Args>
		Temporary call(Args&& ... args) const noexcept;

		//! @brief Protected call method.
		//! @details Any @ref lua::Valobj "suitable value" is accepted as an argument.
		//! @sa basic_state_multiret_call
		template<typename ... Args>
		Temporary pcall(Args&& ... args) const noexcept;

#else	// Not DOXYGEN_ONLY
		template<typename ... Args>
		_::Lazy<_::lazyCall<Valref, Args...>> operator () (Args&& ... args) const noexcept
		{
			return _::Lazy<_::lazyCall<Valref, Args...>>(context, *this, std::forward<Args>(args)...);
		}

		template<typename ... Args>
		_::Lazy<_::lazyCall<Valref, Args...>> call(Args&& ... args) const noexcept
		{
			return _::Lazy<_::lazyCall<Valref, Args...>>(context, *this, std::forward<Args>(args)...);
		}

		template<typename ... Args>
		_::Lazy<_::lazyPCall<Valref, Args...>> pcall(Args&& ... args) const noexcept
		{
			return _::Lazy<_::lazyPCall<Valref, Args...>>(context, *this, std::forward<Args>(args)...);
		}

#endif	// DOXYGEN_ONLY
		//! @}

		//! @name Closure handling
		//! @{

#ifdef DOXYGEN_ONLY
		//! @brief Upvalue access inside closures.
		//! @details You can read and write upvalues inside closures with this function.
		//! @param index 1-based index of accessed upvalue.
		//! @pre this->is<LFunction>()
		//! @throw std::runtime_error on wrong index.
		Temporary upvalue(size_t index) const noexcept;#else	// Not DOXYGEN_ONLY
#else	// Not DOXYGEN_ONLY
		_::Lazy<_::lazyExtConstUpvalue> upvalue(size_t index_) const noexcept;
#endif	// DOXYGEN_ONLY

		//! @brief Retrieve upvalues and their names.
		//! @details This function returns values and names of all closure upvalues.
		//! Upvalues are always named "" in C closures. Upvalue ordering in Lua functions is undefined.
		//! @pre this->is<LFunction>()
		//! @return Value-name pairs for each upvalue.
		Valset getUpvalues() const noexcept;

#if(defined(DOXYGEN_ONLY) || LUAPP_API_VERSION > 51)
		//! @brief Retrieve information about closure @lv52.
		//! @details This function is applicable only to closures.
		//! It returns number of upvalues, parameters and whether function
		//! is variadic (accepts variable number of arguments).
		//! @pre this->is<LFunction>()
		//! @note For closures made from C functions @c nParameters is
		//! always 0 and @c variadic is always <b><code>true</code></b>.
		ClosureInfo getClosureInfo() const noexcept;
#endif
#if(defined(DOXYGEN_ONLY) || LUAPP_API_VERSION <= 51)
		//! @brief Retrieve information about closure @lv51.
		//! @details This function is applicable only to closures.
		//! @return Number of upvalues.
		//! @pre this->is<LFunction>()
		size_t getClosureInfo() const noexcept;
#endif
		//! @}



		//! @name Miscellaneous operations
		//! @{

#ifdef DOXYGEN_ONLY
		//! @brief Length @lv52.
		Temporary len() const  noexcept;

		//! @brief Length @lv51.
		size_t len() const noexcept;

		//! @brief Metatable.
		//! @return value's metatable or @ref lua::nil "nil".
		Temporary mt() const  noexcept;

		//! @brief Linked value @lv53.
		//! @return value's linked value or @ref lua::nil "nil".
		//! @details This function allows to get and set associated value for userdata.
		//! For other types it will produce @ref lua::nil "nil" (also for userdata that doesn't have anything associated with it) on read attempts
		//! and silently ignore write attempts.
		Temporary linked() const noexcept;
#else	// Not DOXYGEN_ONLY
#if(LUAPP_API_VERSION >= 52)
		_::Lazy<_::lazyLen> len() const  noexcept;
#else	// V51-
		size_t len() const noexcept;
#endif	// V52+
		_::Lazy<_::lazyMT> mt() const  noexcept;

#if(LUAPP_API_VERSION >= 53)
		_::Lazy<_::lazyLinked> linked() const  noexcept;
#endif	// V53+
#endif	// DOXYGEN_ONLY
		//! @}

		//! @cond
	protected:
		Valref(Context& context_, int index_) noexcept:
			context(context_),
			index(index_)
		{
		}

		void* operator new(size_t, void* ptr) noexcept
		{
			return ptr;
		}

		void* operator new[](size_t, void*) = delete;

		void* operator new(size_t) = delete;
		void* operator new[](size_t) = delete;
		void* operator new(size_t, const std::nothrow_t&) = delete;
		void* operator new[](size_t, const std::nothrow_t&) = delete;


		// Utility
		//! Write the value from the top of the stack into Valref
		void replace() noexcept;
		//! Read pointer to user-data
		void* readUserData(const char* classname) const;
		//! Check if value is userdata of given type
		bool isUserData(const char* classname) const noexcept;

		//! Push all upvalues to the stack
		void pushUpvalues() const noexcept;

		//! Push single upvalue to the stack
		static const char* readUv(lua_State*, int, size_t);
#if(LUAPP_API_VERSION > 51)
		static ClosureInfo retrieveClosureInfo(lua_State*) noexcept;
#else
		static size_t retrieveClosureInfo(lua_State*) noexcept;
#endif
		// data
		Context& context;
		int index;
		//! @endcond

	};



	//! @cond
	namespace _ {

		//! Base class for temporary values (discarded after first use)
		class tValue final: public Valref {
			friend class ::lua::Context;

		public:

			~tValue() noexcept;

		protected:
			tValue(Context& context_, int index_) noexcept:
				Valref(context_, index_)
			{
			}
		};

	}
	//! @endcond



	//! @brief Key for registry-stored values.
	class RegistryKey {
		friend class ::lua::Context;

	public:
		//! @brief Empty key value (LUA_NOREF).
		constexpr static int noref = -2;

		RegistryKey(const RegistryKey&) noexcept = default;

		//! @brief Default RegistryKey is empty.
		RegistryKey() noexcept = default;

		//! @brief RegistryKey cannot be overwritten by another.
		RegistryKey& operator = (const RegistryKey&) = delete;

		//! @brief Destroy the reference (becomes non-ref).
		void kill() noexcept
		{
			value = noref;
		}

		//! @brief Check if the value is not empty.
		//! @details Only valid non-empty keys can be used with registry.
		operator bool() const noexcept
		{
			return value != noref;
		}

		//! @brief Get underlying key value.
		int get() const noexcept
		{
			return value;
		}

		//! @brief Check for equality of keys.
		bool operator == (const RegistryKey& rhs) const noexcept
		{
			return value == rhs.value;
		}

		//! @brief Check for inequality of keys.
		bool operator != (const RegistryKey& rhs) const noexcept
		{
			return value != rhs.value;
		}

	private:
		int value = noref;
		RegistryKey(int value_) noexcept:
			value(value_)
		{
		}

	};



#ifdef DOXYGEN_ONLY
	//! @name Concatenation
	//! @{

	//! @brief Concatenation.
	//! @relates lua::Valref
	//! @note
	//! @li Chained concatenations are optimized into a single multi-value concatenation;
	//! @li multiple return values and @ref Valset "Valsets" are expanded;
	//! @li the operation symbol '&' has lower priority than '+' and '-' because in C/C++ it means "bitwise AND", which matches priority of concatenation in Lua.
	Temporary operator & (Valobj&& lhs, Valobj&& rhs) const noexcept;
	//! @}



	//! @name Arithmetics (Lua 5.2+ only).
	//! @{

	//! @brief @lv52
	//! @relates lua::Valref
	Temporary operator - (Valobj&& lhs);

	//! @brief @lv52
	//! @relates lua::Valref
	Temporary operator + (Valobj&& lhs, const Valobj& rhs) noexcept;

	//! @brief @lv52
	//! @relates lua::Valref
	Temporary operator - (Valobj&& lhs, const Valobj& rhs) noexcept;

	//! @brief @lv52
	//! @relates lua::Valref
	Temporary operator * (Valobj&& lhs, const Valobj& rhs) noexcept;

	//! @brief @lv52
	//! @relates lua::Valref
	Temporary operator / (Valobj&& lhs, const Valobj& rhs) noexcept;

	//! @brief @lv52
	//! @relates lua::Valref
	Temporary operator % (Valobj&& lhs, const Valobj& rhs) noexcept;

	//! @brief Power @lv52.
	//! @relates lua::Valref
	//! @warning The operation symbol '^' has lower priority than '+' and '-' because in C/C++ it means "bitwise XOR", not "power". Use parentheses to group expressions properly.
	Temporary operator ^ (Valobj&& lhs, const Valobj& rhs) noexcept;

	//! @brief Integer division @lv53.
	//! @relates lua::Valref
	Temporary idiv(Valobj&& lhs, Valobj&& rhs);
	//! @}



	//! @name Binary operations (Lua 5.3+ only)
	//! @{

	//! @brief Bitwise AND @lv53.
	//! @relates lua::Valref
	Temporary band(Valobj&& lhs, Valobj&& rhs);

	//! @brief Bitwise OR @lv53.
	//! @relates lua::Valref
	Temporary bor(Valobj&& lhs, Valobj&& rhs);

	//! @brief Bitwise XOR @lv53.
	//! @relates lua::Valref
	Temporary bxor(Valobj&& lhs, Valobj&& rhs);

	//! @brief Bitwise NOT @lv53.
	//! @relates lua::Valref
	Temporary bneg(Valobj&& lhs);

	//! @brief Bit shift left @lv53.
	//! @relates lua::Valref
	Temporary shl(Valobj&& lhs, Valobj&& rhs);

	//! @brief Bit shift right @lv53.
	//! @relates lua::Valref
	Temporary shr(Valobj&& lhs, Valobj&& rhs);
	//! @}

#endif	// DOXYGEN_ONLY
}

#endif // LUA_BASETYPES_HPP_INCLUDED
