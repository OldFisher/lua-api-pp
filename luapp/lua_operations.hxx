/*
* This file is part of Lua API++ library (https://github.com/OldFisher/lua-api-pp)
* distributed under MIT License (http://opensource.org/licenses/MIT).
* See license.txt for details.
* (c) 2014 OldFisher
*/

#ifndef LUA_OPERATIONS_HPP_INCLUDED
#define LUA_OPERATIONS_HPP_INCLUDED



namespace lua{
//! @cond
	namespace _ {

#if(LUAPP_API_VERSION >= 52)
//#####################  Length  ###############################################

		//! Length Lazy-policy for stack value
		class lazyLen final: public lazyPolicy {
			template<typename> friend class Lazy;
			template<typename> friend class lazyLenTemp;
		public:
			lazyLen(lazyLen&& src) noexcept:
				Idx(src.Idx)
			{
			}

		private:
			lazyLen(Context&, int idx) noexcept:
				Idx(idx)
			{
			}

			void push(Context& S);
			static void pushLength(lua_State* L, int idx);

			void pushSingle(Context& S)
			{
				push(S);
			}
			// data
			const int Idx;
		};


		template<typename Policy>
		class lazyLenTemp: public lazyPolicy {
			template<typename> friend class Lazy;

		public:
			lazyLenTemp(lazyLenTemp<Policy>&&) noexcept = default;

		private:
			lazyLenTemp(Context&, Lazy<Policy>&& lazySrc_):
				lazySrc(std::move(lazySrc_))
			{
			}

			void push(Context& S);

			void pushSingle(Context& S)
			{
				push(S);
			}

			// data
			Lazy<Policy> lazySrc;
		};
#endif	// V52+


//#####################  Concatenation  ########################################


		template<typename, typename> class lazyConcat;

		//! Smart selector that allows to merge several adjacent
		//! concat operations into one
		class lazyConcatSelector final {
			template<typename, typename> friend class lazyConcat;
		private:
			template<typename VT1, typename VT2>
			static void pushOnly(
				Context& S,
				Lazy<lazyImmediateValue<VT1>>& l1,
				Lazy<lazyImmediateValue<VT2>>& l2);

			template<typename VT11, typename VT12, typename VT2>
			static void pushOnly(
				Context& S,
				Lazy<lazyImmediateValue<Lazy<lazyConcat<VT11, VT12>>>>& l1,
				Lazy<lazyImmediateValue<VT2>>& l2);

			template<typename VT1, typename VT21, typename VT22>
			static void pushOnly(
				Context& S,
				Lazy<lazyImmediateValue<VT1>>& l1,
				Lazy<lazyImmediateValue<Lazy<lazyConcat<VT21, VT22>>>>& l2);

			template<typename VT11, typename VT12, typename VT21, typename VT22>
			static void pushOnly(
				Context& S,
				Lazy<lazyImmediateValue<Lazy<lazyConcat<VT11, VT12>>>>& l1,
				Lazy<lazyImmediateValue<Lazy<lazyConcat<VT21, VT22>>>>& l2);
		};



		//! Value-concatenation policy
		template<typename VT1, typename VT2>
#ifdef LUAPP_NONDISCARDABLE_CONCAT
		class lazyConcat final: public lazyPolicyNondiscardable {
#else
		class lazyConcat final: public lazyPolicy {
#endif
			template<typename, bool> friend class _::Lazy;

		public:
			lazyConcat(lazyConcat<VT1, VT2>&&) noexcept = default;

		private:
			typedef Lazy<lazyImmediateValue<VT1>> lazy1;
			typedef Lazy<lazyImmediateValue<VT2>> lazy2;

			friend class lazyConcatSelector;

			lazyConcat(Context& S, VT1&& v1, VT2&& v2) noexcept:
				L1(S, std::move(v1)),
				L2(S, std::move(v2))
			{
			}

			lazyConcat(Context& S, const VT1& v1, VT2&& v2) noexcept:
				L1(S, v1),
				L2(S, std::move(v2))
			{
			}

			lazyConcat(Context& S, VT1&& v1, const VT2& v2) noexcept:
				L1(S, std::move(v1)),
				L2(S, v2)
			{
			}

			lazyConcat(Context& S, const VT1& v1, const VT2& v2) noexcept:
				L1(S, v1),
				L2(S, v2)
			{
			}

			void push(Context& S);

			void pushSingle(Context& S)
			{
				push(S);
			}

			void pushOnly(Context& S)
			{
				lazyConcatSelector::pushOnly(S, L1, L2);
			};

			friend class lazyPolicyNondiscardable;

#ifdef LUAPP_NONDISCARDABLE_CONCAT
			void onDestroy(Context& S)
			{
				lazyPolicyNondiscardable::onDestroySingle(S, *this);
#else
			void onDestroy(Context&)
			{
				L1.onDestroy();
				L2.onDestroy();
#endif	// NOT LUAPP_NONDISCARDABLE_CONCAT
			}

			void moveout() noexcept
			{
#ifdef LUAPP_NONDISCARDABLE_CONCAT
				Pushed = true;
#endif	// NOT LUAPP_NONDISCARDABLE_CONCAT
				L1.moveout();
				L2.moveout();

			}

			// data
			lazy1 L1;
			lazy2 L2;

		};



#if(LUAPP_API_VERSION >= 52)
//#####################  Arithmetics  ##########################################

		template<typename T1, typename T2, _::Arithmetics op>
#ifdef LUAPP_NONDISCARDABLE_ARITHMETICS
		class lazyArithmetics final: public lazyPolicyNondiscardable {
#else
		class lazyArithmetics final: public lazyPolicy {
#endif	// LUAPP_NONDISCARDABLE_ARITHMETICS
			template <typename, bool> friend class _::Lazy;

		public:
			lazyArithmetics(lazyArithmetics<T1, T2, op>&&) noexcept = default;

		private:
			typedef Lazy<lazyImmediateValue<T1>> lazy1;
			typedef Lazy<lazyImmediateValue<T2>> lazy2;

			lazyArithmetics(Context& S, T1&& v1, T2&& v2) noexcept:
				L1(S, std::forward<T1>(v1)),
				L2(S, std::forward<T2>(v2))
			{
			}

			lazyArithmetics(Context& S, const T1& v1, T2&& v2) noexcept:
				L1(S, v1),
				L2(S, std::move(v2))
			{
			}

			lazyArithmetics(Context& S, T1&& v1, const T2& v2) noexcept:
				L1(S, std::move(v1)),
				L2(S, v2)
			{
			}

			lazyArithmetics(Context& S, const T1& v1, const T2& v2) noexcept:
				L1(S, v1),
				L2(S, v2)
			{
			}

			void push(Context& S);

			void pushSingle(Context& S)
			{
				push(S);
			}

#ifdef LUAPP_NONDISCARDABLE_ARITHMETICS
			friend class lazyPolicyNondiscardable;
			void onDestroy(Context& S)
			{
				lazyPolicyNondiscardable::onDestroySingle(S, *this);
			}
#else
			void onDestroy(Context&)
			{
				L1.onDestroy();
				L2.onDestroy();
			}
#endif	// LUAPP_NONDISCARDABLE_ARITHMETICS

			void moveout() noexcept
			{
#ifdef LUAPP_NONDISCARDABLE_ARITHMETICS
				Pushed = true;
#endif	// LUAPP_NONDISCARDABLE_ARITHMETICS
				L1.moveout();
				L2.moveout();

			}
			// data
			lazy1 L1;
			lazy2 L2;
		};



		template<typename T, _::Arithmetics op>
#ifdef LUAPP_NONDISCARDABLE_ARITHMETICS
		class lazyArithmeticsUnary final: public lazyPolicyNondiscardable {
#else
		class lazyArithmeticsUnary final: public lazyPolicyNondiscardable {
#endif	// LUAPP_NONDISCARDABLE_ARITHMETICS
			template <typename> friend class _::Lazy;

		public:
			lazyArithmeticsUnary(lazyArithmeticsUnary<T, op>&&) noexcept = default;

		private:
			using SrcLazy = Lazy<lazyImmediateValue<T>>;

			lazyArithmeticsUnary(Context& c, const T& v) noexcept:
				srcLazy(c, v)
			{
			}

			lazyArithmeticsUnary(Context& c, T&& v) noexcept:
				srcLazy(c, std::forward<T>(v))
			{
			}

			void push(Context& c);

			void pushSingle(Context& c)
			{
				push(c);
			}

#ifdef LUAPP_NONDISCARDABLE_ARITHMETICS
			friend class lazyPolicyNondiscardable;
			void onDestroy(Context& c)
			{
				lazyPolicyNondiscardable::onDestroySingle(c, *this);
			}
#else
			void onDestroy(Context&)
			{
				srcLazy.onDestroy();
			}
#endif	// LUAPP_NONDISCARDABLE_ARITHMETICS

			void moveout() noexcept
			{
#ifdef LUAPP_NONDISCARDABLE_ARITHMETICS
				Pushed = true;
#endif	// LUAPP_NONDISCARDABLE_ARITHMETICS
				srcLazy.moveout();
			}
			// data
			SrcLazy srcLazy;
		};


#endif	// V52+
//#####################  Where operation  ######################################

		class lazyWhere final: public lazyPolicy {
			template<typename> friend class _::Lazy;

		public:
			lazyWhere(lazyWhere&&) noexcept = default;

		private:
			lazyWhere(Context&) {}
			void push(Context& S);
			void pushSingle(Context& S)
			{
				push(S);
			}
		};

	}
//! @endcond

}


#endif // LUA_OPERATIONS_HPP_INCLUDED
