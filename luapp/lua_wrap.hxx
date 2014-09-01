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
			inline auto argCvt(const lua::Valref& value) -> decltype(std::declval<lua::Valref>().cast<ValueType>())
			{
				return value.cast<ValueType>();
			}


			//! Default return type converter.
			//! Pushes the value "as is".
			//! Specialize if necessary.
			template<typename ResultType>
			inline Retval rvCvt(ResultType rv, Context& s)
			{
				return s.ret(rv);
			}


			//! Read upvalue[1] as raw pointer
			void* getRawReserve(lua_State* L);

			//! Read upvalue[1] as raw userdata
			template<typename ImposedType>
			inline ImposedType getReservedFptr(Context& c)
			{
				// Casting void* to member-pointer is tricky business
				union HardCast {
					ImposedType fptr;
					void* ptr;
					HardCast(void* val): ptr(val) {}
				};

				return
					(sizeof(ImposedType) == sizeof(LightUserData)) ?
					HardCast(getRawReserve(c)).fptr :
					*reinterpret_cast<ImposedType*>(getRawReserve(c));
			}


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
				fp f = getReservedFptr<fp>(s);
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
				fp f = getReservedFptr<fp>(s);
				makeCallv(s, f, typename CreatePackIndices<sizeof...(ArgTypes)>::type());
				return s.ret();
			}

			//! Make a call to member function with argument transformation, return result
			template<typename Host, typename ReturnValueType, typename ... ArgTypes, size_t ... Indices>
			inline ReturnValueType makeMemberCall(Context& c, ReturnValueType (Host::*f)(ArgTypes...), PackIndices<Indices...>)
			{
				return (argCvt<Host>(c.args.at(0)).*f)(argCvt<typename std::decay<ArgTypes>::type>(c.args.at(1 + Indices))...);
			}


			//! Make member call
			template<typename Host, typename ReturnType, typename ... ArgTypes>
			Retval memberCall(Context& c)
			{
				using fp = ReturnType (Host::*)(ArgTypes...);
				fp f = getReservedFptr<fp>(c);
				return rvCvt<typename std::decay<ReturnType>::type>(makeMemberCall(c, f, typename CreatePackIndices<sizeof...(ArgTypes)>::type()), c);
			}

			//! Make a call to member function with argument transformation, discard result
			template<typename Host, typename ReturnValueType, typename ... ArgTypes, size_t ... Indices>
			inline void makeMemberCallv(Context& c, ReturnValueType (Host::*f)(ArgTypes...), PackIndices<Indices...>)
			{
				(argCvt<Host>(c.args.at(0)).*f)(argCvt<typename std::decay<ArgTypes>::type>(c.args.at(1 + Indices))...);
			}

			//! Make member call for void-function
			template<typename Host, typename ReturnType, typename ... ArgTypes>
			Retval memberCallv(Context& c)
			{
				using fp = ReturnType (Host::*)(ArgTypes...);
				fp f = getReservedFptr<fp>(c);
				makeMemberCallv(c, f, typename CreatePackIndices<sizeof...(ArgTypes)>::type());
				return c.ret();
			}



		}

	}

}
//! @endcond

#endif // LUA_WRAP_HPP_INCLUDED
