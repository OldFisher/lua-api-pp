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
		class lazyGlobalIndexer;
		class globalIndexer;
		class lazyUpvalueIndexer;
		class uvIndexer;
		template<typename, typename>  class lazyTempIndexer;
		template<typename, typename...> class lazyCall;
		template<typename, typename...> class lazyPCall;
		template<typename...> class lazyClosure;
		template<typename> class lazyLenTemp;
		template<typename> class lazyMtTemp;

		// Operations
		class lazyConcatSelector;
		template<typename, typename> class lazyConcat;
#if(LUAPP_API_VERSION >= 52)
		template<typename, typename, _::Arithmetics> class lazyArithmetics;
#endif

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
			template<typename, typename...> friend class ::lua::_::lazySeries;
			template<typename> friend class ::lua::_::lazyConstIndexer;
			friend class ::lua::_::lazyGlobalIndexer;
			friend class ::lua::_::lazyUpvalueIndexer;
			template<typename, typename> friend class ::lua::_::lazyTempIndexer;
			template<typename, typename ...> friend class ::lua::_::lazyCall;
			template<typename, typename ...> friend class ::lua::_::lazyPCall;
			template<typename...> friend class ::lua::_::lazyClosure;

			// Operations
			friend class ::lua::_::lazyConcatSelector;
			template<typename, typename> friend class ::lua::_::lazyConcat;
#if(LUAPP_API_VERSION >= 52)
			template<typename, typename, lua::_::Arithmetics> friend class ::lua::_::lazyArithmetics;
#endif	// V52+
			friend class ::lua::_::lazyLenTemp<Policy>;
			friend class ::lua::_::lazyMtTemp<Policy>;

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
			template<typename T> T cast()
			{
				return toTemporary().cast<T>();
			}

			//! Explicit conversion to supported types.
			template<typename T> T optcast(const T& backupValue = T())
			{
				return toTemporary().optcast<T>(backupValue);
			}

			//! Check if the value is of given type or convertible.
			template<typename T> bool is()
			{
				return toTemporary().is<T>();
			}

			ValueType type()
			{
				return toTemporary().type();
			}

			operator bool () &&
			{
				return toTemporary();
			}

			operator int ()
			{
				return toTemporary();
			}

			operator unsigned int()
			{
				return toTemporary();
			}

			operator float()
			{
				return toTemporary();
			}

			operator double()
			{
				return toTemporary();
			}

			operator const char* ()
			{
				return toTemporary();
			}

			operator std::string ()
			{
				return toTemporary();
			}

			operator CFunction ()
			{
				return toTemporary();
			}

			operator LightUserData()
			{
				return toTemporary();
			}



			//! @}

			//! @name Modifying (or potentially modifying) operations
			//! @{

			//! Assignment is enabled only if the policy has void assign(Context&, const ValueType&)
			//! Policies have no assignment by default. Indexers do have it.
			template <typename ValueType> void operator = (ValueType&& val)
			{
				static_assert(!std::is_same<::lua::Table&&, decltype(std::forward<ValueType>(val))>::value, "Using anonymous Table object");
				static_assert(!std::is_same<::lua::Value&&, decltype(std::forward<ValueType>(val))>::value, "Using anonymous Value object");
				static_assert(!std::is_same<::lua::Valset&&, decltype(std::forward<ValueType>(val))>::value, "Using anonymous Valset object");
				policy.assign(S, std::forward<ValueType>(val));
			}


			//! Indexation.
			template <typename IndexType>
			Lazy<lazyTempIndexer<Lazy<Policy>, typename std::decay<IndexType>::type>> operator [] (IndexType&& index) noexcept
			{
				return Lazy<lazyTempIndexer<Lazy<Policy>, typename std::decay<IndexType>::type>>(S, std::move(*this), std::forward<IndexType>(index));
			}

			//! Generic call.
			template<typename ... Args>
			Lazy<lazyCall<Lazy<Policy>, Args...>> operator () (Args&& ... args) noexcept
			{
				return Lazy<lazyCall<Lazy<Policy>, Args...>>(S, std::move(*this), std::forward<Args>(args)...);
			}

			//! Call function
			template<typename ... Args>
			Lazy<lazyCall<Lazy<Policy>, Args...>> call(Args&& ... args) noexcept
			{
				return Lazy<lazyCall<Lazy<Policy>, Args...>>(S, std::move(*this), std::forward<Args>(args)...);
			}

			//! Protected call function
			template<typename ... Args>
			Lazy<lazyPCall<Lazy<Policy>, Args...>> pcall(Args&& ... args) noexcept
			{
				return Lazy<lazyPCall<Lazy<Policy>, Args...>>(S, std::move(*this), std::forward<Args>(args)...);
			}
			//! @}

			//! @name Comparisons
			//! @{


			template<typename ValueType> typename std::enable_if<!IsLazy<ValueType>::value, bool>::type operator == (ValueType&& rhs)
			{
				return toTemporary().Valref::operator == (std::forward<ValueType>(rhs));
			}


			template<typename ValueType> typename std::enable_if<!IsLazy<ValueType>::value, bool>::type operator != (ValueType&& rhs)
			{
				return toTemporary().Valref::operator != (std::forward<ValueType>(rhs));
			}


			template<typename ValueType> typename std::enable_if<!IsLazy<ValueType>::value, bool>::type operator < (ValueType&& rhs)
			{
				return toTemporary().Valref::operator < (std::forward<ValueType>(rhs));
			}


			template<typename ValueType> typename std::enable_if<!IsLazy<ValueType>::value, bool>::type operator > (ValueType&& rhs)
			{
				return toTemporary().Valref::operator > (std::forward<ValueType>(rhs));
			}


			template<typename ValueType> typename std::enable_if<!IsLazy<ValueType>::value, bool>::type operator <= (ValueType&& rhs)
			{
				return toTemporary().Valref::operator <= (std::forward<ValueType>(rhs));
			}


			template<typename ValueType> typename std::enable_if<!IsLazy<ValueType>::value, bool>::type operator >= (ValueType&& rhs)
			{
				return toTemporary().Valref::operator >= (std::forward<ValueType>(rhs));
			}


			template<typename Policy2>
			bool operator == (Lazy<Policy2>&& rhs)
			{
				return toTemporary().Valref::operator == (std::move(rhs));
			}


			template<typename Policy2>
			bool operator != (Lazy<Policy2>&& rhs)
			{
				return toTemporary().Valref::operator != (std::move(rhs));
			}


			template<typename Policy2>
			bool operator < (Lazy<Policy2>&& rhs)
			{
				return toTemporary().Valref::operator < (std::move(rhs));
			}


			template<typename Policy2>
			bool operator > (Lazy<Policy2>&& rhs)
			{
				return toTemporary().Valref::operator > (std::move(rhs));
			}


			template<typename Policy2>
			bool operator <= (Lazy<Policy2>&& rhs)
			{
				return toTemporary().Valref::operator <= (std::move(rhs));
			}


			template<typename Policy2>
			bool operator >= (Lazy<Policy2>&& rhs)
			{
				return toTemporary().Valref::operator >= (std::move(rhs));
			}


			//! @}

			//! @name Operations
			//! @{

			//! Length
#if(LUAPP_API_VERSION >= 52)
			Lazy<lazyLenTemp<Policy>> len();
#else	// V51-
			size_t len()
			{
				return toTemporary().len();
			}
#endif	// V52+

			//! Metatable read/write
			Lazy<lazyMtTemp<Policy>> mt();

			//! Concatenation
			template<typename ValueType>
			Lazy<lazyConcat<Lazy<Policy>, typename std::decay<ValueType>::type>> operator & (ValueType&& v) noexcept
			{
				return Lazy<lazyConcat<Lazy<Policy>, typename std::decay<ValueType>::type>>(S, std::move(*this), std::forward<ValueType>(v));
			}

			template<typename ValueType, typename Policy_> friend Lazy<lazyConcat<typename std::enable_if< !HasValueSemantics<ValueType>::value, typename std::decay<ValueType>::type>::type, Lazy<Policy_>>> operator & (ValueType&& v, Lazy<Policy_>&& l) noexcept;
			template<typename ValueType> friend Lazy<lazyConcat<typename std::enable_if<!HasValueSemantics<ValueType>::value, typename std::decay<ValueType>::type>::type, Valref>> lua::operator & (ValueType&& v, const Valref& cv)  noexcept;


#if(LUAPP_API_VERSION >= 52)
			Lazy<lazyArithmetics<Lazy<Policy>, void, Arithmetics::UnaryMinus>> operator - () noexcept
			{
				return Lazy<lazyArithmetics<Lazy<Policy>, void, Arithmetics::UnaryMinus>>(S, std::move(*this));
			}

			template<typename Policy_, typename ValueType> friend Lazy<lazyArithmetics<typename std::enable_if<!HasValueSemantics<ValueType>::value, typename std::decay<ValueType>::type>::type, Lazy<Policy_>, Arithmetics::Add>> operator + (ValueType&& lhs, Lazy<Policy_>&& rhs) noexcept;
			template<typename ValueType> friend Lazy<lazyArithmetics<typename std::enable_if<!HasValueSemantics<ValueType>::value, typename std::decay<ValueType>::type>::type, Valref, Arithmetics::Add>> lua::operator + (ValueType&& lhs, const Valref& rhs) noexcept;

			template<typename ValueType>
			Lazy<lazyArithmetics<Lazy<Policy>, typename std::decay<ValueType>::type, Arithmetics::Add>> operator + (ValueType&& rhs) noexcept
			{
				return Lazy<lazyArithmetics<Lazy<Policy>, typename std::decay<ValueType>::type, Arithmetics::Add>>(S, std::move(*this), std::forward<ValueType>(rhs));
			}

			template<typename Policy_, typename ValueType> friend Lazy<lazyArithmetics<typename std::enable_if<!HasValueSemantics<ValueType>::value, typename std::decay<ValueType>::type>::type, Lazy<Policy_>, Arithmetics::Sub>> operator - (ValueType&& lhs, Lazy<Policy_>&& rhs) noexcept;
			template<typename ValueType> friend Lazy<lazyArithmetics<typename std::enable_if<!HasValueSemantics<ValueType>::value, typename std::decay<ValueType>::type>::type, Valref, Arithmetics::Sub>> lua::operator - (ValueType&& lhs, const Valref& rhs) noexcept;

			template<typename ValueType>
			Lazy<lazyArithmetics<Lazy<Policy>, typename std::decay<ValueType>::type, Arithmetics::Sub>> operator - (ValueType&& rhs) noexcept
			{
				return Lazy<lazyArithmetics<Lazy<Policy>, typename std::decay<ValueType>::type, Arithmetics::Sub>>(S, std::move(*this), std::forward<ValueType>(rhs));
			}

			template<typename Policy_, typename ValueType> friend Lazy<lazyArithmetics<typename std::enable_if<!HasValueSemantics<ValueType>::value, typename std::decay<ValueType>::type>::type, Lazy<Policy_>, Arithmetics::Multiply>> operator * (ValueType&& lhs, Lazy<Policy_>&& rhs) noexcept;
			template<typename ValueType> friend Lazy<lazyArithmetics<typename std::enable_if<!HasValueSemantics<ValueType>::value, typename std::decay<ValueType>::type>::type, Valref, Arithmetics::Multiply>> lua::operator * (ValueType&& lhs, const Valref& rhs) noexcept;

			template<typename ValueType>
			Lazy<lazyArithmetics<Lazy<Policy>, typename std::decay<ValueType>::type, Arithmetics::Multiply>> operator * (ValueType&& rhs) noexcept
			{
				return Lazy<lazyArithmetics<Lazy<Policy>, typename std::decay<ValueType>::type, Arithmetics::Multiply>>(S, std::move(*this), std::forward<ValueType>(rhs));
			}

			template<typename Policy_, typename ValueType> friend Lazy<lazyArithmetics<typename std::enable_if<!HasValueSemantics<ValueType>::value, typename std::decay<ValueType>::type>::type, Lazy<Policy_>, Arithmetics::Divide>> operator / (ValueType&& lhs, Lazy<Policy_>&& rhs) noexcept;
			template<typename ValueType> friend Lazy<lazyArithmetics<typename std::enable_if<!HasValueSemantics<ValueType>::value, typename std::decay<ValueType>::type>::type, Valref, Arithmetics::Divide>> lua::operator / (ValueType&& lhs, const Valref& rhs) noexcept;

			template<typename ValueType>
			Lazy<lazyArithmetics<Lazy<Policy>, typename std::decay<ValueType>::type, Arithmetics::Divide>> operator / (ValueType&& rhs) noexcept
			{
				return Lazy<lazyArithmetics<Lazy<Policy>, typename std::decay<ValueType>::type, Arithmetics::Divide>>(S, std::move(*this), std::forward<ValueType>(rhs));
			}

			template<typename Policy_, typename ValueType> friend Lazy<lazyArithmetics<typename std::enable_if<!HasValueSemantics<ValueType>::value, typename std::decay<ValueType>::type>::type, Lazy<Policy_>, Arithmetics::Modulo>> operator % (ValueType&& lhs, Lazy<Policy_>&& rhs) noexcept;
			template<typename ValueType> friend Lazy<lazyArithmetics<typename std::enable_if<!HasValueSemantics<ValueType>::value, typename std::decay<ValueType>::type>::type, Valref, Arithmetics::Modulo>> lua::operator % (ValueType&& lhs, const Valref& rhs) noexcept;

			template<typename ValueType>
			Lazy<lazyArithmetics<Lazy<Policy>, typename std::decay<ValueType>::type, Arithmetics::Modulo>> operator % (ValueType&& rhs) noexcept
			{
				return Lazy<lazyArithmetics<Lazy<Policy>, typename std::decay<ValueType>::type, Arithmetics::Modulo>>(S, std::move(*this), std::forward<ValueType>(rhs));
			}

			template<typename Policy_, typename ValueType> friend Lazy<lazyArithmetics<typename std::enable_if<!HasValueSemantics<ValueType>::value, typename std::decay<ValueType>::type>::type, Lazy<Policy_>, Arithmetics::Power>> operator ^ (ValueType&& lhs, Lazy<Policy_>&& rhs) noexcept;
			template<typename ValueType> friend Lazy<lazyArithmetics<typename std::enable_if<!HasValueSemantics<ValueType>::value, typename std::decay<ValueType>::type>::type, Valref, Arithmetics::Power>> lua::operator ^ (ValueType&& lhs, const Valref& rhs) noexcept;

			template<typename ValueType>
			Lazy<lazyArithmetics<Lazy<Policy>, typename std::decay<ValueType>::type, Arithmetics::Power>> operator ^ (ValueType&& rhs) noexcept
			{
				return Lazy<lazyArithmetics<Lazy<Policy>, typename std::decay<ValueType>::type, Arithmetics::Power>>(S, std::move(*this), std::forward<ValueType>(rhs));
			}
#endif	// V52+
			//! @}

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
			return rhs.operator ==  <ValueType> (std::forward<ValueType>(lhs));
		}

		template<typename Policy, typename ValueType> typename std::enable_if<!IsLazy<ValueType>::value && !IsAnchor<ValueType>::value, bool>::type operator != (ValueType&& lhs, _::Lazy<Policy>&& rhs)
		{
			return rhs.operator != <ValueType> (std::forward<ValueType>(lhs));
		}

		template<typename Policy, typename ValueType> typename std::enable_if<!IsLazy<ValueType>::value && !IsAnchor<ValueType>::value, bool>::type operator < (ValueType&& lhs, _::Lazy<Policy>&& rhs)
		{
			return rhs.operator >  <ValueType> (std::forward<ValueType>(lhs));
		}

		template<typename Policy, typename ValueType> typename std::enable_if<!IsLazy<ValueType>::value && !IsAnchor<ValueType>::value, bool>::type operator > (ValueType&& lhs, _::Lazy<Policy>&& rhs)
		{
			return rhs.operator <  <ValueType> (std::forward<ValueType>(lhs));
		}

		template<typename Policy, typename ValueType> typename std::enable_if<!IsLazy<ValueType>::value && !IsAnchor<ValueType>::value, bool>::type operator <= (ValueType&& lhs, _::Lazy<Policy>&& rhs)
		{
			return rhs.operator >=  <ValueType> (std::forward<ValueType>(lhs));
		}

		template<typename Policy, typename ValueType> typename std::enable_if<!IsLazy<ValueType>::value && !IsAnchor<ValueType>::value, bool>::type operator >= (ValueType&& lhs, _::Lazy<Policy>&& rhs)
		{
			return rhs.operator <=  <ValueType> (std::forward<ValueType>(lhs));
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
