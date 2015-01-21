/*
* This file is part of Lua API++ library (https://github.com/OldFisher/lua-api-pp)
* distributed under MIT License (http://opensource.org/licenses/MIT).
* See license.txt for details.
* (c) 2014 OldFisher
*/

#ifndef LUA_LAZY_HPP_INCLUDED
#define LUA_LAZY_HPP_INCLUDED



namespace lua{

	//! @cond
	class Context;
	class Value;
	class Valset;



	namespace _ {

		class lazyPolicy;
		template<typename...> class lazySeries;
		template<typename> class lazyImmediateValue;
		template<typename>  class lazyConstIndexer;
		template<typename> class lazyExtTempUpvalue;
		class lazyGlobalIndexer;
		class globalIndexer;
		class lazyUpvalueIndexer;
		class uvIndexer;
		template<typename, typename>  class lazyTempIndexer;
		template<typename, typename...> class lazyCall;
		template<typename, typename...> class lazyPCall;
		template<typename...> class lazyClosure;
#if(LUAPP_API_VERSION >= 52)
		template<typename> class lazyLenTemp;
#endif	// V52+
		template<typename> class lazyMtTemp;
#if(LUAPP_API_VERSION >= 53)
		template<typename>  class lazyTempIntIndexer;
		template<typename> class lazyLinkedTemp;
#endif	// V53+
		template<typename Policy> Context& extractContext(const Lazy<Policy>&) noexcept;
		// Operations
		class lazyConcatSelector;
		template<typename, typename> class lazyConcat;
#if(LUAPP_API_VERSION >= 52)
		template<typename, typename, _::Arithmetics> class lazyArithmetics;
#endif	// V52+

		template<typename T> void moveout(const T&) noexcept;
		template<typename> class Lazy;
		template<typename Policy> void moveout(Lazy<Policy>&) noexcept;


		//! A class that pushes value(s) on the stack when told to.
		//! Temporary, behaves as Lua value.
		template<typename Policy>
		class Lazy final:
			public noNew,
			public returnableByFriends
		{

			friend class ::lua::Context;
			friend class ::lua::Valset;
			friend class ::lua::Table;


			// must be friendly to all policies that may use other lazies
			template<typename> friend class ::lua::_::Lazy;
			friend class ::lua::_::lazyPolicy;
			friend class ::lua::_::lazyImmediateValue<Lazy<Policy>>;
			template<typename...> friend class ::lua::_::lazySeries;
			template<typename> friend class ::lua::_::lazyConstIndexer;
			friend class ::lua::_::lazyGlobalIndexer;
			friend class ::lua::_::lazyUpvalueIndexer;
			template<typename, typename> friend class ::lua::_::lazyTempIndexer;
			template<typename, typename ...> friend class ::lua::_::lazyCall;
			template<typename, typename ...> friend class ::lua::_::lazyPCall;
			template<typename...> friend class ::lua::_::lazyClosure;
			template<typename P, typename ... Args> friend Lazy<P> makeLazy(Args&& ...) noexcept;

			// Operations
			friend class ::lua::_::lazyConcatSelector;
			template<typename, typename> friend class ::lua::_::lazyConcat;
#if(LUAPP_API_VERSION >= 52)
			template<typename, typename, lua::_::Arithmetics> friend class ::lua::_::lazyArithmetics;
			template<typename, lua::_::Arithmetics> friend class ::lua::_::lazyArithmeticsUnary;
			friend class ::lua::_::lazyLenTemp<Policy>;
#endif	// V52+
			friend class ::lua::_::lazyMtTemp<Policy>;
#if(LUAPP_API_VERSION >= 53)
			friend class ::lua::_::lazyLinkedTemp<Policy>;
			template<typename> friend class ::lua::_::lazyTempIntIndexer;
#endif	// V53+
			friend Context& lua::_::extractContext<Policy>(const Lazy<Policy>&) noexcept;

			// must be friendly to all objects that can create Lazy
			friend class ::lua::Valref;
			friend class ::lua::Value;
			friend class ::lua::_::globalIndexer;
			friend class ::lua::_::uvIndexer;

			// utility function(s)
			friend void ::lua::_::moveout<Policy> (Lazy<Policy>&) noexcept;

		public:

			// Valref interface
			//! @name Type info and conversion
			//! @{

			//! Explicit conversion to supported types.
			template<typename T> T cast() &&
			{
				return toTemporary().template cast<T>();
			}

			//! Unsafe conversion to supported types.
			template<typename T> T to() &&
			{
				return toTemporary().template to<T>();
			}

			//! Safe conversion to supported types.
			template<typename T> T to(const T& backupValue) &&
			{
				return toTemporary().template to<T>(backupValue);
			}

#if(LUAPP_API_VERSION <= 52)
			//! Safe conversion to supported types (deprecated).
			template<typename T> T optcast(const T& backupValue = T()) &&
			{
				return toTemporary().template optcast<T>(backupValue);
			}
#endif	// V52-

			//! Check if the value is of given type or convertible.
			template<typename T> bool is() &&
			{
				return toTemporary().template is<T>();
			}

			ValueType type() &&
			{
				return toTemporary().type();
			}

			operator bool () &&
			{
				return toTemporary();
			}

			operator int () &&
			{
				return toTemporary();
			}

			operator unsigned int() &&
			{
				return toTemporary();
			}

			operator long long () &&
			{
				return toTemporary();
			}

			operator unsigned long long() &&
			{
				return toTemporary();
			}

			operator float() &&
			{
				return toTemporary();
			}

			operator double() &&
			{
				return toTemporary();
			}

			operator const char* () &&
			{
				return toTemporary();
			}

			operator std::string () &&
			{
				return toTemporary();
			}

			operator CFunction () &&
			{
				return toTemporary();
			}

			operator LightUserData() &&
			{
				return toTemporary();
			}



			//! @}

			//! @name Modifying (or potentially modifying) operations
			//! @{

			//! Assignment is enabled only if the policy has void assign(Context&, const ValueType&)
			//! Policies have no assignment by default. Indexers do have it.
			template <typename ValueType> void operator = (ValueType&& val) &&
			{
				static_assert(!std::is_same<::lua::Table&&, decltype(std::forward<ValueType>(val))>::value, "Using anonymous Table object");
				static_assert(!std::is_same<::lua::Value&&, decltype(std::forward<ValueType>(val))>::value, "Using anonymous Value object");
				static_assert(!std::is_same<::lua::Valset&&, decltype(std::forward<ValueType>(val))>::value, "Using anonymous Valset object");
				policy.assign(S, std::forward<ValueType>(val));
			}


			//! Indexation.
			template <typename IndexType>
			Lazy<lazyTempIndexer<Lazy<Policy>, typename std::decay<IndexType>::type>> operator [] (IndexType&& index) && noexcept
			{
				return Lazy<lazyTempIndexer<Lazy<Policy>, typename std::decay<IndexType>::type>>(S, std::move(*this), std::forward<IndexType>(index));
			}

#if(LUAPP_API_VERSION >= 53)
			Lazy<lazyTempIntIndexer<Lazy<Policy>>> operator [] (int index) && noexcept
			{
				return Lazy<lazyTempIntIndexer<Lazy<Policy>>>(S, std::move(*this), index);
			}

			Lazy<lazyTempIntIndexer<Lazy<Policy>>> operator [] (long long index) && noexcept
			{
				return Lazy<lazyTempIntIndexer<Lazy<Policy>>>(S, std::move(*this), index);
			}

			Lazy<lazyTempIntIndexer<Lazy<Policy>>> operator [] (unsigned index) && noexcept
			{
				return Lazy<lazyTempIntIndexer<Lazy<Policy>>>(S, std::move(*this), index);
			}

			Lazy<lazyTempIntIndexer<Lazy<Policy>>> operator [] (unsigned long long index) && noexcept
			{
				return Lazy<lazyTempIntIndexer<Lazy<Policy>>>(S, std::move(*this), index);
			}

#endif	// V53+

			//! Upvalue access
			Lazy<lazyExtTempUpvalue<Policy>> upvalue(size_t index) && noexcept
			{
				return Lazy<lazyExtTempUpvalue<Policy>>(S, std::move(*this), index);
			}

			//! Read upvalues.
			Valset getUpvalues() &&;

			//! Closure properties
#if(LUAPP_API_VERSION > 51)
			ClosureInfo getClosureInfo() &&;
#else
			size_t getClosureInfo() &&;
#endif

			//! Generic call.
			template<typename ... Args>
			Lazy<lazyCall<Lazy<Policy>, Args...>> operator () (Args&& ... args) && noexcept
			{
				return Lazy<lazyCall<Lazy<Policy>, Args...>>(S, std::move(*this), std::forward<Args>(args)...);
			}

			//! Call function
			template<typename ... Args>
			Lazy<lazyCall<Lazy<Policy>, Args...>> call(Args&& ... args) && noexcept
			{
				return Lazy<lazyCall<Lazy<Policy>, Args...>>(S, std::move(*this), std::forward<Args>(args)...);
			}

			//! Protected call function
			template<typename ... Args>
			Lazy<lazyPCall<Lazy<Policy>, Args...>> pcall(Args&& ... args) && noexcept
			{
				return Lazy<lazyPCall<Lazy<Policy>, Args...>>(S, std::move(*this), std::forward<Args>(args)...);
			}
			//! @}

			//! @name Comparisons
			//! @{


			template<typename ValueType> typename std::enable_if<!IsLazy<ValueType>::value, bool>::type operator == (ValueType&& rhs) &&
			{
				return toTemporary().Valref::operator == (std::forward<ValueType>(rhs));
			}


			template<typename ValueType> typename std::enable_if<!IsLazy<ValueType>::value, bool>::type operator != (ValueType&& rhs) &&
			{
				return toTemporary().Valref::operator != (std::forward<ValueType>(rhs));
			}


			template<typename ValueType> typename std::enable_if<!IsLazy<ValueType>::value, bool>::type operator < (ValueType&& rhs) &&
			{
				return toTemporary().Valref::operator < (std::forward<ValueType>(rhs));
			}


			template<typename ValueType> typename std::enable_if<!IsLazy<ValueType>::value, bool>::type operator > (ValueType&& rhs) &&
			{
				return toTemporary().Valref::operator > (std::forward<ValueType>(rhs));
			}


			template<typename ValueType> typename std::enable_if<!IsLazy<ValueType>::value, bool>::type operator <= (ValueType&& rhs) &&
			{
				return toTemporary().Valref::operator <= (std::forward<ValueType>(rhs));
			}


			template<typename ValueType> typename std::enable_if<!IsLazy<ValueType>::value, bool>::type operator >= (ValueType&& rhs) &&
			{
				return toTemporary().Valref::operator >= (std::forward<ValueType>(rhs));
			}


			template<typename Policy2>
			bool operator == (Lazy<Policy2>&& rhs) &&
			{
				return toTemporary().Valref::operator == (std::move(rhs));
			}


			template<typename Policy2>
			bool operator != (Lazy<Policy2>&& rhs) &&
			{
				return toTemporary().Valref::operator != (std::move(rhs));
			}


			template<typename Policy2>
			bool operator < (Lazy<Policy2>&& rhs) &&
			{
				return toTemporary().Valref::operator < (std::move(rhs));
			}


			template<typename Policy2>
			bool operator > (Lazy<Policy2>&& rhs) &&
			{
				return toTemporary().Valref::operator > (std::move(rhs));
			}


			template<typename Policy2>
			bool operator <= (Lazy<Policy2>&& rhs) &&
			{
				return toTemporary().Valref::operator <= (std::move(rhs));
			}


			template<typename Policy2>
			bool operator >= (Lazy<Policy2>&& rhs) &&
			{
				return toTemporary().Valref::operator >= (std::move(rhs));
			}


			//! @}

			//! @name Operations
			//! @{

			//! Length
#if(LUAPP_API_VERSION >= 52)
			Lazy<lazyLenTemp<Policy>> len() &&;
#else	// V51-
			size_t len() &&
			{
				return toTemporary().len();
			}
#endif	// V52+

			//! Metatable read/write
			Lazy<lazyMtTemp<Policy>> mt() &&;

#if(LUAPP_API_VERSION >= 53)
			//! Linked value read/write
			Lazy<lazyLinkedTemp<Policy>> linked() &&;
#endif	// V53+

			~Lazy()
			{
				onDestroy();
			}

			Lazy(Lazy<Policy>&& src) noexcept:
				S(src.S),
				policy(std::move(src.policy))
			{
				src.moveout();
			}

			Lazy<Policy>& operator = (Lazy<Policy>&&) noexcept = delete;

		private:
			Context& S;
			Policy policy;

			// friend ops
			template<typename ... Args> Lazy(Context& s, Args&& ... args) noexcept:
				S(s),
				policy(S, std::forward<Args>(args)...)
			{
			}

			// stack ops
			//! Push all the values on the stack
			void push()
			{
				policy.push(S);
			}

			//! Push exactly one value on the stack
			void pushSingle()
			{
				policy.pushSingle(S);
			}

			//! Push-only routine to collect concatenation arguments
			void pushOnly()
			{
				policy.pushOnly(S);
			}

			//! Discard the value
			void onDestroy()
			{
				policy.onDestroy(S);
			}

			//! Move the value out
			void moveout() noexcept
			{
				policy.moveout();
			}

			// other
			//! Conversion to tValue, move-out when applicable
			//! Covers value conversions and comparisons
			tValue toTemporary ();



		};



		template<typename Policy, typename ValueType> typename std::enable_if<!IsLazy<ValueType>::value && !IsAnchor<ValueType>::value, bool>::type operator == (ValueType&& lhs, _::Lazy<Policy>&& rhs)
		{
			return std::move(rhs).template operator ==  <ValueType> (std::forward<ValueType>(lhs));
		}

		template<typename Policy, typename ValueType> typename std::enable_if<!IsLazy<ValueType>::value && !IsAnchor<ValueType>::value, bool>::type operator != (ValueType&& lhs, _::Lazy<Policy>&& rhs)
		{
			return std::move(rhs).template operator != <ValueType> (std::forward<ValueType>(lhs));
		}

		template<typename Policy, typename ValueType> typename std::enable_if<!IsLazy<ValueType>::value && !IsAnchor<ValueType>::value, bool>::type operator < (ValueType&& lhs, _::Lazy<Policy>&& rhs)
		{
			return std::move(rhs).template operator >  <ValueType> (std::forward<ValueType>(lhs));
		}

		template<typename Policy, typename ValueType> typename std::enable_if<!IsLazy<ValueType>::value && !IsAnchor<ValueType>::value, bool>::type operator > (ValueType&& lhs, _::Lazy<Policy>&& rhs)
		{
			return std::move(rhs).template operator <  <ValueType> (std::forward<ValueType>(lhs));
		}

		template<typename Policy, typename ValueType> typename std::enable_if<!IsLazy<ValueType>::value && !IsAnchor<ValueType>::value, bool>::type operator <= (ValueType&& lhs, _::Lazy<Policy>&& rhs)
		{
			return std::move(rhs).template operator >=  <ValueType> (std::forward<ValueType>(lhs));
		}

		template<typename Policy, typename ValueType> typename std::enable_if<!IsLazy<ValueType>::value && !IsAnchor<ValueType>::value, bool>::type operator >= (ValueType&& lhs, _::Lazy<Policy>&& rhs)
		{
			return std::move(rhs).template operator <=  <ValueType> (std::forward<ValueType>(lhs));
		}






		//! Lazy policy concept
		class lazyPolicy {
			template<typename> friend class _::Lazy;

		public:
			lazyPolicy(lazyPolicy&&) noexcept = default;

		protected:

			lazyPolicy() noexcept
			{
			}

			void push(Context&);        //!< Push all values
			void pushSingle(Context&);  //!< Push exactly one value

			void onDestroy(Context&)
			{
			}

			void moveout() noexcept
			{
			}

		};



		//! Lazy policy for non-discardable expressions (calculated even if result is not used)
		class lazyPolicyNondiscardable: public lazyPolicy {
			template<typename> friend class _::Lazy;
		protected:
			lazyPolicyNondiscardable() noexcept
			{
			}

			void onDestroy(Context&);
			template<typename Descendant> void onDestroy(Context& S, Descendant& d);
			template<typename Descendant> void onDestroySingle(Context& S, Descendant& d);	//!< Simplified discard check for lazies that guaranteedly push single value

			void moveout()
			{
				Pushed = true;
			}

			// data
			bool Pushed = false;
		};



		//! A policy to push immediate value
		template<typename ValueType>
		class lazyImmediateValue final: public lazyPolicy {

			friend class Lazy<lazyImmediateValue<ValueType>>;
			template<typename...> friend class lazySeries;
			template<typename> friend class _::Lazy;

		public:
			lazyImmediateValue(lazyImmediateValue<ValueType>&& src) noexcept:
				V(std::move(src.V))
			{
			}

		private:
			lazyImmediateValue(Context&, const ValueType& val) noexcept:
				V(val)
			{
			}

			lazyImmediateValue(Context&, ValueType&& val) noexcept:
				V(std::move(val))
			{
			}



			void push(Context& S);
			void pushSingle(Context& S);

			// data
			ValueType V;
		};



		//! A policy to push immediate value, Valset specialization
		template<>
		class lazyImmediateValue<Valset> final: public lazyPolicy {

			friend class Lazy<lazyImmediateValue<Valset>>;
			template<typename...> friend class lazySeries;
			template<typename> friend class _::Lazy;

		public:
			lazyImmediateValue(lazyImmediateValue<Valset>&& src) noexcept:
				V(src.V)
			{
			}

		private:
			lazyImmediateValue(Context&, const Valset& val) noexcept:
				V(val)
			{
			}

			lazyImmediateValue(Context&, Valset&& val) noexcept = delete;
			void push(Context& S);
			// data
			const Valset& V;
		};



		//! A policy to push immediate value, Value specialization
		template<>
		class lazyImmediateValue<Value> final: public lazyPolicy {

			friend class Lazy<lazyImmediateValue<Value>>;
			template<typename...> friend class lazySeries;
			template<typename> friend class _::Lazy;

		public:
			lazyImmediateValue(lazyImmediateValue<Value>&& src) noexcept:
				V(src.V)
			{
			}

		private:
			lazyImmediateValue(Context&, const Value& val) noexcept:
				V(val)
			{
			}

			lazyImmediateValue(Context&, Value&& val) noexcept = delete;
			void push(Context& S);

			void pushSingle(Context& S)
			{
				push(S);
			}
			// data
			const Value& V;
		};



		//! A policy to push immediate value, Table specialization
		template<>
		class lazyImmediateValue<Table> final: public lazyPolicy {

			friend class Lazy<lazyImmediateValue<Table>>;
			template<typename...> friend class lazySeries;
			template<typename> friend class _::Lazy;

		public:
			lazyImmediateValue(lazyImmediateValue<Table>&& src) noexcept:
				V(src.V)
			{
			}

		private:
			lazyImmediateValue(Context&, const Table& val) noexcept:
				V(val)
			{
			}

			lazyImmediateValue(Context& S, Table&& val) noexcept = delete;

			void push(Context& S);

			void pushSingle(Context& S)
			{
				push(S);
			}
			// data
			const Table& V;
		};



		//! Lazy pusher of results of other delayed operations
		//! (specialization of lazyImmediateValue)
		template<class Policy>
		class lazyImmediateValue<Lazy<Policy>> final: public lazyPolicy {

			friend class Lazy<lazyImmediateValue<Lazy<Policy>>>;
			template<typename...> friend class lazySeries;
			template<typename> friend class _::Lazy;

		public:
			lazyImmediateValue(lazyImmediateValue<Lazy<Policy>>&&) noexcept = default;

		private:
			lazyImmediateValue(Context&, Lazy<Policy>&& l) noexcept:
				lref(std::move(l))
			{
			}


			void push(Context&)
			{
				lref.push();
			}

			void pushSingle(Context&)
			{
				lref.pushSingle();
			}

			void pushOnly(Context&)
			{
				lref.pushOnly();
			}

			void onDestroy(Context&)
			{
				lref.onDestroy();
			}

			void moveout() noexcept
			{
				lref.moveout();
			}

			//data
			Lazy<Policy> lref;
		};

		template<typename ... Values> class lazyTableArray;
		template<typename ... KVPairs> class lazyTableRecords;

		//! Partial series-pusher specialization
		template<typename ValueType, typename ... Rest>
		class lazySeries<ValueType, Rest...> final: public lazyPolicy {

			template<typename> friend class _::Lazy;
			template<typename...> friend class lazySeries;
			template<typename...> friend class _::lazyTableArray;
			template<typename...> friend class _::lazyTableRecords;

		public:
			lazySeries(lazySeries<ValueType, Rest...>&&) noexcept = default;

		private:
			lazySeries(Context& S, ValueType&& val, Rest&& ... rest) noexcept:
				first(S, std::forward<ValueType>(val)),
				second(S, std::forward<Rest>(rest)...)
			{
			}

			void push(Context& S);

			void pushSingle(Context& S)
			{
				try {
					first.pushSingle(S);
				} catch(std::exception&) {
					second.moveout();
					throw;
				}
			}

			void pushBySingle(Context& S);

			void onDestroy(Context& S)
			{
				first.onDestroy(S);
				second.onDestroy(S);
			}

			void moveout() noexcept
			{
				first.moveout();
				second.moveout();
			}

			// data
			lazyImmediateValue<typename std::decay<ValueType>::type> first;
			lazySeries<Rest...> second;
		};



		//! Terminal series-pusher specialization
		template<>
		class lazySeries<> final{
			template<typename> friend class _::Lazy;
			template<typename...> friend class lazySeries;
			template<typename ...> friend class _::lazyTableRecords;
			template<typename ...> friend class _::lazyTableArray;

		public:
			lazySeries(lazySeries<>&&) noexcept = default;

		private:

			lazySeries(Context&)
			{
			}

			void push(Context&)
			{
			}

			void pushSingle(Context&)
			{
			}

			void pushBySingle(Context&)
			{
			}

			void onDestroy(Context&)
			{
			}

			void moveout() noexcept
			{
			}
		};


	}
	//! @endcond
}

#endif // LUA_LAZY_HPP_INCLUDED
