#include <boost/test/unit_test.hpp>

#include "fixtures.h"
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <algorithm>
#include <iterator>

using lua::Valref;
using lua::Value;
using lua::Table;
using lua::Valset;
using lua::Nil;
using lua::nil;
using lua::Context;
using lua::Retval;

using std::string;


struct Udata{int x;};
LUAPP_USERDATA(Udata, "Test.Userdata")




BOOST_AUTO_TEST_SUITE(ValsetObject)



BOOST_FIXTURE_TEST_CASE(CreateEmpty, fxContext)
{
	Valset vs(context);
	BOOST_CHECK_EQUAL(context.getTop(), 0);
	BOOST_CHECK_EQUAL(vs.size(), 0);
	BOOST_CHECK(vs.empty());
	BOOST_CHECK(vs.success());
	BOOST_CHECK(!vs.isBlocked());
}



BOOST_FIXTURE_TEST_CASE(CreateFromTemporary, fxContext)
{
	context.runString("val = 3.14");
	Valset vs(context.global["val"]);
	BOOST_CHECK_EQUAL(context.getTop(), 1);
	BOOST_CHECK_EQUAL(vs.size(), 1);
	BOOST_CHECK(!vs.empty());
	BOOST_CHECK(vs.success());
	BOOST_CHECK(!vs.isBlocked());
	BOOST_CHECK_EQUAL(vs[0].cast<double>(), 3.14);
}



BOOST_FIXTURE_TEST_CASE(CreateFromNormalCall, fxContext)
{
	context.runString("function fn() return 1, \"2\", 3; end");
	Valset vs = context.global["fn"]();
	BOOST_CHECK_EQUAL(context.getTop(), 3);
	BOOST_CHECK_EQUAL(vs.size(), 3);
	BOOST_CHECK(!vs.empty());
	BOOST_CHECK(vs.success());
	BOOST_CHECK(!vs.isBlocked());
	BOOST_CHECK(vs[0].is<int>());
	BOOST_CHECK_EQUAL(vs[0].cast<int>(), 1);
	BOOST_CHECK(vs[1].is<string>());
	BOOST_CHECK_EQUAL(vs[1].cast<string>(), "2");
	BOOST_CHECK(vs[2].is<int>());
	BOOST_CHECK_EQUAL(vs[2].cast<int>(), 3);
}



BOOST_FIXTURE_TEST_CASE(CreateFromExplicitCall, fxContext)
{
	context.runString("function fn() return 1, \"2\", 3; end");
	Valset vs = context.global["fn"].call();
	BOOST_CHECK_EQUAL(context.getTop(), 3);
	BOOST_CHECK_EQUAL(vs.size(), 3);
	BOOST_CHECK(!vs.empty());
	BOOST_CHECK(vs.success());
	BOOST_CHECK(!vs.isBlocked());
	BOOST_CHECK(vs[0].is<int>());
	BOOST_CHECK_EQUAL(vs[0].cast<int>(), 1);
	BOOST_CHECK(vs[1].is<string>());
	BOOST_CHECK_EQUAL(vs[1].cast<string>(), "2");
	BOOST_CHECK(vs[2].is<int>());
	BOOST_CHECK_EQUAL(vs[2].cast<int>(), 3);
}



BOOST_FIXTURE_TEST_CASE(CreateFromProtectedCallSuccess, fxContext)
{
	context.runString("function fn() return 1, \"2\", 3; end");
	Valset vs = context.global["fn"].pcall();
	BOOST_CHECK_EQUAL(context.getTop(), 3);
	BOOST_CHECK_EQUAL(vs.size(), 3);
	BOOST_CHECK(!vs.empty());
	BOOST_CHECK(vs.success());
	BOOST_CHECK(!vs.isBlocked());
	BOOST_CHECK(vs[0].is<int>());
	BOOST_CHECK_EQUAL(vs[0].cast<int>(), 1);
	BOOST_CHECK(vs[1].is<string>());
	BOOST_CHECK_EQUAL(vs[1].cast<string>(), "2");
	BOOST_CHECK(vs[2].is<int>());
	BOOST_CHECK_EQUAL(vs[2].cast<int>(), 3);
}




BOOST_FIXTURE_TEST_CASE(CreateFromProtectedCallFailure, fxContext)
{
	context.runString("function fn() error(\"Nice little error\") end");
	Valset vs = context.global["fn"].pcall();
	BOOST_CHECK_EQUAL(context.getTop(), 1);
	BOOST_CHECK_EQUAL(vs.size(), 1);
	BOOST_CHECK(!vs.empty());
	BOOST_CHECK(!vs.success());
	BOOST_CHECK(!vs.isBlocked());
	BOOST_CHECK(vs[0].is<string>());
	BOOST_CHECK(vs[0].cast<string>().find("Nice little error") != string::npos);
}



BOOST_FIXTURE_TEST_CASE(CreateFromAnchors, fxContext)
{
	{
		Valset src(context);
		src.push_back(3.14);
		Valset vs(src[0]);
		BOOST_CHECK_EQUAL(context.getTop(), 2);
		BOOST_CHECK_EQUAL(vs.size(), 1);
		BOOST_CHECK(!vs.empty());
		BOOST_CHECK(vs.success());
		BOOST_CHECK(!vs.isBlocked());
		BOOST_CHECK_EQUAL(vs[0].cast<double>(), 3.14);
	}
	{
		Value src(3.14, context);
		Valset vs(src);
		BOOST_CHECK_EQUAL(context.getTop(), 2);
		BOOST_CHECK_EQUAL(vs.size(), 1);
		BOOST_CHECK(!vs.empty());
		BOOST_CHECK(vs.success());
		BOOST_CHECK(!vs.isBlocked());
		BOOST_CHECK_EQUAL(vs[0].cast<double>(), 3.14);
	}
	{
		Table src = Table::array(context, 3.14);
		Valset vs(src);
		BOOST_CHECK_EQUAL(context.getTop(), 2);
		BOOST_CHECK_EQUAL(vs.size(), 1);
		BOOST_CHECK(!vs.empty());
		BOOST_CHECK(vs.success());
		BOOST_CHECK(!vs.isBlocked());
		BOOST_CHECK_EQUAL(vs[0][1].cast<double>(), 3.14);
	}
}



static Valset retVs(Context& c)
{
	Valset vs(c);
	vs.push_back(1);
	vs.push_back("2");
	vs.push_back(3);
	return vs;
}

BOOST_FIXTURE_TEST_CASE(ReturnFromFunction, fxContext)
{
	Valset vs = retVs(context);
	BOOST_CHECK_EQUAL(context.getTop(), 3);
	BOOST_CHECK_EQUAL(vs.size(), 3);
	BOOST_CHECK(!vs.empty());
	BOOST_CHECK(vs.success());
	BOOST_CHECK(!vs.isBlocked());
	BOOST_CHECK(vs[0].is<int>());
	BOOST_CHECK_EQUAL(vs[0].cast<int>(), 1);
	BOOST_CHECK(vs[1].is<string>());
	BOOST_CHECK_EQUAL(vs[1].cast<string>(), "2");
	BOOST_CHECK(vs[2].is<int>());
	BOOST_CHECK_EQUAL(vs[2].cast<int>(), 3);
}



BOOST_FIXTURE_TEST_CASE(DirectCopy, fxContext)
{
	Valset src = retVs(context);
	Valset vs = src;
	BOOST_CHECK_EQUAL(context.getTop(), 6);
	BOOST_CHECK_EQUAL(vs.size(), 3);
	BOOST_CHECK(!vs.empty());
	BOOST_CHECK(vs.success());
	BOOST_CHECK(!vs.isBlocked());
	BOOST_CHECK(vs[0].is<int>());
	BOOST_CHECK_EQUAL(vs[0].cast<int>(), 1);
	BOOST_CHECK(vs[1].is<string>());
	BOOST_CHECK_EQUAL(vs[1].cast<string>(), "2");
	BOOST_CHECK(vs[2].is<int>());
	BOOST_CHECK_EQUAL(vs[2].cast<int>(), 3);
}




BOOST_FIXTURE_TEST_CASE(Destruction, fxContext)
{
	{
		Valset vs= retVs(context);
		BOOST_CHECK_EQUAL(context.getTop(), 3);
		BOOST_CHECK_EQUAL(vs.size(), 3);
		BOOST_CHECK(!vs.empty());
	}
	BOOST_CHECK_EQUAL(context.getTop(), 0);
}



BOOST_FIXTURE_TEST_CASE(Modification, fxContext)
{
	Valset src = retVs(context);
	Valset vs(context);
	BOOST_CHECK(src.isBlocked());
	vs.push_back(1);
	BOOST_CHECK_EQUAL(context.getTop(), 4);
	BOOST_CHECK(src.isBlocked());
	vs.push_back("2");
	BOOST_CHECK_EQUAL(context.getTop(), 5);
	vs.push_back(3);
	BOOST_CHECK_EQUAL(context.getTop(), 6);
	vs.pop_back();
	BOOST_CHECK_EQUAL(context.getTop(), 5);
	vs.pop_back();
	BOOST_CHECK_EQUAL(context.getTop(), 4);
	vs.pop_back();
	BOOST_CHECK_EQUAL(context.getTop(), 3);
	vs.push_back(src);
	BOOST_CHECK_EQUAL(context.getTop(), 6);
	{
		Value v(1, context);
		BOOST_CHECK(vs.isBlocked());
	}
	BOOST_CHECK(!vs.isBlocked());

	BOOST_CHECK_THROW(src.push_back(1), std::runtime_error);
	BOOST_CHECK_THROW(src.pop_back(), std::runtime_error);

	context.runString("function fn() return 2, 3, 4; end");
	Valset vs2(context);
	vs2.push_back(1, context.global["fn"](), 5);
	BOOST_CHECK_EQUAL(vs2.size(), 5);
	for(size_t i = 0; i < 5; ++i)
		BOOST_CHECK_EQUAL(vs2[i].cast<unsigned int>(), i + 1);
}



BOOST_FIXTURE_TEST_CASE(Indexation, fxContext)
{
	Valset vs = retVs(context);
	BOOST_CHECK_EQUAL(vs.at(2).cast<int>(), 3);
	BOOST_CHECK_THROW(vs.at(3), std::range_error);
	BOOST_CHECK_EQUAL(vs[2].cast<int>(), 3);
}



BOOST_FIXTURE_TEST_CASE(IteratorRetrieval, fxContext)
{
	const Valset ce(context);
	const Valset cf = retVs(context);
	Valset e(context);
	Valset f = retVs(context);

	BOOST_CHECK(ce.begin() == ce.end());
	BOOST_CHECK(ce.cbegin() == ce.end());
	BOOST_CHECK(ce.begin() == ce.cend());
	BOOST_CHECK(ce.cbegin() == ce.cend());
	BOOST_CHECK(e.begin() == e.end());
	BOOST_CHECK(e.cbegin() == e.cend());
	BOOST_CHECK(e.cbegin() == e.end());
	BOOST_CHECK(e.begin() == e.cend());

	BOOST_CHECK(cf.begin() != cf.end());
	BOOST_CHECK(cf.cbegin() != cf.end());
	BOOST_CHECK(cf.begin() != cf.cend());
	BOOST_CHECK(cf.cbegin() != cf.cend());
	BOOST_CHECK(f.begin() != f.end());
	BOOST_CHECK(f.cbegin() != f.cend());
	BOOST_CHECK(f.cbegin() != f.end());
	BOOST_CHECK(f.begin() != f.cend());

	BOOST_CHECK(ce.rbegin() == ce.rend());
	BOOST_CHECK(ce.crbegin() == ce.rend());
	BOOST_CHECK(ce.rbegin() == ce.crend());
	BOOST_CHECK(ce.crbegin() == ce.crend());
	BOOST_CHECK(e.rbegin() == e.rend());
	BOOST_CHECK(e.crbegin() == e.crend());
	BOOST_CHECK(e.crbegin() == e.rend());
	BOOST_CHECK(e.rbegin() == e.crend());

	BOOST_CHECK(cf.rbegin() != cf.rend());
	BOOST_CHECK(cf.crbegin() != cf.rend());
	BOOST_CHECK(cf.rbegin() != cf.crend());
	BOOST_CHECK(cf.crbegin() != cf.crend());
	BOOST_CHECK(f.rbegin() != f.rend());
	BOOST_CHECK(f.crbegin() != f.crend());
	BOOST_CHECK(f.crbegin() != f.rend());
	BOOST_CHECK(f.rbegin() != f.crend());
}



using std::is_same;
using std::is_convertible;
BOOST_FIXTURE_TEST_CASE(IteratorInterface, fxContext)
{
	Valset vs = retVs(context);
	Valset::iterator i = vs.begin();
	Valset::const_iterator ci = vs.cbegin();

	// Iterator: CopyConstructible, CopyAssignable, lvalues Swappable, Destructible
	{
		auto j = vs.end();
		i = vs.begin();
		std::swap(i, j);
		BOOST_CHECK(i == vs.end());
		BOOST_CHECK(j == vs.begin());
		i = vs.begin();
	}
	{
		auto cj = vs.cend();
		ci = vs.cbegin();
		std::swap(ci, cj);
		BOOST_CHECK(ci == vs.cend());
		BOOST_CHECK(cj == vs.cbegin());
		ci = vs.begin();
	}

	// Iterator: dereferencing
	static_assert(is_same<decltype(*i), Valset::reference>::value, "Iterator must be dereferenced to type reference");
	static_assert(is_same<decltype(++i), Valset::iterator&>::value, "Iterator must be dereferenced to type reference");
	BOOST_CHECK_EQUAL((*i).cast<int>(), 1);
	static_assert(is_same<decltype(*ci), Valset::const_reference>::value, "Iterator must be dereferenced to type reference");
	static_assert(is_same<decltype(++ci), Valset::const_iterator&>::value, "Iterator must be dereferenced to type reference");
	BOOST_CHECK_EQUAL((*ci).cast<int>(), 1);

	// InputIterator: EqualityComparable
	BOOST_CHECK(i == vs.begin());
	BOOST_CHECK(vs.begin() == i);
	BOOST_CHECK(ci == vs.begin());
	BOOST_CHECK(vs.begin() == ci);

	// InputIterator: inequality comparable
	BOOST_CHECK(i != vs.end());
	BOOST_CHECK(vs.end() != i);
	BOOST_CHECK(ci != vs.end());
	BOOST_CHECK(vs.end() != ci);

	// InputIterator: dereferenceable with ->
	BOOST_CHECK_EQUAL(i->cast<int>(), 1);
	BOOST_CHECK_EQUAL(ci->cast<int>(), 1);

	//InputIterator: preincrementable
	static_assert(is_same<decltype(++i), Valset::iterator&>::value, "Iterator's preincrement must return iterator&");
	BOOST_CHECK(&i == &++i);
	static_assert(is_same<decltype(++ci), Valset::const_iterator&>::value, "Iterator's preincrement must return iterator&");
	BOOST_CHECK(&ci == &++ci);

	// InputIterator: increment (must be dereferenceable or past-the-end after it)
	++i; ++i;
	BOOST_CHECK(i == vs.end());
	++ci; ++ci;
	BOOST_CHECK(ci == vs.cend());

	// InputIterator: (void)i++ effects same as (void)++i
	i = vs.begin();
	++i;
	BOOST_CHECK_EQUAL(i->cast<string>(), "2");
	i++;
	BOOST_CHECK_EQUAL(i->cast<int>(), 3);
	ci = vs.cbegin();
	++ci;
	BOOST_CHECK_EQUAL(ci->cast<string>(), "2");
	ci++;
	BOOST_CHECK_EQUAL(ci->cast<int>(), 3);

	// InputIterator: *i++ convertible to value type
	static_assert(is_same<decltype(*i++), Valset::value_type>::value, "Dereferencing post-incremented iterator must be convertible to value type");
	BOOST_CHECK_EQUAL((*i++).cast<int>(), 3);
	BOOST_CHECK(i == vs.end());
	i = vs.begin();
	static_assert(is_same<decltype(*ci++), const Valset::value_type>::value, "Dereferencing post-incremented iterator must be convertible to value type");
	BOOST_CHECK_EQUAL((*ci++).cast<int>(), 3);
	BOOST_CHECK(ci == vs.cend());
	ci = vs.cbegin();


	// OutputIterator: dereference and assign
	*i = 1.1;
	BOOST_CHECK_EQUAL(vs[0].cast<double>(), 1.1);

	// BidirectionalIterator: predecrement
	static_assert(is_same<decltype(--i), Valset::iterator&>::value, "Iterator's predecrement must return iterator&");
	++i;
	BOOST_CHECK(&i == &--i);
	BOOST_CHECK(i == vs.begin());
	static_assert(is_same<decltype(--ci), Valset::const_iterator&>::value, "Iterator's predecrement must return iterator&");
	++ci;
	BOOST_CHECK(&ci == &--ci);
	BOOST_CHECK(ci == vs.cbegin());

	// BidirectionalIterator: postdecrement
	static_assert(is_convertible<decltype(i--), const Valset::iterator&>::value, "Iterator's postdecrement must be convertible to const iterator&");
	i = vs.end();
	BOOST_CHECK(i-- == vs.end());
	static_assert(is_convertible<decltype(ci--), const Valset::const_iterator&>::value, "Iterator's postdecrement must be convertible to const iterator&");
	ci = vs.cend();
	BOOST_CHECK(ci-- == vs.cend());

	// BidirectionalIterator: *i-- convertible to value type
	i = vs.end();
	--i; --i;
	static_assert(is_same<decltype(*i--), Valset::value_type>::value, "Dereferencing post-incremented iterator must be convertible to value type");
	BOOST_CHECK_EQUAL((*i--).cast<string>(), "2");
	BOOST_CHECK(i == vs.begin());
	ci = vs.cend();
	--ci; --ci;
	static_assert(is_same<decltype(*ci--), const Valset::value_type>::value, "Dereferencing post-incremented iterator must be convertible to value type");
	BOOST_CHECK_EQUAL((*ci--).cast<string>(), "2");
	BOOST_CHECK(ci == vs.cbegin());

	// RandomAccessIterator: operator +=
	static_assert(is_same<decltype(i += 1), Valset::iterator&>::value, "Iterator's += expression must have iterator& type");
	BOOST_CHECK(&i == &(i += 2));
	BOOST_CHECK_EQUAL(i->cast<int>(), 3);
	static_assert(is_same<decltype(ci += 1), Valset::const_iterator&>::value, "Iterator's += expression must have iterator& type");
	BOOST_CHECK(&ci == &(ci += 2));
	BOOST_CHECK_EQUAL(ci->cast<int>(), 3);

	// RandomAccessIterator operator -=
	static_assert(is_same<decltype(i -= 1), Valset::iterator&>::value, "Iterator's -= expression must have iterator& type");
	BOOST_CHECK(&i == &(i -= 2));
	BOOST_CHECK_EQUAL(i->cast<double>(), 1.1);
	static_assert(is_same<decltype(ci -= 1), Valset::const_iterator&>::value, "Iterator's -= expression must have iterator& type");
	BOOST_CHECK(&ci == &(ci -= 2));
	BOOST_CHECK_EQUAL(ci->cast<double>(), 1.1);

	// RandomAccessIterator: i + n, n + i
	static_assert(is_same<decltype(i + 1), Valset::iterator>::value, "Iterator's plus expression must have iterator type");
	{
		auto j = i + 2;
		BOOST_CHECK_EQUAL(j->cast<int>(), 3);
	}
	static_assert(is_same<decltype(ci + 1), Valset::const_iterator>::value, "Iterator's plus expression must have iterator type");
	{
		auto cj = ci + 2;
		BOOST_CHECK_EQUAL(cj->cast<int>(), 3);
	}

	static_assert(is_same<decltype(1 + i), Valset::iterator>::value, "Iterator's plus expression must have iterator type");
	{
		auto j = 2 + i;
		BOOST_CHECK_EQUAL(j->cast<int>(), 3);
	}
	static_assert(is_same<decltype(1 + ci), Valset::const_iterator>::value, "Iterator's plus expression must have iterator type");
	{
		auto cj = 2 + ci;
		BOOST_CHECK_EQUAL(cj->cast<int>(), 3);
	}

	// RandomAccessIterator: i - n
	static_assert(is_same<decltype(i - 1), Valset::iterator>::value, "Iterator's minus expression must have iterator type");
	{
		auto j = vs.end() - 1;
		BOOST_CHECK_EQUAL(j->cast<int>(), 3);
	}
	static_assert(is_same<decltype(ci - 1), Valset::const_iterator>::value, "Iterator's minus expression must have iterator type");
	{
		auto cj = vs.cend() - 1;
		BOOST_CHECK_EQUAL(cj->cast<int>(), 3);
	}

	// RandomAccessIterator: difference
	static_assert(is_same<decltype(i - i), Valset::difference_type>::value, "Iterator difference must be of difference_type");
	BOOST_CHECK_EQUAL(vs.end() - vs.begin(), 3);
	static_assert(is_same<decltype(ci - ci), Valset::difference_type>::value, "Iterator difference must be of difference_type");
	BOOST_CHECK_EQUAL(vs.cend() - vs.cbegin(), 3);

	// RandomAccessIterator: indexation
	static_assert(is_same<decltype(i[0]), Valset::reference>::value, "Iterator's indexation must be convertible to reference");
	BOOST_CHECK_EQUAL(i[2].cast<int>(), 3);
	static_assert(is_same<decltype(ci[0]), Valset::const_reference>::value, "Iterator's indexation must be convertible to reference");
	BOOST_CHECK_EQUAL(ci[2].cast<int>(), 3);

	// RandomAccessIterator: <
	BOOST_CHECK( (vs.begin() < vs.end()));
	BOOST_CHECK(!(vs.end() < vs.begin()));
	BOOST_CHECK(!(vs.begin() < i));

	BOOST_CHECK( (vs.cbegin() < vs.cend()));
	BOOST_CHECK(!(vs.cend() < vs.cbegin()));
	BOOST_CHECK(!(vs.cbegin() < ci));

	// RandomAccessIterator: >
	BOOST_CHECK(!(vs.begin() > vs.end()));
	BOOST_CHECK( (vs.end() > vs.begin()));
	BOOST_CHECK(!(vs.begin() > i));

	BOOST_CHECK(!(vs.cbegin() > vs.cend()));
	BOOST_CHECK( (vs.cend() > vs.cbegin()));
	BOOST_CHECK(!(vs.cbegin() > ci));

	// RandomAccessIterator: <=
	BOOST_CHECK( (vs.begin() <= vs.end()));
	BOOST_CHECK(!(vs.end() <= vs.begin()));
	BOOST_CHECK( (vs.begin() <= i));

	BOOST_CHECK( (vs.cbegin() <= vs.cend()));
	BOOST_CHECK(!(vs.cend() <= vs.cbegin()));
	BOOST_CHECK( (vs.cbegin() <= ci));

	// RandomAccessIterator: >=
	BOOST_CHECK(!(vs.begin() >= vs.end()));
	BOOST_CHECK( (vs.end() >= vs.begin()));
	BOOST_CHECK( (vs.begin() >= i));

	BOOST_CHECK(!(vs.cbegin() >= vs.cend()));
	BOOST_CHECK( (vs.cend() >= vs.cbegin()));
	BOOST_CHECK( (vs.cbegin() >= ci));
}


BOOST_AUTO_TEST_SUITE_END()
