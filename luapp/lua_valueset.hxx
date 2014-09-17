/*
* This file is part of Lua API++ library (https://github.com/OldFisher/lua-api-pp)
* distributed under MIT License (http://opensource.org/licenses/MIT).
* See license.txt for details.
* (c) 2014 OldFisher
*/

#ifndef LUA_VALUESET_HPP_INCLUDED
#define LUA_VALUESET_HPP_INCLUDED



namespace lua{

	class Valset;

	//! @cond
	namespace _ {

		//! Valset const_iterator
		class vsCIterator {

		friend class lua::Valset;

		public:
			// Iterator
			const Valref operator * () const noexcept
			{
				return val;
			}

			vsCIterator& operator ++ () noexcept
			{
				++val.index;
				return *this;
			}

			vsCIterator(const vsCIterator& src) noexcept = default;
			vsCIterator& operator = (const vsCIterator& src) noexcept
			{
				new(&val) Valref(src.val);
				return *this;
			}

			// InputIterator
			bool operator == (const vsCIterator& rhs) const noexcept
			{
				return val.index == rhs.val.index;
			}

			bool operator !=  (const vsCIterator& rhs) const noexcept
			{
				return ! (*this == rhs);
			}

			const Valref* operator -> () const noexcept
			{
				return &val;
			}

			vsCIterator operator ++ (int) noexcept
			{
				auto rv = *this;
				++*this;
				return rv;
			}

			// ForwardIterator
			vsCIterator() noexcept;//: val(*static_cast<Context*>(nullptr), 0) {}

			// BidirectionalIterator
			vsCIterator& operator -- () noexcept
			{
				--val.index;
				return *this;
			}

			vsCIterator operator -- (int) noexcept
			{
				auto rv(*this);
				--*this;
				return rv;
			}

			// RandomAccessIterator
			vsCIterator& operator += (size_t n) noexcept
			{
				val.index += n;
				return *this;
			}

			vsCIterator operator + (size_t n) const noexcept
			{
				auto rv(*this);
				return rv += n;
			}

			vsCIterator& operator -= (size_t n) noexcept
			{
				val.index -= n;
				return *this;
			}

			vsCIterator operator - (size_t n) const noexcept
			{
				auto rv(*this);
				return rv -= n;
			}

			ptrdiff_t operator - (const vsCIterator& rhs) const noexcept
			{
				return val.index - rhs.val.index;
			}

			const Valref operator [] (size_t index) const noexcept
			{
				return *(*this + index);
			}

			bool operator < (const vsCIterator& rhs) const noexcept
			{
				return val.index < rhs.val.index;
			}

			bool operator > (const vsCIterator& rhs) const noexcept
			{
				return val.index > rhs.val.index;
			}

			bool operator <= (const vsCIterator& rhs) const noexcept
			{
				return val.index <= rhs.val.index;
			}

			bool operator >= (const vsCIterator& rhs) const noexcept
			{
				return val.index >= rhs.val.index;
			}

		protected:
			// data
			mutable Valref val;
			// friend functions
			vsCIterator(Context& s, size_t idx) noexcept:
				val(s, idx)
			{
			}
		};


		inline vsCIterator operator + (size_t n, const vsCIterator& rhs) noexcept
		{
			return rhs + n;
		}

		inline vsCIterator operator - (size_t n, const vsCIterator& rhs) noexcept
		{
			return rhs - n;
		}

		//! Valset iterator
		class vsIterator final: public vsCIterator {
			friend class lua::Valset;

		public:
			// Iterator
			Valref operator * () const noexcept
			{
				return val;
			}

			vsIterator& operator ++ () noexcept
			{
				vsCIterator::operator ++ ();
				return *this;
			}

			vsIterator(const vsIterator&) noexcept = default;
			vsIterator& operator = (const vsIterator&) = default;


			// InputIterator
			Valref* operator -> () const noexcept
			{
				return &val;
			}

			vsIterator operator ++ (int) noexcept
			{
				auto rv = *this;
				++*this;
				return rv;
			}

			// ForwardIterator
			vsIterator() noexcept;// {}

			// BidirectionalIterator
			vsIterator& operator -- () noexcept
			{
				vsCIterator::operator -- ();
				return *this;
			}

			vsIterator operator -- (int) noexcept
			{
				auto rv(*this);
				--*this;
				return rv;
			}

			// RandomAccessIterator
			vsIterator& operator += (size_t n) noexcept
			{
				vsCIterator::operator += (n);
				return *this;
			}

			vsIterator operator + (size_t n) const noexcept
			{
				auto rv(*this);
				return rv += n;
			}

			vsIterator& operator -= (size_t n) noexcept
			{
				vsCIterator::operator -= (n);
				return *this;
			}

			vsIterator operator - (size_t n) const noexcept
			{
				auto rv(*this);
				return rv -= n;
			}

			ptrdiff_t operator - (const vsIterator& rhs) const noexcept
			{
				return vsCIterator::operator - (rhs);
			}

			Valref operator [] (size_t index) const noexcept
			{
				return *(*this + index);
			}

		private:
			vsIterator(Context& s, size_t idx) noexcept:
				vsCIterator(s, idx)
			{
			}
		};


		inline vsIterator operator + (size_t n, const vsIterator& rhs) noexcept
		{
			return rhs + n;
		}

		inline vsIterator operator - (size_t n, const vsIterator& rhs) noexcept
		{
			return rhs - n;
		}


	}
	//! @endcond

}



//! @cond
namespace std {

	template<>
	struct iterator_traits<lua::_::vsCIterator> {
		typedef ptrdiff_t difference_type;
		typedef const lua::Valref value_type;
		typedef const lua::Valref* pointer;
		typedef const lua::Valref& reference;
		typedef random_access_iterator_tag iterator_category;
	};

	template<>
	struct iterator_traits<lua::_::vsIterator> {
		typedef ptrdiff_t difference_type;
		typedef lua::Valref value_type;
		typedef lua::Valref* pointer;
		typedef lua::Valref& reference;
		typedef random_access_iterator_tag iterator_category;
	};

}
//! @endcond




namespace lua{

	//! @brief Valset is an STL-compatible container for contiguous Lua stack slots. Its primary use is accepting multiple return values.
	//! @details This object owns arbitrary number of neighbouring stack slots.
	//! It is STL compatible, so it's possible to use standard algorithms with it (also things like back_inserter, range-based for and so on).
	//! The values are accessed by indexation with integer indices or through iterators which dereference to Valref.\n
	//! This class is designed for following uses:
	//! - accept multiple return values from function calls;
	//! - hold function arguments (see @ref Context::args);
	//! - be a temporary storage for processing a number of values;
	//! - collect values to be returned from the function when their number is big or variable.
	//!
	//! If no values are placed on the stack after the Valset, it is unblocked, which means that new value can be added
	//! to its end with @ref Valset::push_back "push_back", or the last value be removed with @ref Valset::pop_back "pop_back" function.
	//! Otherwise the Valset is in "blocked" state and cannot be grown or shrunk.\n
	//! On object's destruction all owned slots are freed (unless it is used to return values from the function).\n
	//! When put inside value sequence (like function arguments or @ref Table::array "array content"), Valset automatically expands into contained values.
	//! But Valset cannot be used in a place that requires single value (like index).
	//! @note Contrary to general Lua standards, Valset indexation is 0-based.
	//! @note Valset's iterators conform to RandomAccessIterator concept.
	//! @note Valset does not actually store anything, it only remembers the lower and upper indices of owned stack slots.
	//! @note Lua has no concept of immutability, so even constant Valset could be used to change the referenced or nested values.
	class Valset final: public _::noNew {
		friend class Context;
		friend Valset Valref::getUpvalues() const noexcept;
		template<typename Policy> friend Valset _::Lazy<Policy>::getUpvalues() &&;

	public:

		typedef Valref value_type ;	//!< @brief The Valref objects are not actually stored, but rather created dynamically.
		typedef size_t size_type;	//!< @brief Index type.
		typedef ptrdiff_t difference_type;	//!< @brief Index difference.
#ifdef DOXYGEN_ONLY
		typedef valset_iterator iterator;		//!< @brief Normal iterator.
		typedef valset_const_iterator const_iterator;	//!< @brief Const iterator.
#else	// Not DOXYGEN_ONLY
		typedef _::vsIterator iterator;
		typedef _::vsCIterator const_iterator;
#endif	// DOXYGEN_ONLY
		typedef std::reverse_iterator<iterator> reverse_iterator;	//!< @brief Reverse iterator.
		typedef std::reverse_iterator<const_iterator> const_reverse_iterator;	//!< @brief Reverse const iterator.

		typedef value_type reference;	//!< @brief Reference to a value is still a Valref.
		typedef const value_type const_reference;	//!< @brief Const reference doesn't strictly enforce immutability.
		//! @cond
		typedef value_type* pointer;
        //! @endcond


		//! @name Life cycle
		//! @{

#ifdef DOXYGEN_ONLY
		//! @brief Construct from an expression.
		//! @details The created Valset will have 1 element inside.\n
		//! If the expression was a function call that returned multiple values, the Valset will contain all returned values.\n
		//! If the expression was a protected function call which ended succesfully, the Valset will contain all returned values.\n
		//! Unsuccesfull protected call will put into Valset error description and the @ref Valset::success "success" status will be "false".
		Valset(Temporary src);

		//! @brief Copy a value into set (works with Value and Table too).
		Valset(const Valref& src);
#else	// Not DOXYGEN_ONLY
		template<typename Policy> Valset(_::Lazy<Policy>&& l);

		//! Construct from stack value
		Valset(const Valref& src) noexcept;

		Valset(const Value& src) noexcept;
		Valset(const Table& src) noexcept;

		Valset(Value&&) = delete;
		Valset(Table&&) = delete;

		// Construct from call
		template<typename Function, typename ... Args>
		Valset(_::Lazy<_::lazyCall<Function, Args...>>&& l, int rvAmount = -1);

		// Construct from protected call
		template<typename Function, typename ... Args>
		Valset(_::Lazy<_::lazyPCall<Function, Args...>>&& l, int rvAmount = -1);
#endif	// DOXYGEN_ONLY
		//! @brief Valset can be returned from functions but not actually moved.
		Valset(Valset&&)/* = delete*/;

		//! @brief Create an empty Valset.
		//! @note Empty Valset does not occupy space on the stack, but it still blocks other Valsets.
		explicit Valset(Context& s) noexcept;

		//! @brief Create copies of values stored in src.
		Valset(const Valset& src) noexcept;

		//! @brief Owned stack slots are freed on destruction.
		~Valset() noexcept;
		//! @}

		//! @name Element access
		//! @{

		//! @brief Checked indexation.
		//! @throw std::range error
		reference at(size_t index)
		{
			if(!(index < size()))
				throw std::range_error("Lua: Valset accessed at wrong index");
			return begin()[index];
		}

		//! @brief Checked indexation.
		//! @throw std::range error
		const_reference at(size_t index) const
		{
			if(!(index < size()))
				throw std::range_error("Lua: Valset accessed at wrong index");
			return cbegin()[index];
		}

		//! @brief Unchecked indexation.
		const_reference operator [] (size_t index) const noexcept {return cbegin()[index];}

		//! @brief Unchecked indexation.
		reference operator [] (size_t index) noexcept {return begin()[index];}
		//! @}

		//! @name Iterators
		//! @{

		iterator begin() noexcept {return iterator(S, Idx);}
		const_iterator begin() const noexcept {return const_iterator(S, Idx);}
		const_iterator cbegin() const noexcept {return const_iterator(S, Idx);}
		iterator end() noexcept {return iterator(S, Idx + Size);}
		const_iterator end() const noexcept {return const_iterator(S, Idx + Size);}
		const_iterator cend() const noexcept {return const_iterator(S, Idx + Size);}
		reverse_iterator rbegin() noexcept {return reverse_iterator(end());}
		const_reverse_iterator rbegin() const noexcept {return const_reverse_iterator(cend());}
		const_reverse_iterator crbegin() const noexcept {return const_reverse_iterator(cend());}
		reverse_iterator rend() noexcept {return reverse_iterator(begin());}
		const_reverse_iterator rend() const noexcept {return const_reverse_iterator(cbegin());}
		const_reverse_iterator crend() const noexcept {return const_reverse_iterator(cbegin());}
		//! @}

		//! @name Capacity
		//! @{

		//! @brief Amount of stored values.
		size_t size() const noexcept {return Size;}

		//! @brief Check if the container is empty.
		bool empty() const noexcept {return size() == 0;}
		//! @}

		//! @name Modification
		//! @{

		//! @cond
		// Covered by push_back(...)
		//! @brief Add a value to the end.
		//! @pre !isBlocked()
		//! @throw std::runtime_error if Valset is @ref Valset::isBlocked "blocked" or if push failed.
		template<typename T> void push_back(T&& value);
		//! @endcond

		//! @brief Add values to the end.
		//! @pre !isBlocked()
		//! @throw std::runtime_error if Valset is @ref Valset::isBlocked "blocked" or if push failed.
		template<typename... Types> void push_back(Types&& ... values);

		//! @cond
		void push_back(Table&&) = delete;
		void push_back(Value&&) = delete;
		void push_back(Valset&&) = delete;
		//! @endcond

		//! @brief Remove a value from the end.
		//! @pre !isBlocked()
		//! @throw std::runtime_error if Valset is @ref Valset::isBlocked "blocked".
		//! @note Minimum of size() and "amount" is chosen.
		void pop_back(size_t amount = 1);
		//! @}

		//! @name Status query
		//! @{

		//! @brief Protected call status.
		//! @details If success() returns false, the protected call failed and Valset contains error message.
		//! If Valset wasn't created from protected call, this function always returns true.
		bool success () const noexcept {return Success;}

		//! @brief Blocked status.
		//! @details Any value that occupies a slot after Valset makes it blocked.
		//! Attempt to @ref Valset::push_back "add" or @ref Valset::pop_back "remove" an item from blocked Valset creates an exception.
		bool isBlocked() const noexcept;
		//! @}

	private:
		// data
		Context& S;
		const size_t Idx;
		const bool Success = true;
		size_t Size;
		const Valset* const oldCurrent;

		//friend functions
		Valset(Context& s, size_t idx, size_t size_) noexcept;

	};


}


#endif // LUA_VALUESET_HPP_INCLUDED
