/*
* This file is part of Lua API++ library (https://github.com/OldFisher/lua-api-pp)
* distributed under MIT License (http://opensource.org/licenses/MIT).
* See license.txt for details.
* (c) 2014 OldFisher
*/

#ifndef LUA_GSTATE_HPP_INCLUDED
#define LUA_GSTATE_HPP_INCLUDED



namespace lua {

	//! @brief Lua state object.
	//! @details State object represents a Lua state. Besides state creation and destruction,
	//! it can be used for setting up the environment by executing Lua files,
	//! C functions or strings.
	class State final {

	public:

		//! @name Life cycle
		//! @{

		//! @brief Default constructor.
		State();

		//! @brief Custom allocator constructor.
		explicit State(void* (__cdecl customAllocatorFunction) (void* ud, void* ptr, size_t oldSize, size_t newSize), void* ud = nullptr);

		//! @brief Destructor frees the associated Lua state.
		//! @details All raw pointers to lua_State are invalidated after object destruction.
		~State() noexcept;

		//! @details State copying is prohibited.
		State(const State&) = delete;

		//! @details Assignment to State is prohibited
		State& operator = (const State&) = delete;
		//! @}

		//! @name Environment setup
		//! @{

		//! @brief Execute Lua file (text or bytecode).
		//! @pre filename != nullptr
		//! @throw std::runtime_error In case of execution error, what() contains additional information.
		void runFile(const char* filename);

		//! @brief Execute Lua file (text or bytecode).
		//! @throw std::runtime_error In case of execution error, what() contains additional information.
		//! @overload
		void runFile(const std::string& filename)
		{
			runFile(filename.c_str());
		}

		//! @brief Execute a string.
		//! @pre expression != nullptr
		//! @throw std::runtime_error In case of execution error, what() contains additional information.
		void runString(const char* expression);

		//! @brief Execute a string.
		//! @throw std::runtime_error In case of execution error, what() contains additional information.
		//! @overload
		void runString(const std::string& expression)
		{
			return runString(expression.c_str());
		}

		//! @brief Execute C function.
		//! @pre f != nullptr
		//! @throw std::runtime_error In case of execution error, what() contains additional information.
		void call(CFunction f);
		//! @}

		//! @name Direct Lua API interaction
		//! @{

		//! @brief Access to raw Lua state pointer (for direct use with Lua API).
		lua_State* getRawState() noexcept
		{
			return state;
		}

		//! @}

	private:

		lua_State* const state;
	};

}

#endif // LUA_GSTATE_HPP_INCLUDED
