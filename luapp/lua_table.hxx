/*
* This file is part of Lua API++ library (https://github.com/OldFisher/lua-api-pp)
* distributed under MIT License (http://opensource.org/licenses/MIT).
* See license.txt for details.
* (c) 2014 OldFisher
*/

#ifndef LUA_TABLE_HPP_INCLUDED
#define LUA_TABLE_HPP_INCLUDED



namespace lua {

	//! @cond
	template<> bool ::lua::Valref::is<Table>() const noexcept;
	//! @endcond

	//! @cond
	namespace _ {


		template<typename T>
		struct IsTable {
			static constexpr const bool value = std::is_same<::lua::Table, typename strip<T>::type>::value;
		};

		//! Table creator policy for array value packs
		template<typename ... Values>
		class lazyTableArray: public ::lua::_::lazyPolicy {

			template<typename> friend class ::lua::_::Lazy;
		public:
			lazyTableArray(lazyTableArray<Values...>&&) noexcept = default;

		private:
			lazyTableArray(Context& s, Values ... values_):
				values(s, std::forward<Values>(values_)...)
			{
			}

			void push(Context& s);

			void pushSingle(Context& s)
			{
				push(s);
			}

			void onDestroy(Context& s)
			{
				values.onDestroy(s);
			}

			void moveout() noexcept
			{
				values.moveout();
			}

			_::lazySeries<Values...> values;
		};




		//! Table creator policy for key-value pairs
		template<typename ... KVPairs>
		class lazyTableRecords: public _::lazyPolicy {

			template<typename> friend class ::lua::_::Lazy;

		public:
			lazyTableRecords(lazyTableRecords<KVPairs...>&&) noexcept = default;

		private:
			lazyTableRecords(Context& s, KVPairs ... values_):
				values(s, std::forward<KVPairs>(values_)...)
			{
			}

			void push(Context& s);

			void pushSingle(Context& s)
			{
				push(s);
			}

			void onDestroy(Context& s)
			{
				values.onDestroy(s);
			}

			void moveout() noexcept
			{
				values.moveout();
			}

			_::lazySeries<KVPairs...> values;
		};


		class TableUtils {
		private:
			template<typename...> friend class ::lua::_::lazyTableArray;
			template<typename...> friend class ::lua::_::lazyTableRecords;
			friend class ::lua::Table;

			static int makeNew(lua_State* s, int arrSize, int recSize) noexcept;

			static void setValue(lua_State* s, int tableNum, int key) noexcept;
			static void setValue(lua_State* s, int tableNum) noexcept;
		};
	}
	//! @endcond

	//! @brief Table object with specialized interface. Similar to Value, it owns a stack slot.
	//! @details This class is similar to Value, but it offers interface tailored for table manipulation.
	//! Many generic operations are absent while new special methods and members are added.
	class Table final: public _::noNew {

		class RawAccessor {
			friend class ::lua::Table;
		public:

			template<typename IndexType>
			_::Lazy<_::lazyRawIndexer<IndexType>> operator [] (IndexType&& idx) const noexcept;

			_::Lazy<_::lazyRawIndexer<int>> operator [] (int idx) const noexcept;

			_::Lazy<_::lazyRawIndexer<int>> operator [] (unsigned int idx) const noexcept;

			_::Lazy<_::lazyRawIndexer<LightUserData>> operator [] (LightUserData idx) const noexcept;

		private:
			explicit RawAccessor(Value& t):
				T(t)
			{
			}

			RawAccessor(const RawAccessor&);

			RawAccessor& operator = (const RawAccessor&) = delete;

			~RawAccessor() noexcept
			{
			}
			// data
			Value& T;
		};

	public:

		//! @name Life cycle
		//! @{

		//! @brief Create a copy of referenced value in owned slot.
		//! @throw std::runtime_error if the source is not a table.
		Table(const Valref& val):
			Anchor(val), raw(Anchor)
		{
			checkTableness();
		}

		//! @cond
		Table(const Table& src):
			Table(static_cast<const Valref&>(src))
		{
		}

		Table(const Value& src):
			Table(static_cast<const Valref&>(src))
		{
		}
		//! @endcond

		//! @brief It is possible to return Table from functions, but actually moving it is not.
		Table(Table&&)/* = delete*/;

		//! @cond
		Table(Value&&) = delete;
		//! @endcond

		//! @brief Tables cannot be assigned to.
		Table& operator = (const Table&) = delete;

		//! @brief Create a new empty table.
		//! @details Anonymous tables cannot be used as Lua values.
		explicit Table(Context& context, size_t arrSize = 0, size_t recSize = 0)  noexcept;

#ifdef DOXYGEN_ONLY
		//! @brief Take ownership of a temporary value.
		//! @throw std::runtime_error if the source is not a table.
		Table(Temporary src);
#else	// Not DOXYGEN_ONLY
		template<typename Policy>
		Table(_::Lazy<Policy>&& src):
			Anchor(std::move(src)), raw(Anchor)
		{
			checkTableness();
		}
#endif	// DOXYGEN_ONLY

		//! @}

		//! @name Type conversion
		//! @{

		//! @brief Reference to owned slot.
		operator Valref& () noexcept
		{
			return Anchor;
		}

		//! @brief Const reference to owned slot.
		operator const Valref& () const noexcept
		{
			return Anchor;
		}
		//! @}

		//! @name Indexation
		//! @{

#ifdef DOXYGEN_ONLY
		//! @brief Indexation works as in normal Valref.
		Temporary operator [] (Valobj index) const noexcept;
#else	// Not DOXYGEN_ONLY
		template<typename IndexType>
		auto operator []  (IndexType&& index) const noexcept -> decltype(std::declval<lua::Valref>()[std::forward<IndexType>(index)])
		{
			return Anchor[std::forward<IndexType>(index)];
		}

#endif	// DOXYGEN_ONLY
		//! @}

		//! @name Comparisons (table needs only equality checks).
		//! @{

#ifdef DOXYGEN_ONLY
		bool operator == (Valobj rhs) const;
		bool operator != (Valobj rhs) const;
#else	// Not DOXYGEN_ONLY
		template<typename ValueType>
		typename std::enable_if<!_::IsTable<ValueType>::value, bool>::type operator == (ValueType&& rhs) const
		{
			return Anchor == std::forward<ValueType>(rhs);
		}

		template<typename ValueType>
		typename std::enable_if<!_::IsTable<ValueType>::value, bool>::type operator != (ValueType&& rhs) const
		{
			return Anchor != std::forward<ValueType>(rhs);
		}

		bool operator == (const Table& rhs) const noexcept
		{
			return Anchor == rhs.Anchor;
		}

		bool operator != (const Table& rhs) const noexcept
		{
			return Anchor != rhs.Anchor;
		}
#endif	// DOXYGEN_ONLY
		//! @}

		//! @name Miscellaneous
		//! @{

#ifdef DOXYGEN_ONLY
		//! @brief Length @lv52.
		//! @details Same as Valref::len.
		Temporary len() const noexcept;

		//! @brief Length @lv51.
		size_t len() const noexcept;

		//! @brief Metatable access
		//! @details Same as Valref::mt()
		Temporary mt() const noexcept;
#else	// Not DOXYGEN_ONLY

#if(LUAPP_API_VERSION >= 52)
		_::Lazy<_::lazyLen> len() const noexcept
		{
			return Anchor.len();
		}
#else	// V51-
		size_t len() const noexcept
		{
			return Anchor.len();
		}
#endif	// V52+
		_::Lazy<_::lazyMT> mt() const noexcept
		{
			return Anchor.mt();
		}
#endif	// DOXYGEN_ONLY
		//! @}

		//! @name Iteration
		//! @{

#ifdef DOXYGEN_ONLY
		//! @brief Iterate over all table elements.
		//! @tparam IterationFunction A <b>bool</b>( @ref Valref key, @ref Valref value) function/functor.
		//! @param ifunc Function that processes key-value pairs. Return value tells wheter to continue(<b>true</b>) or break(<b>false</b>) the iteration.
		//! @note During iteration any existing Valset will be blocked.
		template<typename IterationFunction>
		size_t iterate(IterationFunction ifunc) const;
#else	// Not DOXYGEN_ONLY
		template<typename IterationFunction> typename std::enable_if<
			std::is_void<
				decltype(std::declval<IterationFunction>()(std::declval<Valref>(), std::declval<Valref>()))
			>::value,
		size_t>::type iterate(IterationFunction ifunc) const;

		template<typename IterationFunction> typename std::enable_if<
			std::is_convertible<
				bool,
				decltype(std::declval<IterationFunction>()(std::declval<Valref>(), std::declval<Valref>()))
			>::value,
		size_t>::type iterate(IterationFunction ifunc) const;

		template<typename T = void>
		size_t iterate(...) const
		{
			static_assert(!std::is_same<T, T>::value, "The callback must be void(Valref, Valref) or bool(Valref, Valref)");
			return 0;
		}
#endif	// DOXYGEN_ONLY
		//! @}


	private:
		//! Check if given value is indeed a table. Throws an exception if it's not.
		void checkTableness() const
		{
			if(!Anchor.is<Table>())
				throw std::runtime_error("Lua: attempt to create a table object from non-table value");
		}

		// Iteration helpers
		static void beginIteration(Context& S);
		static bool nextIteration(const Value& v);

		// data
		Value Anchor;

		// populate utilities
		template<typename VT1, typename ... OVT> Table& addvalues(int idx, VT1&& val, OVT&& ... otherValues)
		{
			(*this)[idx] = std::forward<VT1>(val);
			return this->addvalues(idx + 1, std::forward<OVT>(otherValues)...);
		}

		Table& addvalues(int)
		{
			return *this;
		}


		template<typename K, typename V, typename ... OKV> Table& addrecords(K&& key, V&& value, OKV&& ... okv)
		{
			(*this)[std::forward<K>(key)] = std::forward<V>(value);
			return addrecords(std::forward<OKV>(okv)...);
		}

		Table& addrecords ()
		{
			return *this;
		}

	public:

		//! @name Indexation
		//! @{

		//! @brief Accessor for raw reads/writes (skip metatable).
		//! @details This member is indexed with any suitable value just like Valref.
		RawAccessor raw;
		//! @}

		//! @name Pre-filled table creation
		//! @{

#ifdef DOXYGEN_ONLY
		//! @brief Create filled array table.
		//! @details This functions accepts arbitrary number of Valobj values that fill the table under sequential numeric indices starting with 1.
		//! @note Without inlining and other optimizations, excessive amount of arguments could lead to stack overflow (native stack, not Lua stack).
		//! @note @ref Valset "Value sets" and call results will be expanded.
		template<typename ... ValueTypes> Temporary array(Context& context, ValueType ... values);
		//! @brief Create filled table filled with records.
		//! @details This functions accepts arbitrary even number of Valobj values. Each pair is interpreted as key and value.
		//! Repeated keys cause overwrite of corresponding value, no error is generated.
		//! @note Without inlining and other optimizations, excessive amount of arguments could lead to stack overflow (native stack, not Lua stack).
		//! @note @ref Valset "Value sets" cannot be used with this function, and call results will be treated as a single value (no expansion).
		template<typename ... KeyValueTypes> Temporary records(Context& context, KeyValueTypes ... keyValuePairs);
#else	// Not DOXYGEN_ONLY
		template<typename ... VT>
		static _::Lazy<_::lazyTableArray<VT...>> array(Context& S, VT&& ... values)
		{
			return _::Lazy<_::lazyTableArray<VT...>>(S, std::forward<VT>(values)...);
		}

		template<typename ... KV>
		static _::Lazy<_::lazyTableRecords<KV...>> records(Context& S, KV&& ... pairs)
		{
			static_assert((sizeof ... (KV) & 1) == 0, "Odd number of arguments is incorrect. Must provide key-value pairs.");
			return _::Lazy<_::lazyTableRecords<KV...>>(S, std::forward<KV>(pairs)...);
		}
#endif	// DOXYGEN_ONLY
		//! @}

	};


	//! @name Comparisons (table needs only equality checks).
	//! @{

#ifdef DOXYGEN_ONLY
	//! @relates lua::Table
	bool operator == (Valobj lhs, const Table& rhs);
	//! @relates lua::Table
	bool operator != (Valobj lhs, const Table& rhs);
#else	// Not DOXYGEN_ONLY
	template<typename ValueType>
	typename std::enable_if<!_::IsAnchor<ValueType>::value, bool>::type operator == (ValueType&& lhs, const Table& rhs)
	{
		return rhs == std::forward<ValueType>(lhs);
	}

	template<typename ValueType>
	typename std::enable_if<!_::IsAnchor<ValueType>::value, bool>::type operator != (ValueType&& lhs, const Table& rhs)
	{
		return rhs != std::forward<ValueType>(lhs);
	}

#endif	// DOXYGEN_ONLY
	//! @}
}


#endif // LUA_TABLE_HPP_INCLUDED
