/*
* This file is part of Lua API++ library (https://github.com/OldFisher/lua-api-pp)
* distributed under MIT License (http://opensource.org/licenses/MIT).
* See license.txt for details.
* (c) 2014 OldFisher
*/

#ifndef LUA_CALL_HPP_INCLUDED
#define LUA_CALL_HPP_INCLUDED


namespace lua {

	class Context;
	class Value;

	//! @cond

	namespace _ {


		class lazyCallUtils final {
			template <typename, typename...> friend class ::lua::_::lazyCall;
		private:
			static void call(lua_State* L, size_t oldtop, int rvAmount) noexcept;
		};

		//! Lazy policy for function calls
		template<typename Function, typename ... Args>
		class lazyCall final: public lazyPolicyNondiscardable{

			template<typename> friend class ::lua::_::Lazy;
			friend class ::lua::Valset;

		public:
			lazyCall(lazyCall<Function, Args...>&&) noexcept = default;

		private:

			lazyCall(Context& S, Function&& f, Args&& ... args) noexcept:
				funclazy(S, std::move(f)),
				arglazy(S, std::forward<Args>(args)...)
			{
			}

			lazyCall(Context& S, const Function& f, Args&& ... args) noexcept:
				funclazy(S, f),
				arglazy(S, std::forward<Args>(args)...)
			{
			}

			void push(Context& S, int rvAmount = -1);

			void pushSingle(Context& S)
			{
				push(S, 1);
			}

			void onDestroy(Context& S)
			{
				if(!Pushed)
					push(S, 0);
			}

			void moveout() noexcept
			{
				funclazy.moveout();
				arglazy.moveout();
			}

			// data
			Lazy<lazyImmediateValue<Function>> funclazy;
			Lazy<lazySeries<Args...>> arglazy;
		};




		class lazyPCallUtils final {
			template <typename, typename...> friend class ::lua::_::lazyPCall;
		private:
			static bool pcall(lua_State* L, size_t oldtop, int rvAmount) noexcept;
		};

		//! Lazy policy for protected function calls
		template<typename Function, typename ... Args>
		class lazyPCall final: public lazyPolicyNondiscardable{

			template<typename> friend class ::lua::_::Lazy;
			friend class ::lua::Valset;

		public:
			lazyPCall(lazyPCall<Function, Args...>&&) noexcept = default;

		private:

			lazyPCall(Context& S, Function&& f, Args&& ... args) noexcept:
				funclazy(S, std::move(f)),
				arglazy(S, std::forward<Args>(args)...)
			{
			}

			lazyPCall(Context& S, const Function& f, Args&& ... args) noexcept:
				funclazy(S, f),
				arglazy(S, std::forward<Args>(args)...)
			{
			}

			bool push(Context& S, int rvAmount = -1);

			bool pushSingle(Context& S)
			{
				return push(S, 1);
			}

			void onDestroy(Context& S)
			{
				if(!Pushed)
					push(S, 0);
			}

			void moveout() noexcept
			{
				funclazy.moveout();
				arglazy.moveout();
			}

			// data
			Lazy<lazyImmediateValue<Function>> funclazy;
			Lazy<lazySeries<Args...>> arglazy;
		};

	}

	//! @endcond

}

#endif // LUA_CALL_HPP_INCLUDED
