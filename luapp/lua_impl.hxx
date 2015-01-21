/*
* This file is part of Lua API++ library (https://github.com/OldFisher/lua-api-pp)
* distributed under MIT License (http://opensource.org/licenses/MIT).
* See license.txt for details.
* (c) 2014 OldFisher
*/

#ifndef LUA_IMPL_HPP_INCLUDED
#define LUA_IMPL_HPP_INCLUDED


//! @cond

namespace lua {


	namespace _ {


		//! Move-out function for assigned values, does nothing
		//! for immediate values and moves out the lazies
		template<typename T>
		inline void moveout(const T&) noexcept
		{
		}

		template<typename Policy>
		inline void moveout(Lazy<Policy>& l) noexcept
		{
			l.moveout();
		}
	}

//#####################  Valref  ###############################################


	//! @name Valref comparisons
	//! @{
	inline bool Valref::operator == (const Valref& rhs) const noexcept
	{
		return context.doCompare(index, rhs.index, _::Comparison::Equal);
	}

	inline bool Valref::operator != (const Valref& rhs) const noexcept
	{
		return !context.doCompare(index, rhs.index, _::Comparison::Equal);
	}

	inline bool Valref::operator < (const Valref& rhs) const noexcept
	{
		return context.doCompare(index, rhs.index, _::Comparison::Less);
	}

	inline bool Valref::operator > (const Valref& rhs) const noexcept
	{
		return context.doCompare(rhs.index, index, _::Comparison::Less);
	}

	inline bool Valref::operator <= (const Valref& rhs) const noexcept
	{
		return context.doCompare(index, rhs.index, _::Comparison::LessEqual);
	}

	inline bool Valref::operator >= (const Valref& rhs) const noexcept
	{
		return context.doCompare(rhs.index, index,_:: Comparison::LessEqual);
	}




	template <typename ValueType> inline typename std::enable_if<!_::IsAnchor<ValueType>::value, bool>::type Valref::operator == (ValueType&& rhs) const
	{
		context.ipush(std::forward<ValueType>(rhs));
		auto rv = context.doCompare(index, context.getTop(), _::Comparison::Equal);
		context.pop();
		return rv;
	}

	template <typename ValueType> inline typename std::enable_if<!_::IsAnchor<ValueType>::value, bool>::type Valref::operator != (ValueType&& rhs) const
	{
		context.ipush(std::forward<ValueType>(rhs));
		auto rv =  !context.doCompare(index, context.getTop(), _::Comparison::Equal);
		context.pop();
		return rv;
	}

	template <typename ValueType> inline typename std::enable_if<!_::IsAnchor<ValueType>::value, bool>::type Valref::operator < (ValueType&& rhs) const
	{
		context.ipush(std::forward<ValueType>(rhs));
		auto rv =  context.doCompare(index, context.getTop(), _::Comparison::Less);
		context.pop();
		return rv;
	}

	template <typename ValueType> inline typename std::enable_if<!_::IsAnchor<ValueType>::value, bool>::type Valref::operator > (ValueType&& rhs) const
	{
		context.ipush(std::forward<ValueType>(rhs));
		auto rv =  context.doCompare(context.getTop(), index, _::Comparison::Less);
		context.pop();
		return rv;
	}

	template <typename ValueType> inline typename std::enable_if<!_::IsAnchor<ValueType>::value, bool>::type Valref::operator <= (ValueType&& rhs) const
	{
		context.ipush(std::forward<ValueType>(rhs));
		auto rv =  context.doCompare(index, context.getTop(), _::Comparison::LessEqual);
		context.pop();
		return rv;
	}

	template <typename ValueType> inline typename std::enable_if<!_::IsAnchor<ValueType>::value, bool>::type Valref::operator >= (ValueType&& rhs) const
	{
		context.ipush(std::forward<ValueType>(rhs));
		auto rv =  context.doCompare(context.getTop(), index, _::Comparison::LessEqual);
		context.pop();
		return rv;
	}




	template <typename ValueType> inline typename std::enable_if<!_::IsAnchor<ValueType>::value, bool>::type operator == (ValueType&& lhs, const Valref& rhs)
	{
		return rhs == std::forward<ValueType>(lhs);
	}

	template <typename ValueType> inline typename std::enable_if<!_::IsAnchor<ValueType>::value, bool>::type operator != (ValueType&& lhs, const Valref& rhs)
	{
		return rhs != std::forward<ValueType>(lhs);
	}

	template <typename ValueType> inline typename std::enable_if<!_::IsAnchor<ValueType>::value, bool>::type operator < (ValueType&& lhs, const Valref& rhs)
	{
		return rhs > std::forward<ValueType>(lhs);
	}

	template <typename ValueType> inline typename std::enable_if<!_::IsAnchor<ValueType>::value, bool>::type operator > (ValueType&& lhs, const Valref& rhs)
	{
		return rhs < std::forward<ValueType>(lhs);
	}

	template <typename ValueType> inline typename std::enable_if<!_::IsAnchor<ValueType>::value, bool>::type operator <= (ValueType&& lhs, const Valref& rhs)
	{
		return rhs >= std::forward<ValueType>(lhs);
	}

	template <typename ValueType> inline typename std::enable_if<!_::IsAnchor<ValueType>::value, bool>::type operator >= (ValueType&& lhs, const Valref& rhs)
	{
		return rhs <= std::forward<ValueType>(lhs);
	}

	//! @}



	template<typename ValueType> Valref& Valref::operator = (ValueType&& val)
	{
		context.ipush(std::forward<ValueType>(val));
		this->replace();
		return *this;
	}


#if(LUAPP_API_VERSION >= 52)
	inline _::Lazy<_::lazyLen> Valref::len() const  noexcept
	{
		return _::Lazy<_::lazyLen>(context, index);
	}
#endif	// V52+


	inline _::Lazy<_::lazyMT> Valref::mt() const noexcept
	{
		return _::Lazy<_::lazyMT>(context, index);
	}


#if(LUAPP_API_VERSION >= 53)

	inline _::Lazy<_::lazyLinked> Valref::linked() const noexcept
	{
		return _::Lazy<_::lazyLinked>(context, index);
	}


	inline _::Lazy<_::lazyConstIntIndexer> Valref::operator [] (int index_) const noexcept
	{
		return _::Lazy<_::lazyConstIntIndexer>(context, index, index_);
	}

	inline _::Lazy<_::lazyConstIntIndexer> Valref::operator [] (unsigned index_) const noexcept
	{
		return _::Lazy<_::lazyConstIntIndexer>(context, index, index_);
	}

	inline _::Lazy<_::lazyConstIntIndexer> Valref::operator [] (long long index_) const noexcept
	{
		return _::Lazy<_::lazyConstIntIndexer>(context, index, index_);
	}

	inline _::Lazy<_::lazyConstIntIndexer> Valref::operator [] (unsigned long long index_) const noexcept
	{
		return _::Lazy<_::lazyConstIntIndexer>(context, index, index_);
	}

#endif	// V53+


	inline _::Lazy<_::lazyExtConstUpvalue> Valref::upvalue(size_t index_) const noexcept
	{
		return _::Lazy<_::lazyExtConstUpvalue>(context, index, index_);
	}



	inline void Valref::pushUpvalues() const noexcept
	{
		const char* name = nullptr;
		size_t uvIndex = 0;
		do {
			++uvIndex;
			name = readUv(context, index, uvIndex);
			if(name)
				context.push(name);
		} while(name);
	}


	inline Valset Valref::getUpvalues() const noexcept
	{
		Valset rv(context);
		pushUpvalues();
		rv.Size = context.getTop() - rv.Idx + 1;
		return rv;
	}


#if(LUAPP_API_VERSION > 51)
	inline ClosureInfo Valref::getClosureInfo() const noexcept
	{
		context.duplicate(index);
		return retrieveClosureInfo(context);
	}
#else
	inline size_t Valref::getClosureInfo() const noexcept
	{
		context.duplicate(index);
		return retrieveClosureInfo(context);
	}
#endif


//#####################  tValue  ###############################################


	namespace _ {


		inline tValue::~tValue() noexcept
		{
#ifdef LUAPP_WATCH_STACK
			if(context.getCheckedTop() != index)
				throw std::runtime_error("Lua stack integrity compromised");
#endif // LUAPP_WATCH_STACK
			context.pop();
		}


//#####################  Lazy  #################################################


		template<typename Policy>
		inline tValue Lazy<Policy>::toTemporary()
		{
			pushSingle();
			return S.makeTemp();
		}

#if(LUAPP_API_VERSION >= 52)
		template<typename Policy>
		inline Lazy<lazyLenTemp<Policy>> Lazy<Policy>::len() &&
		{
			return Lazy<lazyLenTemp<Policy>>(S, std::move(*this));
		}
#endif	// V52+

		template<typename Policy>
		inline Lazy<lazyMtTemp<Policy>> Lazy<Policy>::mt() &&
		{
			return Lazy<lazyMtTemp<Policy>>(S, std::move(*this));
		}

#if(LUAPP_API_VERSION >= 53)
		template<typename Policy>
		inline Lazy<lazyLinkedTemp<Policy>> Lazy<Policy>::linked() &&
		{
			return Lazy<lazyLinkedTemp<Policy>>(S, std::move(*this));
		}
#endif	// V53+

		template<typename Policy>
		inline Valset Lazy<Policy>::getUpvalues() &&
		{
			Valset rv(S);
			this->pushSingle();
			rv[0].pushUpvalues();
			S.remove(rv.Idx);
			rv.Size = S.getTop() - rv.Idx + 1;
			return rv;
		}


#if(LUAPP_API_VERSION > 51)
		template<typename Policy>
		ClosureInfo Lazy<Policy>::getClosureInfo() &&
		{
			this->pushSingle();
			return ::lua::Valref::retrieveClosureInfo(S.L);
		}
#else
		template<typename Policy>
		size_t Lazy<Policy>::getClosureInfo() &&
		{
			this->pushSingle();
			return ::lua::Valref::retrieveClosureInfo(S.L);
		}
#endif

//#####################  lazySeries  ###########################################

		template<typename ValueType, typename ... Rest>
		inline void lazySeries<ValueType, Rest...>::push(Context& S)
		{
			try {
				first.push(S);
			} catch(std::exception&) {
				second.moveout();
				throw;
			}

			try {
				second.push(S);
			} catch (std::exception&) {
				S.pop();
				throw;
			}
		}

		template<typename ValueType, typename ... Rest>
		inline void lazySeries<ValueType, Rest...>::pushBySingle(Context& S)
		{
			try {
				first.pushSingle(S);
			} catch(std::exception&) {
				second.moveout();
				throw;
			}

			try {
				second.pushBySingle(S);
			} catch (std::exception&) {
				S.pop();
				throw;
			}
		}


//#####################  lazyPolicyNondiscardable  ################################

		template<typename Descendant>
		inline void lazyPolicyNondiscardable::onDestroy(Context& S, Descendant& d)
		{
			if(!Pushed)
			{
				const size_t oldtop = S.getTop();
				Pushed = true;
				d.push(S);
				S.pop(S.getTop() - oldtop);
			}
		}

		template<typename Descendant>
		inline void lazyPolicyNondiscardable::onDestroySingle(Context& S, Descendant& d)
		{
			if(!Pushed)
			{
				Pushed = true;
				d.pushSingle(S);
				S.pop();
			}
		}


//#####################  lazyImmediateValue  ###################################


		template<typename T> inline void lazyImmediateValue<T>::push(Context& S)
		{
			S.push(V);
		}


		inline void lazyImmediateValue<Valset>::push(Context& S)
		{
			S.push(V);
		}


		inline void lazyImmediateValue<Table>::push(Context& S)
		{
			S.push(V);
		}


		inline void lazyImmediateValue<Value>::push(Context& S)
		{
			S.push(V);
		}


		template<typename T> inline void lazyImmediateValue<T>::pushSingle(Context& S)
		{
			S.ipush(V);
		}


//#####################  lazyConstIndexer  #####################################


		template<typename IndexType>
		template<typename ValueType>
		inline void lazyConstIndexer<IndexType>::assign(Context& S, ValueType&& value)
		{
#ifdef LUAPP_NONDISCARDABLE_INDEX
			Pushed = true;
#endif	// LUAPP_NONDISCARDABLE_INDEX

			try {
				idxlazy.pushSingle();
			} catch(std::exception&) {
				moveout();
				throw;
			}

			try {
				S.ipush(std::forward<ValueType>(value));
			} catch(std::exception&) {
				S.pop();
				throw;
			}

			lazyConstIndexerUtils::writeValue(S, tableref);
		}

		template<typename IndexType>
		inline void lazyConstIndexer<IndexType>::push(Context& S)
		{
#ifdef LUAPP_NONDISCARDABLE_INDEX
			Pushed = true;
#endif	// LUAPP_NONDISCARDABLE_INDEX
			idxlazy.pushSingle();
			lazyConstIndexerUtils::extractValue(S, tableref);
		}



#if(LUAPP_API_VERSION >= 53)
		template<typename ValueType>
		inline void lazyConstIntIndexer::assign(Context& c, ValueType&& value)
		{
#ifdef LUAPP_NONDISCARDABLE_INDEX
			Pushed = true;
#endif	// LUAPP_NONDISCARDABLE_INDEX
			c.ipush(std::forward<ValueType>(value));
			lazyConstIndexerUtils::writeValuei(c, tableref, index);
		}

		inline void lazyConstIntIndexer::push(Context& c)
		{
#ifdef LUAPP_NONDISCARDABLE_INDEX
			Pushed = true;
#endif	// LUAPP_NONDISCARDABLE_INDEX
			lazyConstIndexerUtils::extractValuei(c, tableref, index);
		}
#endif	// V53+


//#####################  lazyGlobalIndexer  ####################################


		template<typename ValueType>
		inline void lazyGlobalIndexer::assign(Context& S, ValueType&& value)
		{
			S.ipush(std::forward<ValueType>(value));
			this->writeValue(S);
		}


//#####################  globalIndexer  ########################################


		template<typename ValueType, typename ... OtherTypes>
		inline void globalIndexer::set(const char* name, ValueType&& value, OtherTypes&& ... other)
		{
			(*this)[name] = std::forward<ValueType>(value);
			set(std::forward<OtherTypes>(other)...);
		}


//#####################  lazyTempIndexer  ######################################


		template<typename TableType, typename IndexType>
		template<typename ValueType>
		inline void lazyTempIndexer<TableType, IndexType>::assign(Context& S, ValueType&& value)
		{
#ifdef LUAPP_NONDISCARDABLE_INDEX
			Pushed = true;
#endif	// LUAPP_NONDISCARDABLE_INDEX

			try {
				tablelazy.pushSingle();
			} catch(std::exception&) {
				idxlazy.moveout();
				_::moveout(std::forward<ValueType>(value));
				throw;
			}

			try {
				idxlazy.pushSingle();
			} catch(std::exception&) {
				S.pop();
				_::moveout(std::forward<ValueType>(value));
				throw;
			}

			try {
				S.ipush(std::forward<ValueType>(value));
			} catch(std::exception&)
			{
				S.pop(2);
				throw;
			}

			lazyTempIndexerUtils::writeValue(S);
		}


		template<typename TableType, typename IndexType>
		inline void lazyTempIndexer<TableType, IndexType>::push(Context& S)
		{
#ifdef LUAPP_NONDISCARDABLE_INDEX
			Pushed = true;
#endif	// LUAPP_NONDISCARDABLE_INDEX

			try {
				tablelazy.pushSingle();
			} catch(std::exception&) {
				idxlazy.moveout();
				throw;
			}

			try {
				idxlazy.pushSingle();
			} catch(std::exception&) {
				S.pop();
				throw;
			}

			lazyTempIndexerUtils::extractValue(S);
		}


#if(LUAPP_API_VERSION >= 53)
//#####################  lazyTempIntIndexer  ###################################
		template<typename TableType>
		template<typename ValueType>
		inline void lazyTempIntIndexer<TableType>::assign(Context& c, ValueType&& value)
		{
#ifdef LUAPP_NONDISCARDABLE_INDEX
			Pushed = true;
#endif	// LUAPP_NONDISCARDABLE_INDEX

			try {
				tableLazy.pushSingle();
			} catch(std::exception&) {
				_::moveout(std::forward<ValueType>(value));
				throw;
			}

			try {
				c.ipush(std::forward<ValueType>(value));
			} catch(std::exception&)
			{
				c.pop(1);
				throw;
			}

			lazyTempIndexerUtils::writeValuei(c, index);
		}


		template<typename TableType>
		inline void lazyTempIntIndexer<TableType>::push(Context& c)
		{
#ifdef LUAPP_NONDISCARDABLE_INDEX
			Pushed = true;
#endif	// LUAPP_NONDISCARDABLE_INDEX

			tableLazy.pushSingle();
			lazyTempIndexerUtils::extractValuei(c, index);
		}
#endif	// V53+



#if(LUAPP_API_VERSION >= 52)
//#####################  lazyLenTemp  ##########################################

		template<typename Policy>
		inline void lazyLenTemp<Policy>::push(Context& S)
		{
			lazySrc.pushSingle();
			lazyLen::pushLength(S, S.getTop());
			S.remove(S.getTop() - 1);
		}

#endif	// V52+
//#####################  lazyMT  ###############################################


		template<typename TableType>
		inline void lazyMT::assign(Context& S, TableType&& t)
		{
			S.ipush(std::forward<TableType>(t));
			doAssign(S, Index);
		}


		template<typename Policy>
		inline void lazyMtTemp<Policy>::push(Context& S)
		{
			srcLazy.pushSingle();
			lazyMT::doPush(S, S.getTop());
			S.remove(S.getTop() - 1);
		}


		template<typename Policy>
		template<typename TableType>
		inline void lazyMtTemp<Policy>::assign(Context& S, TableType&& t)
		{
			srcLazy.pushSingle();
			try {
				S.push(std::forward<TableType>(t));
			} catch(...) {
				S.pop();
				throw;
			}
			lazyMT::doAssign(S, -2);
			S.pop();
		}

#if(LUAPP_API_VERSION >= 53)

//#####################  lazyLinked  ###########################################


		template<typename ValueType>
		inline void lazyLinked::assign(Context& c, ValueType&& val)
		{
			c.ipush(std::forward<ValueType>(val));
			doAssign(c, index);
		}


		template<typename Policy>
		inline void lazyLinkedTemp<Policy>::push(Context& c)
		{
			srcLazy.pushSingle();
			lazyLinked::doPush(c, c.getTop());
			c.remove(c.getTop() - 1);
		}


		template<typename Policy>
		template<typename ValueType>
		inline void lazyLinkedTemp<Policy>::assign(Context& c, ValueType&& val)
		{
			srcLazy.pushSingle();
			try {
				c.push(std::forward<ValueType>(val));
			} catch(...) {
				c.pop();
				throw;
			}
			lazyLinked::doAssign(c, -2);
			c.pop();
		}
#endif	// V53+

//#####################  lazyRawIndexer  #######################################

		template<typename IndexType>
		inline void lazyRawIndexer<IndexType>::push(Context& S)
		{
			idxlazy.push();
			lazyRawIndexerUtils::read(S, tableref);
		}


		template<typename IndexType>
		template<typename ValueType>
		inline void lazyRawIndexer<IndexType>::assign(Context& S, ValueType&& v)
		{
			idxlazy.pushSingle();
			try {
				S.ipush(std::forward<ValueType>(v));
				lazyRawIndexerUtils::write(S, tableref);
			} catch(std::exception&) {
				S.pop();
				throw;
			}
		}


		template<typename ValueType>
		inline void lazyRawIndexer<int>::assign(Context& S, ValueType&& v)
		{
			S.ipush(std::forward<ValueType>(v));
			this->set(S, tableref, Idx);
		}



		template<typename ValueType>
		inline void lazyRawIndexer<LightUserData>::assign(Context& S, ValueType&& v)
		{
			S.ipush(std::forward<ValueType>(v));
			this->set(S, tableref, Idx);
		}



//#####################  lazyExtConstUpvalue  ##################################

		inline void lazyExtConstUpvalue::push(Context& c)
		{
			doPush(c, closureRef, index);
		}

		template<typename ValueType>
		inline void lazyExtConstUpvalue::assign(Context& c, ValueType&& v)
		{
			c.ipush(std::forward<ValueType>(v));
			doAssign(c, closureRef, index);
		}

//#####################  lazyExtTempUpvalue  ###################################

		template<typename Policy>
		inline void lazyExtTempUpvalue<Policy>::push(Context& c)
		{
			c.ipush(std::move(srcLazy));
			try { lazyExtConstUpvalue::doPush(c, c.getTop(), index); }
			catch(...) {
				c.pop();
				throw;
			}
			c.remove(c.getTop() - 1);
		}

		template<typename Policy>
		template<typename ValueType>
		inline void lazyExtTempUpvalue<Policy>::assign(Context& c, ValueType&& v)
		{
			const auto oldTop = c.getTop();
			try {
				c.ipush(std::move(srcLazy));
				c.ipush(std::forward<ValueType>(v));
				lazyExtConstUpvalue::doAssign(c,  c.getTop() - 1, index); }
			catch(...) {
				c.pop(c.getTop() - oldTop);
				throw;
			}
			c.pop();
		}

//#####################  lazyCall  #############################################

		template<typename Function, typename ... Args>
		inline void lazyCall<Function, Args...>::push(Context& S, int rvAmount)
		{
			const size_t oldtop = S.getTop();
			Pushed = true;

			try {
				funclazy.pushSingle();
			} catch (std::exception&) {
				arglazy.moveout();
				throw;
			}

			try {
				arglazy.push();
			} catch(std::exception&) {
				S.pop();	// Drop function
				throw;
			}

			lazyCallUtils::call(S, oldtop, rvAmount);
		}

//#####################  lazyPCall  ############################################

		template<typename Function, typename ... Args>
		inline bool lazyPCall<Function, Args...>::push(Context& S, int rvAmount)
		{
			const size_t oldtop = S.getTop();
			Pushed = true;
			// Push function
			try {
				funclazy.pushSingle();
			} catch(std::exception&) {
				arglazy.moveout();
				throw;
			}
			// Push args
			try {
				arglazy.push();
			} catch(std::exception&) {
				S.pop();	// Drop function
				throw;
			}
			return lazyPCallUtils::pcall(S, oldtop, rvAmount);
		}


//#####################  lazyClosure  ##########################################

		template<typename ... UVTypes>
		inline void lazyClosure<UVTypes...>::push(Context& S)
		{
			const size_t oldtop = S.getTop();
			uvlazy.push();
			lazyClosureUtils::makeClosure(S, Fn, S.getTop() - oldtop);
		}



//#####################  lazyConcatSelector  ###################################


		template<typename VT1, typename VT2>
		inline void lazyConcatSelector::pushOnly(
			Context& S,
			Lazy<lazyImmediateValue<VT1>>& l1,
			Lazy<lazyImmediateValue<VT2>>& l2)
		{
			const size_t oldtop = S.getTop();
			try {
				l1.push();
			} catch(std::exception&) {
				moveout(l2);
				throw;
			}

			try {
				l2.push();
			} catch(std::exception&) {
				S.pop(S.getTop() - oldtop);
				throw;
			}
		}


		template<typename VT11, typename VT12, typename VT2>
		inline void lazyConcatSelector::pushOnly(
			Context& S,
			Lazy<lazyImmediateValue<Lazy<lazyConcat<VT11, VT12>>>>& l1,
			Lazy<lazyImmediateValue<VT2>>& l2)
		{
			const size_t oldtop = S.getTop();
			try {
				l1.pushOnly();
			} catch(std::exception&) {
				moveout(l2);
				throw;
			}

			try {
				l2.push();
			} catch(std::exception&) {
				S.pop(S.getTop() - oldtop);
				throw;
			}
		}


		template<typename VT1, typename VT21, typename VT22>
		inline void lazyConcatSelector::pushOnly(
			Context& S,
			Lazy<lazyImmediateValue<VT1>>& l1,
			Lazy<lazyImmediateValue<Lazy<lazyConcat<VT21, VT22>>>>& l2)
		{
			const size_t oldtop = S.getTop();
			try {
				l1.push();
			} catch(std::exception&) {
				moveout(l2);
				throw;
			}

			try {
				l2.pushOnly();
			} catch(std::exception&) {
				S.pop(S.getTop() - oldtop);
				throw;
			}
		}


		template<typename VT11, typename VT12, typename VT21, typename VT22>
		inline void lazyConcatSelector::pushOnly(
			Context& S,
			Lazy<lazyImmediateValue<Lazy<lazyConcat<VT11, VT12>>>>& l1,
			Lazy<lazyImmediateValue<Lazy<lazyConcat<VT21, VT22>>>>& l2)
		{
			const size_t oldtop = S.getTop();
			try {
				l1.pushOnly();
			} catch(std::exception&) {
				moveout(l2);
				throw;
			}

			try {
				l2.pushOnly();
			} catch(std::exception&) {
				S.pop(S.getTop() - oldtop);
				throw;
			}
		}


//#####################  lazyConcat  ###########################################


		template<typename VT1, typename VT2>
		inline void lazyConcat<VT1, VT2>::push(Context& S)
		{
			const size_t oldtop = S.getTop();
#ifdef LUAPP_NONDISCARDABLE_CONCAT
			Pushed = true;
#endif	// LUAPP_NONDISCARDABLE_CONCAT
			pushOnly(S);
			S.doConcat(oldtop);
		}
#if(LUAPP_API_VERSION >= 52)
//#####################  lazyArithmetics  ######################################

		template<typename T1, typename T2, Arithmetics op>
		inline void lazyArithmetics<T1, T2, op>::push(Context& S)
		{
#ifdef LUAPP_NONDISCARDABLE_ARITHMETICS
			Pushed = true;
#endif	// LUAPP_NONDISCARDABLE_ARITHMETICS
			try {
				L1.pushSingle();
			} catch(std::exception&) {
				L2.moveout();
				throw;
			}

			try {
				L2.pushSingle();
			} catch(std::exception&) {
				S.pop();
				throw;
			}

			S.doArith(op);
		}


		template<typename T, Arithmetics op>
		inline void lazyArithmeticsUnary<T, op>::push(Context& c)
		{
#ifdef LUAPP_NONDISCARDABLE_ARITHMETICS
			Pushed = true;
#endif	// LUAPP_NONDISCARDABLE_ARITHMETICS
			srcLazy.pushSingle();
			c.doArith(op);
		}

#endif	// V52+
//##############################################################################
	}	// end of namespace _


//#####################  Valset  #############################################


	inline Valset::Valset(Context& s, size_t idx, size_t size_) noexcept:
		S(s),
		Idx(idx),
		Size(size_),
		oldCurrent(S.swapVs(this))
	{
	}


	inline Valset::Valset(Context& s) noexcept:
		S(s),
		Idx(s.getTop()+1),
		Size(0),
		oldCurrent(S.swapVs(this))
	{
	}


	inline Valset::Valset(const Valref& src) noexcept:
		S(src.context),
		Idx(S.duplicate(src.index)),
		Size(1),
		oldCurrent(S.swapVs(this))
	{
	}

	inline Valset::Valset(const Value& src) noexcept:
		Valset(static_cast<const Valref&>(src))
	{
	}

	inline Valset::Valset(const Table& src) noexcept:
		Valset(static_cast<const Valref&>(src))
	{
	}

	inline Valset::Valset(const Valset& src) noexcept:
		S(src.S),
		Idx(S.getTop()+1),
		Size( (S.push(src), S.getTop() + 1 - Idx) ),
		oldCurrent(S.swapVs(this))
	{
	}


	inline Valset::~Valset() noexcept
	{
#ifdef LUAPP_WATCH_STACK
		if(size_t(S.getCheckedTop()) != Idx + Size - 1)
			throw std::runtime_error("Lua stack integrity compromised");
#endif // LUAPP_WATCH_STACK
		S.pop(Size);
		S.currentVs = oldCurrent;
	}

	template<typename Policy>
	inline Valset::Valset(_::Lazy<Policy>&& l):
		S(l.S),
		Idx(S.getTop()+1),
		Size( (l.push(), S.getTop() + 1 - Idx) ),
		oldCurrent(S.swapVs(this))
	{
	}


	template<typename Function, typename ... Args>
	inline Valset::Valset(_::Lazy<_::lazyPCall<Function, Args...>>&& l, int rvAmount):
		S(l.S),
		Idx(S.getTop()+1),
		Success(l.policy.push(S, rvAmount)),
		Size(S.getTop() + 1 - Idx),
		oldCurrent(S.swapVs(this))
	{
	}

	template<typename Function, typename ... Args>
	inline Valset::Valset(_::Lazy<_::lazyCall<Function, Args...>>&& l, int rvAmount):
		S(l.S),
		Idx(S.getTop()+1),
		Size( (l.policy.push(S, rvAmount), S.getTop() + 1 - Idx) ),
		oldCurrent(S.swapVs(this))
	{
	}


	template<typename T>
	inline void Valset::push_back(T&& val)
	{
		if(!this->isBlocked())
		{
			S.push(std::forward<T>(val));
			Size = S.getTop() + 1 - Idx;
		}
		else
			throw std::runtime_error("Lua: attempt to grow blocked Valset");
	}


	template<typename... Types>
	inline void Valset::push_back(Types&& ... values)
	{
		if(!this->isBlocked())
		{
			S.masspush(std::forward<Types>(values)...);
			Size = S.getTop() + 1 - Idx;
		}
		else
			throw std::runtime_error("Lua: attempt to grow blocked Valset");
	}



	inline void Valset::pop_back(size_t amount)
	{
		if(!this->isBlocked())
		{
			S.pop(std::min(amount, Size));
			Size = S.getTop() + 1 - Idx;
		} else
			throw std::runtime_error("Lua: attempt to shrink blocked Valset");
	}


	inline bool Valset::isBlocked() const noexcept
	{
		return (this != S.getCurrentVs()) || (S.getTop() != Idx + Size - 1);
	}


//#####################  Value  ################################################

	template<typename Policy>
	inline Value::Value(_::Lazy<Policy>&& l):
		Valref(l.S, (l.pushSingle(), l.S.getTop()))
	{
	}


	inline Value::Value(const Valref& src) noexcept:
		Valref(src.context, src.context.duplicate(src.index))
	{
	}


	template<typename ValueType>
	inline Value::Value(ValueType&& val, Context& c):
		Valref(c, (c.ipush(std::forward<ValueType>(val)), c.getTop()))
	{

	}


	inline Value::~Value() noexcept
	{
#ifdef LUAPP_WATCH_STACK
		if(context.getCheckedTop() != index)
			throw std::runtime_error("Lua stack integrity compromised");
#endif // LUAPP_WATCH_STACK
		context.pop();
	}



	inline Value::Value(const Table& src):
		Value(static_cast<const Valref&>(src))
	{
	}



//#####################  Table  ################################################


	namespace _ {

		template<typename ... Values>
		inline void lazyTableArray<Values...>::push(Context& s)
		{
			const int tableNum = TableUtils::makeNew(s, sizeof...(Values), 0);
			try {
				values.push(s);
				for(int i = s.getTop(), idx = i - tableNum; i > tableNum; --i, --idx)
					TableUtils::setValue(s, tableNum, idx);
			} catch(std::exception&) {
				s.pop();
				throw;
			}
		}



		template<typename ... KVPairs>
		inline void lazyTableRecords<KVPairs...>::push(Context& s)
		{
			const int tableNum = TableUtils::makeNew(s, 0, sizeof...(KVPairs) / 2);
			try {
				values.pushBySingle(s);
				for(int i = s.getTop(); i > tableNum; i -= 2)
					TableUtils::setValue(s, tableNum);
			} catch(std::exception&) {
				s.pop();
				throw;
			}
		}




	}




	template<typename IterationFunction>
	inline typename std::enable_if<
		std::is_convertible<
			bool,
			decltype(std::declval<IterationFunction>()(std::declval<Valref>(), std::declval<Valref>()))
		>::value,
	size_t>::type Table::iterate(IterationFunction ifunc) const
	{
		const size_t kIdx = Anchor.context.getTop() + 1, vIdx = kIdx + 1;
		size_t iterationCount = 0;
		beginIteration(Anchor.context);
		while(nextIteration(Anchor)){
			++iterationCount;
			if(!ifunc(Valref(Anchor.context, kIdx), Valref(Anchor.context, vIdx)))
			{
				Anchor.context.pop(2);
				break;
			}
		}
		return iterationCount;
	}



	template<typename IterationFunction>
	inline typename std::enable_if<
		std::is_void<
			decltype(std::declval<IterationFunction>()(std::declval<Valref>(), std::declval<Valref>()))
		>::value,
	size_t>::type Table::iterate(IterationFunction ifunc) const
	{
		const size_t kIdx = Anchor.context.getTop() + 1, vIdx = kIdx + 1;
		size_t iterationCount = 0;
		beginIteration(Anchor.context);
		while(nextIteration(Anchor)){
			++iterationCount;
			ifunc(Valref(Anchor.context, kIdx), Valref(Anchor.context, vIdx));
		}
		return iterationCount;
	}



	template<typename IndexType>
	inline _::Lazy<_::lazyRawIndexer<IndexType>> Table::RawAccessor::operator [] (IndexType&& idx) const noexcept
	{
		return _::Lazy<_::lazyRawIndexer<IndexType>> (T.context, std::forward<IndexType>(idx));
	}


	inline _::Lazy<_::lazyRawIndexer<int>> Table::RawAccessor::operator [] (int idx) const noexcept
	{
		return _::Lazy<_::lazyRawIndexer<int>>(T.context, T.index, idx);
	}


	inline _::Lazy<_::lazyRawIndexer<int>> Table::RawAccessor::operator [] (unsigned int idx) const noexcept
	{
		return _::Lazy<_::lazyRawIndexer<int>>(T.context, T.index, idx);
	}


	inline _::Lazy<_::lazyRawIndexer<LightUserData>> Table::RawAccessor::operator [] (LightUserData idx) const noexcept
	{
		return _::Lazy<_::lazyRawIndexer<LightUserData>>(T.context, T.index, idx);
	}


	namespace _ {
		template<typename Policy, typename ... Args> inline Lazy<Policy> makeLazy(Args&& ... args) noexcept
		{
            return Lazy<Policy>(std::forward<Args>(args)...);
		}
	}



//#####################  Destructive operations  ###############################
//################  Concatenation, arithmetics, bit ops  #######################

	namespace _ {


		inline Context& selectContext (Context& c, decltype(nullptr)) noexcept
		{
			return c;
		}

		inline Context& selectContext (decltype(nullptr), Context& c) noexcept
		{
			return c;
		}

		inline Context& selectContext (Context& , Context& c2) noexcept
		{
			return c2;
		}

		inline Context& extractContext(const lua::Valref& obj) noexcept
		{
			return obj.context;
		}

		template<typename Policy> inline Context& extractContext(const Lazy<Policy>& obj) noexcept
		{
			return obj.S;
		}

		inline decltype(nullptr) extractContext(...) noexcept
		{
			return nullptr;
		}


		template<typename ValueType>
		struct canPromote {
			static constexpr bool value =
				std::is_same<
					Context&,
					decltype(
						extractContext(
							std::declval<
								typename std::decay<ValueType>::type
							>()
						)
					)
				>::value;
		};

		template<typename Value1, typename Value2>
		struct isPromotable {
			static constexpr bool value = canPromote<Value1>::value || canPromote<Value2>::value;
		};

	}


	template<typename Value1, typename Value2>
	inline _::Lazy<_::lazyConcat<typename std::enable_if<_::isPromotable<Value1, Value2>::value, typename std::decay<Value1>::type>::type, typename std::decay<Value2>::type>> operator& (Value1&& lhs, Value2&& rhs) noexcept
	{
		return _::makeLazy<_::lazyConcat<typename std::decay<Value1>::type, typename std::decay<Value2>::type>>(_::selectContext(_::extractContext(lhs), _::extractContext(rhs)), std::forward<Value1>(lhs), std::forward<Value2>(rhs));
	}

#if(LUAPP_API_VERSION >= 52)

	template<typename ValueType>
	inline 	_::Lazy<_::lazyArithmeticsUnary<typename std::enable_if<_::isPromotable<ValueType, ValueType>::value, typename std::decay<ValueType>::type>::type, _::Arithmetics::UnaryMinus>> operator - (ValueType&& val) noexcept
	{
        return _::makeLazy<_::lazyArithmeticsUnary<typename std::decay<ValueType>::type, _::Arithmetics::UnaryMinus>>(_::extractContext(val), std::forward<ValueType>(val));
	}

	template<typename Value1, typename Value2>
	inline _::Lazy<_::lazyArithmetics<typename std::enable_if<_::isPromotable<Value1, Value2>::value, typename std::decay<Value1>::type>::type, typename std::decay<Value2>::type, _::Arithmetics::Add>> operator+ (Value1&& lhs, Value2&& rhs) noexcept
	{
		return _::makeLazy<_::lazyArithmetics<typename std::decay<Value1>::type, typename std::decay<Value2>::type, _::Arithmetics::Add>>(_::selectContext(_::extractContext(lhs), _::extractContext(rhs)), std::forward<Value1>(lhs), std::forward<Value2>(rhs));
	}

	template<typename Value1, typename Value2>
	inline _::Lazy<_::lazyArithmetics<typename std::enable_if<_::isPromotable<Value1, Value2>::value, typename std::decay<Value1>::type>::type, typename std::decay<Value2>::type, _::Arithmetics::Sub>> operator- (Value1&& lhs, Value2&& rhs) noexcept
	{
		return _::makeLazy<_::lazyArithmetics<typename std::decay<Value1>::type, typename std::decay<Value2>::type, _::Arithmetics::Sub>>(_::selectContext(_::extractContext(lhs), _::extractContext(rhs)), std::forward<Value1>(lhs), std::forward<Value2>(rhs));
	}

	template<typename Value1, typename Value2>
	inline _::Lazy<_::lazyArithmetics<typename std::enable_if<_::isPromotable<Value1, Value2>::value, typename std::decay<Value1>::type>::type, typename std::decay<Value2>::type, _::Arithmetics::Multiply>> operator* (Value1&& lhs, Value2&& rhs) noexcept
	{
		return _::makeLazy<_::lazyArithmetics<typename std::decay<Value1>::type, typename std::decay<Value2>::type, _::Arithmetics::Multiply>>(_::selectContext(_::extractContext(lhs), _::extractContext(rhs)), std::forward<Value1>(lhs), std::forward<Value2>(rhs));
	}

	template<typename Value1, typename Value2>
	inline _::Lazy<_::lazyArithmetics<typename std::enable_if<_::isPromotable<Value1, Value2>::value, typename std::decay<Value1>::type>::type, typename std::decay<Value2>::type, _::Arithmetics::Divide>> operator/ (Value1&& lhs, Value2&& rhs) noexcept
	{
		return _::makeLazy<_::lazyArithmetics<typename std::decay<Value1>::type, typename std::decay<Value2>::type, _::Arithmetics::Divide>>(_::selectContext(_::extractContext(lhs), _::extractContext(rhs)), std::forward<Value1>(lhs), std::forward<Value2>(rhs));
	}

	template<typename Value1, typename Value2>
	inline _::Lazy<_::lazyArithmetics<typename std::enable_if<_::isPromotable<Value1, Value2>::value, typename std::decay<Value1>::type>::type, typename std::decay<Value2>::type, _::Arithmetics::Modulo>> operator% (Value1&& lhs, Value2&& rhs) noexcept
	{
		return _::makeLazy<_::lazyArithmetics<typename std::decay<Value1>::type, typename std::decay<Value2>::type, _::Arithmetics::Modulo>>(_::selectContext(_::extractContext(lhs), _::extractContext(rhs)), std::forward<Value1>(lhs), std::forward<Value2>(rhs));
	}

	template<typename Value1, typename Value2>
	inline _::Lazy<_::lazyArithmetics<typename std::enable_if<_::isPromotable<Value1, Value2>::value, typename std::decay<Value1>::type>::type, typename std::decay<Value2>::type, _::Arithmetics::Power>> operator^ (Value1&& lhs, Value2&& rhs) noexcept
	{
		return _::makeLazy<_::lazyArithmetics<typename std::decay<Value1>::type, typename std::decay<Value2>::type, _::Arithmetics::Power>>(_::selectContext(_::extractContext(lhs), _::extractContext(rhs)), std::forward<Value1>(lhs), std::forward<Value2>(rhs));
	}

#if(LUAPP_API_VERSION >= 53)

	template<typename Value1, typename Value2>
	inline _::Lazy<_::lazyArithmetics<typename std::enable_if<_::isPromotable<Value1, Value2>::value, typename std::decay<Value1>::type>::type, typename std::decay<Value2>::type, _::Arithmetics::IntegerDivide>> idiv(Value1&& lhs, Value2&& rhs) noexcept
	{
		return _::makeLazy<_::lazyArithmetics<typename std::decay<Value1>::type, typename std::decay<Value2>::type, _::Arithmetics::IntegerDivide>>(_::selectContext(_::extractContext(lhs), _::extractContext(rhs)), std::forward<Value1>(lhs), std::forward<Value2>(rhs));
	}

	template<typename Value1, typename Value2>
	inline _::Lazy<_::lazyArithmetics<typename std::enable_if<_::isPromotable<Value1, Value2>::value, typename std::decay<Value1>::type>::type, typename std::decay<Value2>::type, _::Arithmetics::BitwiseAnd>> band(Value1&& lhs, Value2&& rhs) noexcept
	{
		return _::makeLazy<_::lazyArithmetics<typename std::decay<Value1>::type, typename std::decay<Value2>::type, _::Arithmetics::BitwiseAnd>>(_::selectContext(_::extractContext(lhs), _::extractContext(rhs)), std::forward<Value1>(lhs), std::forward<Value2>(rhs));
	}

	template<typename Value1, typename Value2>
	inline _::Lazy<_::lazyArithmetics<typename std::enable_if<_::isPromotable<Value1, Value2>::value, typename std::decay<Value1>::type>::type, typename std::decay<Value2>::type, _::Arithmetics::BitwiseOr>> bor(Value1&& lhs, Value2&& rhs) noexcept
	{
		return _::makeLazy<_::lazyArithmetics<typename std::decay<Value1>::type, typename std::decay<Value2>::type, _::Arithmetics::BitwiseOr>>(_::selectContext(_::extractContext(lhs), _::extractContext(rhs)), std::forward<Value1>(lhs), std::forward<Value2>(rhs));
	}

	template<typename Value1, typename Value2>
	inline _::Lazy<_::lazyArithmetics<typename std::enable_if<_::isPromotable<Value1, Value2>::value, typename std::decay<Value1>::type>::type, typename std::decay<Value2>::type, _::Arithmetics::BitwiseXor>> bxor(Value1&& lhs, Value2&& rhs) noexcept
	{
		return _::makeLazy<_::lazyArithmetics<typename std::decay<Value1>::type, typename std::decay<Value2>::type, _::Arithmetics::BitwiseXor>>(_::selectContext(_::extractContext(lhs), _::extractContext(rhs)), std::forward<Value1>(lhs), std::forward<Value2>(rhs));
	}

	template<typename ValueType>
	inline _::Lazy<_::lazyArithmeticsUnary<typename std::enable_if<_::isPromotable<ValueType, ValueType>::value, typename std::decay<ValueType>::type>::type, _::Arithmetics::BitwiseNeg>> bneg(ValueType&& lhs) noexcept
	{
		return _::makeLazy<_::lazyArithmeticsUnary<typename std::decay<ValueType>::type, _::Arithmetics::BitwiseNeg>>(_::extractContext(lhs), std::forward<ValueType>(lhs));
	}

	template<typename Value1, typename Value2>
	inline _::Lazy<_::lazyArithmetics<typename std::enable_if<_::isPromotable<Value1, Value2>::value, typename std::decay<Value1>::type>::type, typename std::decay<Value2>::type, _::Arithmetics::ShiftLeft>> shl(Value1&& lhs, Value2&& rhs) noexcept
	{
		return _::makeLazy<_::lazyArithmetics<typename std::decay<Value1>::type, typename std::decay<Value2>::type, _::Arithmetics::ShiftLeft>>(_::selectContext(_::extractContext(lhs), _::extractContext(rhs)), std::forward<Value1>(lhs), std::forward<Value2>(rhs));
	}

	template<typename Value1, typename Value2>
	inline _::Lazy<_::lazyArithmetics<typename std::enable_if<_::isPromotable<Value1, Value2>::value, typename std::decay<Value1>::type>::type, typename std::decay<Value2>::type, _::Arithmetics::ShiftRight>> shr(Value1&& lhs, Value2&& rhs) noexcept
	{
		return _::makeLazy<_::lazyArithmetics<typename std::decay<Value1>::type, typename std::decay<Value2>::type, _::Arithmetics::ShiftRight>>(_::selectContext(_::extractContext(lhs), _::extractContext(rhs)), std::forward<Value1>(lhs), std::forward<Value2>(rhs));
	}

#endif	// next: V52-
#endif // next: all versions

	namespace _ {
#if(LUAPP_API_VERSION >= 52)
#if(LUAPP_API_VERSION >= 53)
		using ::lua::idiv;
		using ::lua::band;
		using ::lua::bor;
		using ::lua::bxor;
		using ::lua::bneg;
		using ::lua::shl;
		using ::lua::shr;
#endif	// next: V52-
		using ::lua::operator-;
		using ::lua::operator+;
		using ::lua::operator*;
		using ::lua::operator/;
		using ::lua::operator%;
		using ::lua::operator^;
#endif // next: all versions
		using ::lua::operator&;
	}




//#############################  Context  ######################################

	template<typename ... ArgTypes> void lua::Context::requireArgs(size_t amount)
	{
		const auto nArgsExpected = std::max(sizeof ... (ArgTypes), amount);
		if(args.size() < nArgsExpected)
			error(where() & " Insufficient number of arguments (" &  nArgsExpected & " expected, " & args.size() & " passed).");
		requireArg<ArgTypes..., void>(0);
	}



	template<typename T, typename ... OtherArgTypes> void lua::Context::requireArg(size_t idx)
	{
		if(args[idx].is<typename std::conditional<std::is_void<T>::value, Value, T>::type>())
			requireArg<OtherArgTypes...>(idx + 1);
		else
			error(where() & " Argument " & (idx + 1) & " type is incompatible.");
	}


//##############################################################################

//! @endcond
}


#endif // LUA_IMPL_HPP_INCLUDED
