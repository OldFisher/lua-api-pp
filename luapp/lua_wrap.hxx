/*
* This file is part of Lua API++ library (https://github.com/OldFisher/lua-api-pp)
* distributed under MIT License (http://opensource.org/licenses/MIT).
* See license.txt for details.
* (c) 2014 OldFisher
*/

#ifndef LUA_WRAP_HPP_INCLUDED
#define LUA_WRAP_HPP_INCLUDED

//! @cond
namespace lua {

	namespace _ {

		namespace wrap {

			// Pack enumeration helpers
			template <size_t ... N> struct PackIndices {
				typedef PackIndices<N..., sizeof...(N)> next;
			};

			template <size_t N> struct CreatePackIndices {
				typedef typename CreatePackIndices<N-1>::type::next type;
			};

			template <> struct CreatePackIndices<0> {
				typedef PackIndices<> type;
			};


			//! Default argument conversion routine (simple cast).
			//! Specialized for certain type if necessary.
			template<typename ValueType>
			inline ValueType argCvt(const lua::Valref& value)
			{
				return value.cast<ValueType>();
			}


			//! Default return type converter.
			//! Pushes the value "as is".
			//! Specialize if necessary.
			template<typename ResultType>
			inline Retval rvCvt(ResultType&& rv, Context& s)
			{
				return s.ret(rv);
			}


			//! Read upvalue[1] as a pointer to function
			StrippedFptr getAddress(lua_State* L);


			//! Make a call with argument transformation, return result
			template<typename ReturnValueType, typename ... ArgTypes, size_t ... Indices>
			inline ReturnValueType makeCall(Context& s, ReturnValueType (*f)(ArgTypes...), PackIndices<Indices...>)
			{
				return f(argCvt<typename std::decay<ArgTypes>::type>(s.args.at(Indices))...);
			}


			//! Wrap a call
			template<typename ReturnValueType, typename ... ArgTypes>
			Retval call(Context& s)
			{
				typedef ReturnValueType (*fp)(ArgTypes...);
				fp f = reinterpret_cast<fp>(getAddress(s));
				return rvCvt<typename std::decay<ReturnValueType>::type>(makeCall(s, f, typename CreatePackIndices<sizeof...(ArgTypes)>::type()), s);
			}


			//! Call to a void function with argument transformation
			template<typename ReturnValueType, typename ... ArgTypes, size_t ... Indices>
			inline void makeCallv(Context& s, ReturnValueType (*f)(ArgTypes...), PackIndices<Indices...>)
			{
				f(argCvt<typename std::decay<ArgTypes>::type>(s.args.at(Indices))...);
			}

			//! Wrap a call to a function discarding return value
			template<typename ReturnType, typename ... ArgTypes>
			Retval callv(Context& s)
			{
				typedef ReturnType (*fp)(ArgTypes...);
				fp f = reinterpret_cast<fp>(getAddress(s));
				makeCallv(s, f, typename CreatePackIndices<sizeof...(ArgTypes)>::type());
				return s.ret();
			}


		}

	}

}
//! @endcond

#endif // LUA_WRAP_HPP_INCLUDED
