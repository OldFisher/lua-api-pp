/*
* This file is part of Lua API++ library (https://github.com/OldFisher/lua-api-pp)
* distributed under MIT License (http://opensource.org/licenses/MIT).
* See license.txt for details.
* (c) 2014 OldFisher
*/

#ifndef LUA_INDEXER_HPP_INCLUDED
#define LUA_INDEXER_HPP_INCLUDED


namespace lua {

	//! @cond
	namespace _ {


//##############################################################################


		class lazyConstIndexerUtils final {
			template<typename> friend class lazyConstIndexer;
#if(LUAPP_API_VERSION >= 53)
			friend class lazyConstIntIndexer;
#endif	// V53+
		private:
			static void extractValue(lua_State* L, int tableref) noexcept;
			static void writeValue(lua_State* L, int tableref) noexcept;
#if(LUAPP_API_VERSION >= 53)
			static void extractValuei(lua_State* L, int tableref, long long index) noexcept;
			static void writeValuei(lua_State* L, int tableref, long long index) noexcept;
#endif	// V53+
		};


		//! Constant-value indexer (the table is not a temprorary)
		template<typename IndexType>
#ifdef LUAPP_NONDISCARDABLE_INDEX
		class lazyConstIndexer final: public lazyPolicyNondiscardable {
			friend class ::lua::_::lazyPolicy;
			friend class ::lua::_::lazyPolicyNondiscardable;
#else
		class lazyConstIndexer final: public ::lua::_::lazyPolicy {
#endif	// LUAPP_NONDISCARDABLE_INDEX

			friend class Valref;
			template<typename> friend class ::lua::_::Lazy;

		public:
			lazyConstIndexer(lazyConstIndexer<IndexType>&& src) noexcept:
				tableref(src.tableref),
				idxlazy(std::move(src.idxlazy))
			{
			}

		private:

			lazyConstIndexer(Context& S, int tableIndex, IndexType&& index) noexcept:
				tableref(tableIndex),
				idxlazy(S, std::move(index))
			{
			}

			lazyConstIndexer(Context& S, int tableIndex, const IndexType& index) noexcept:
				tableref(tableIndex),
				idxlazy(S, index)
			{
			}

			void push(Context& S);

			void pushSingle(Context& S)
			{
				push(S);
			}

			// Push value and discard it
#ifdef LUAPP_NONDISCARDABLE_INDEX
			void onDestroy(Context& S)
			{
				lazyPolicyNondiscardable::onDestroySingle(S, *this);
#else
			void onDestroy(Context&)
			{
				idxlazy.onDestroy();
#endif	// LUAPP_NONDISCARDABLE_INDEX
			}

			void moveout() noexcept
			{
#ifdef LUAPP_NONDISCARDABLE_INDEX
				Pushed = true;
#endif	// LUAPP_NONDISCARDABLE_INDEX
				idxlazy.moveout();
			}

			template<typename ValueType>
			void assign(Context& S, ValueType&& val);

		private:
			// data
			const int tableref;
			Lazy<lazyImmediateValue<IndexType>> idxlazy;
		};



#if(LUAPP_API_VERSION >= 53)
		//! Constant-value indexer integer specialization
#ifdef LUAPP_NONDISCARDABLE_INDEX
		class lazyConstIntIndexer final: public lazyPolicyNondiscardable {
			friend class ::lua::_::lazyPolicy;
			friend class ::lua::_::lazyPolicyNondiscardable;
#else
		class lazyConstIntIndexer final: public ::lua::_::lazyPolicy {
#endif	// LUAPP_NONDISCARDABLE_INDEX

			friend class Valref;
			template<typename> friend class ::lua::_::Lazy;

		public:
			lazyConstIntIndexer(lazyConstIntIndexer&& src) noexcept:
				tableref(src.tableref),
				index(src.index)
			{
			}

		private:

			lazyConstIntIndexer(Context&, int tableIndex, long long index_) noexcept:
				tableref(tableIndex),
				index(index_)
			{
			}

			void push(Context& S);

			void pushSingle(Context& S)
			{
				push(S);
			}

			// Push value and discard it
#ifdef LUAPP_NONDISCARDABLE_INDEX
			void onDestroy(Context& S)
			{
				lazyPolicyNondiscardable::onDestroySingle(S, *this);
			}
#else
			void onDestroy(Context&)
			{
			}
#endif	// LUAPP_NONDISCARDABLE_INDEX

			void moveout() noexcept
			{
#ifdef LUAPP_NONDISCARDABLE_INDEX
				Pushed = true;
#endif	// LUAPP_NONDISCARDABLE_INDEX
			}

			template<typename ValueType>
			void assign(Context& c, ValueType&& val);

		private:
			// data
			const int tableref;
			const long long index;
		};
#endif	// V53+

//##############################################################################


		class globalIndexer;


		//! Global value indexer policy (only string indices are allowed)
		class lazyGlobalIndexer final: public lazyPolicy {

			friend class _::globalIndexer;
			template<typename> friend class _::Lazy;

		public:
			lazyGlobalIndexer(lazyGlobalIndexer&& src) noexcept:
				nameref(src.nameref)
			{
			}

		private:

			lazyGlobalIndexer(Context&, const char* name) noexcept:
				nameref(name)
			{
			}

			void push(Context& S);

			void pushSingle (Context& S)
			{
				push(S);
			}

			template<typename ValueType>
			void assign(Context& S, ValueType&& val);

			// data
			const char* const nameref;

			// utils
			void writeValue(lua_State* L) noexcept;

		};



		//! Global value access object
		class globalIndexer final: public noNew, public nonCopyable {

			friend class ::lua::Context;

		public:

			Lazy<lazyGlobalIndexer> operator [] (const char* index) const noexcept
			{
				return Lazy<lazyGlobalIndexer>(S, index);
			}

			//! String literal specialization (to provide exact match and
			//! avoid ambiguity with std::string version)
			template<size_t N>
			Lazy<lazyGlobalIndexer> operator [] (const char (&index)[N]) const noexcept
			{
				return Lazy<lazyGlobalIndexer>(S, index);
			}

			Lazy<lazyGlobalIndexer> operator [] (const std::string& index) const noexcept
			{
				return Lazy<lazyGlobalIndexer>(S, index.c_str());
			}


			//! Modify several values
			template<typename ValueType, typename ... OtherTypes>
			void set(const char* name, ValueType&& value, OtherTypes&& ... others);


		private:
			// construction
			globalIndexer(Context& s) noexcept:
				S(s)
			{
			}

			// data
			Context& S;

			// utils
			void set() const noexcept
			{
			}
		};


//##############################################################################

		//! Upvalue access object
		class uvIndexer final: public nonCopyable, public noNew {
			friend class ::lua::Context;

		public:
			const Valref operator [] (size_t index) const noexcept
			{
				return const_cast<uvIndexer*>(this)->operator [] (index);
			}

			Valref operator [] (size_t index)  noexcept;

		private:
			explicit uvIndexer(Context& s):
				S(s)
			{
			}

			Context& S;
		};

//##############################################################################


		class lazyTempIndexerUtils final {
			template<typename, typename> friend class lazyTempIndexer;
#if(LUAPP_API_VERSION >= 53)
			template<typename> friend class lazyTempIntIndexer;
#endif	// V53+
		private:
			static void extractValue(lua_State* L) noexcept;
			static void writeValue(lua_State* L) noexcept;
#if(LUAPP_API_VERSION >= 53)
			static void extractValuei(lua_State* L, long long index) noexcept;
			static void writeValuei(lua_State* L, long long index) noexcept;
#endif	// V53+
		};


		//! Temp-value indexer (the table is discarded after read-write operation)
		template<typename TableType, typename IndexType>
#ifdef LUAPP_NONDISCARDABLE_INDEX
		class lazyTempIndexer final: public lazyPolicyNondiscardable{
			friend class ::lua::_::lazyPolicyNondiscardable;
			friend class ::lua::_::lazyPolicy;
#else
		class lazyTempIndexer final: public lazyPolicy{
#endif	// LUAPP_NONDISCARDABLE_INDEX
			template<typename> friend class ::lua::_::Lazy;

			typedef _::Lazy<_::lazyImmediateValue<TableType>> tlazy;
			typedef _::Lazy<_::lazyImmediateValue<IndexType>> ilazy;

		public:
			lazyTempIndexer(lazyTempIndexer<TableType, IndexType>&&) noexcept = default;

		private:

			lazyTempIndexer(Context& S, TableType&& table, IndexType&& index) noexcept:
				tablelazy(S, std::move(table)),
				idxlazy(S, std::move(index))
			{
			}

			lazyTempIndexer(Context& S, const TableType& table, IndexType&& index) noexcept:
				tablelazy(S, table),
				idxlazy(S, std::move(index))
			{
			}

			lazyTempIndexer(Context& S, TableType&& table, const IndexType& index) noexcept:
				tablelazy(S, std::move(table)),
				idxlazy(S, index)
			{
			}

			lazyTempIndexer(Context& S, const TableType& table, const IndexType& index) noexcept:
				tablelazy(S, table),
				idxlazy(S, index)
			{
			}

			void push(Context& S);

			void pushSingle(Context& S)
			{
				push(S);
			}

			// Push value and discard it
#ifdef LUAPP_NONDISCARDABLE_INDEX
			void onDestroy(Context& S)
			{
				lazyPolicyNondiscardable::onDestroySingle(S, *this);

#else
			void onDestroy(Context&)
			{
				tablelazy.onDestroy();
				idxlazy.onDestroy();
#endif	// LUAPP_NONDISCARDABLE_INDEX
			}


			void moveout() noexcept
			{
#ifdef LUAPP_NONDISCARDABLE_INDEX
				Pushed = true;
#endif	// LUAPP_NONDISCARDABLE_INDEX
				tablelazy.moveout();
				idxlazy.moveout();
			}

			template<typename ValueType>
			void assign(Context& S, ValueType&& val);


			// data
			tlazy tablelazy;
			ilazy idxlazy;

		};



#if(LUAPP_API_VERSION >= 53)
		//! Temp-value indexer for integer indices
		template<typename TableType>
#ifdef LUAPP_NONDISCARDABLE_INDEX
		class lazyTempIntIndexer final: public lazyPolicyNondiscardable{
			friend class ::lua::_::lazyPolicyNondiscardable;
			friend class ::lua::_::lazyPolicy;
#else
		class lazyTempIntIndexer final: public lazyPolicy{
#endif	// LUAPP_NONDISCARDABLE_INDEX
			template<typename> friend class ::lua::_::Lazy;

			typedef _::Lazy<_::lazyImmediateValue<TableType>> tlazy;

		public:
			lazyTempIntIndexer(lazyTempIntIndexer<TableType>&& src) noexcept:
				tableLazy(std::move(src.tableLazy)),
				index(src.index)
			{
			}

		private:

			lazyTempIntIndexer(Context& c, TableType&& table, long long index_) noexcept:
				tableLazy(c, std::move(table)),
				index(index_)
			{
			}

			lazyTempIntIndexer(Context& c, const TableType& table, long long index_) noexcept:
				tableLazy(c, table),
				index(index_)
			{
			}

			void push(Context& c);

			void pushSingle(Context& c)
			{
				push(c);
			}

			// Push value and discard it
#ifdef LUAPP_NONDISCARDABLE_INDEX
			void onDestroy(Context& c)
			{
				lazyPolicyNondiscardable::onDestroySingle(c, *this);

#else
			void onDestroy(Context&)
			{
				tableLazy.onDestroy();
#endif	// LUAPP_NONDISCARDABLE_INDEX
			}


			void moveout() noexcept
			{
#ifdef LUAPP_NONDISCARDABLE_INDEX
				Pushed = true;
#endif	// LUAPP_NONDISCARDABLE_INDEX
				tableLazy.moveout();
			}

			template<typename ValueType>
			void assign(Context& c, ValueType&& val);


			// data
			tlazy tableLazy;
			const long long index;

		};
#endif	// V53+


//#####################  lazyMT  ###############################################


		class lazyMT final: public lazyPolicy {
			template<typename> friend class _::Lazy;
			template<typename> friend class lazyMtTemp;

		public:
			lazyMT(lazyMT&& src) noexcept:
				Index(src.Index)
			{
			}

		private:
			lazyMT(Context&, const int index) noexcept:
				Index(index)
			{
			}

			static void doPush(Context& S, int index) noexcept;

			void push(Context& S)
			{
				doPush(S, Index);
			}

			void pushSingle(Context& S)
			{
				push(S);
			}

			template<typename TableType>
			void assign(Context& S, TableType&& t);
			static void doAssign(lua_State* L, int index) noexcept;

			// data
			const int Index;
		};



		template<typename Policy>
		class lazyMtTemp final: public lazyPolicy {
			template<typename> friend class _::Lazy;

		public:
			lazyMtTemp(lazyMtTemp<Policy>&&) noexcept = default;

		private:
			lazyMtTemp(Context&, Lazy<Policy>&& src):
				srcLazy(std::move(src))
			{
			}

			void push(Context& S);

			void pushSingle(Context& S)
			{
				push(S);
			}

			template<typename TableType>
			void assign(Context& S, TableType&& t);

			// data
			Lazy<Policy> srcLazy;
		};



//##############################  Linked  ######################################
#if(LUAPP_API_VERSION >= 53)

		class lazyLinked final: public lazyPolicy {
			template<typename> friend class _::Lazy;
			template<typename> friend class lazyLinkedTemp;

		public:
			lazyLinked(lazyLinked&& src) noexcept:
				index(src.index)
			{
			}

		private:
			lazyLinked(Context&, int index_) noexcept:
				index(index_) {}

			static void doPush(Context& c, int index) noexcept;

			void push(Context& c)
			{
				doPush(c, index);
			}

			void pushSingle(Context& c)
			{
				push(c);
			}

			template<typename ValueType>
			void assign(Context& S, ValueType&& val);
			static void doAssign(lua_State* L, int index) noexcept;

			// data
			const int index;
		};



		template<typename Policy>
		class lazyLinkedTemp final: public lazyPolicy {
			template<typename> friend class _::Lazy;

		public:
			lazyLinkedTemp(lazyLinkedTemp<Policy>&&) noexcept = default;

		private:
			lazyLinkedTemp(Context&, Lazy<Policy>&& src):
				srcLazy(std::move(src))
			{
			}

			void push(Context& c);

			void pushSingle(Context& c)
			{
				push(c);
			}

			template<typename ValueType>
			void assign(Context& c, ValueType&& val);

			// data
			Lazy<Policy> srcLazy;
		};
#endif	// V53+



//#####################  lazyRawIndexer  #######################################
		template<typename> class lazyRawIndexer;

		class lazyRawIndexerUtils {
			template<typename> friend class _::lazyRawIndexer;
		private:
			static void read(lua_State* L, int tIdx);
			static void write(lua_State* L, int tIdx);
		};

		//! Policy for raw table access.
		template<typename IndexType>
		class lazyRawIndexer final: public _::lazyPolicy {
			template<typename> friend class _::Lazy;
			typedef _::Lazy<_::lazyImmediateValue<IndexType>> LazyIdx;

		public:
			lazyRawIndexer(lazyRawIndexer<IndexType>&& src) noexcept:
				tableref(src.tableref),
				idxlazy(std::move(src.idxlazy))
			{
			}

		private:

			lazyRawIndexer(Context& S, int tRef, IndexType&& i) noexcept:
				tableref(tRef),
				idxlazy(S, std::move(i))
			{
			}

			lazyRawIndexer(Context& S, int tRef, const IndexType& i) noexcept:
				tableref(tRef),
				idxlazy(S, i)
			{
			}

			void push(Context& S);

			void pushSingle(Context& S)
			{
				push(S);
			}

			void onDestroy(Context& S)
			{
				idxlazy.onDestroy();
			}

			void moveout() noexcept
			{
				idxlazy.moveout();
			}

			template<typename ValueType>
			void assign(Context& S, ValueType&& v);

			// data
			const int tableref;
			LazyIdx idxlazy;
		};


		//! Int specialization
		template<>
		class lazyRawIndexer<int> final: public _::lazyPolicy {
			template<typename> friend class _::Lazy;

		public:
			lazyRawIndexer(lazyRawIndexer<int>&& src) noexcept:
				tableref(src.tableref),
				Idx(src.Idx)
			{
			}

		private:

			lazyRawIndexer(Context&, int tRef, int i) noexcept:
				tableref(tRef),
				Idx(i)
			{
			}

			void push(Context& S);

			void pushSingle(Context& S)
			{
				push(S);
			}

			template<typename ValueType>
			void assign(Context& S, ValueType&& v);

			// data
			const int tableref;
			const int Idx;

			static void set(lua_State* L, int tIdx, int idx);
		};


		//! Pointer specialization
		template<>
		class lazyRawIndexer<LightUserData> final: public _::lazyPolicy {
			template<typename> friend class _::Lazy;

		public:
			lazyRawIndexer(lazyRawIndexer<LightUserData>&& src) noexcept:
				tableref(src.tableref),
				Idx(src.Idx)
			{
			}

		private:

			lazyRawIndexer(Context&, int tRef, LightUserData i) noexcept:
				tableref(tRef),
				Idx(i)
			{
			}

			void push(Context& S);

			void pushSingle(Context& S)
			{
				push(S);
			}

			template<typename ValueType>
			void assign(Context& S, ValueType&& v);

			// data
			const int tableref;
			const LightUserData Idx;

			static void set(lua_State* L, int tIdx, LightUserData idx);
		};



//#####################  lazyExtConstUpvalue  ##################################


		class lazyExtConstUpvalue final: public _::lazyPolicy{
			template<typename> friend class _::Lazy;
			template<typename> friend class lazyExtTempUpvalue;

		private:
			lazyExtConstUpvalue(Context&, int closureRef_, size_t index_):
				closureRef(closureRef_),
				index(index_)
			{
			}

			void push(Context&);

			void pushSingle(Context& c)
			{
				push(c);
			}

			template<typename ValueType> void assign(Context& c, ValueType&& v);

			const int closureRef;
			const size_t index;

			static void doPush(lua_State* s, int closureRef, size_t index);
			static void doAssign(lua_State* s, int closureRef, size_t index);
		};


//#####################  lazyExtTempUpvalue  ##################################


		template<typename Policy> class lazyExtTempUpvalue final: public _::lazyPolicy {
			template<typename> friend class _::Lazy;
		private:
			lazyExtTempUpvalue(Context&, Lazy<Policy>&& src, size_t index_):
				srcLazy(std::move(src)),
				index(index_)
			{
			}

			void push(Context&);

			void pushSingle(Context& c)
			{
				push(c);
			}

			template<typename ValueType> void assign(Context& c, ValueType&& v);

			Lazy<Policy> srcLazy;
			size_t index;
		};


	}


	//! @endcond

}

#endif // LUA_INDEXER_HPP_INCLUDED
