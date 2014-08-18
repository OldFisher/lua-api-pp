/*
* This file is part of Lua API++ library (https://github.com/OldFisher/lua-api-pp)
* distributed under MIT License (http://opensource.org/licenses/MIT).
* See license.txt for details.
* (c) 2014 OldFisher
*/

#ifndef LUA_VALUE_HPP_INCLUDED
#define LUA_VALUE_HPP_INCLUDED



namespace lua {

	//! @brief Owner of a single stack slot.
	//! @details This is basically a @ref lua::Valref "Valref" which allocates
	//! its own slot on creation and frees it when destroyed. Its purpose is
	//! to anchor some value for repeated use.
	//! @note It is impossible to create a Value with empty table using anonymous Table object as initializer.
	//! Use @ref lua::Table::records "records" or @ref lua::Table::array "array" instead.
	class Value final: public Valref {
		friend class Table;

	public:
		//! @name Life cycle
		//! @{

#ifdef DOXYGEN_ONLY
		//! @brief Create a copy of referenced value.
		Value(Valref src);
		//! @brief Promote native value.
		Value(Valobj src, Context& context);
		//! @brief Take ownership of expression result.
		Value(Temporary src);
		//! @brief Assign new value to owned stack slot.
		Value& operator = (Valobj newValue);

#else	// Not DOXYGEN_ONLY
		Value(const Valref& src) noexcept;

		Value(const Value& src) noexcept:
			Value(static_cast<const Valref&>(src))
		{
		}

		template<typename ValueType>
		Value(ValueType&& val, Context& context);


		template<typename Policy> Value(_::Lazy<Policy>&& l);

		template<typename ValueType>
		Value& operator = (ValueType&& val)
		{
			Valref::operator = (std::forward<ValueType>(val));
			return *this;
		}

		Value(Table&&) = delete;	// See docs on Table constructor

		Value(const Table& src);
#endif	// DOXYGEN_ONLY

		//! @brief It is possible to return Value from functions.
		//! @details The move constructor is not implemented so you're not allowed to actually move Value, but you can return Values from functions.
		Value(Value&&)/* = delete*/;

		//! @details The owned slot is freed upon Value destruction.
		~Value() noexcept;

		//! @}

	private:
		Value(Context& s, int idx) noexcept:
			Valref(s, idx)
		{
		}
	};

}


#endif // LUA_VALUE_HPP_INCLUDED
