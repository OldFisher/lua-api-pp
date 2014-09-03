#include <boost/test/unit_test.hpp>

#include "fixtures.h"
#include <stdexcept>

using lua::Valref;
using lua::Value;
using lua::Table;
using std::string;


struct Udata{int x;};
LUAPP_USERDATA(Udata, "Test.Userdata")




BOOST_AUTO_TEST_SUITE(TableObject)



BOOST_FIXTURE_TEST_CASE(Creation, fxContext)
{
	context.runString("val = {1, 2, 3}");
	{
		Table t = context.global["val"];
		BOOST_CHECK_EQUAL(t[2].cast<int>(), 2);
		BOOST_CHECK_EQUAL(context.getTop(), 1);
	}
	{
		Value v = context.global["val"];
		Table t = v;
		BOOST_CHECK_EQUAL(t[2].cast<int>(), 2);
		BOOST_CHECK_EQUAL(context.getTop(), 2);
	}
	{
		Table tsrc = context.global["val"];
		Table t = tsrc;
		BOOST_CHECK_EQUAL(t[2].cast<int>(), 2);
		BOOST_CHECK_EQUAL(context.getTop(), 2);
	}
	{
		Table t(context, 3, 4);
		BOOST_CHECK(static_cast<Valref&>(t).is<Table>());
	}
	{
		Value v = context.global["val"];
		Table t = v;
		BOOST_CHECK(static_cast<Valref&>(t).is<Table>());
	}
	{
		lua::Valset vs(context);
		vs.push_back(context.global["val"]);
		Table t = vs[0];
		BOOST_CHECK(static_cast<Valref&>(t).is<Table>());
	}
}



static Table r1(lua::Context& context)
{
	return Table(context);
}

static Table r2(lua::Context& context)
{
	Table t(context);
	return t;
}

BOOST_FIXTURE_TEST_CASE(ReturnFromFunction, fxContext)
{
	Table t1 = r1(context);
	BOOST_CHECK_EQUAL(context.getTop(), 1);
	Table t2 = r2(context);
	BOOST_CHECK(static_cast<Valref&>(t1).is<Table>());
	BOOST_CHECK(static_cast<Valref&>(t2).is<Table>());
	BOOST_CHECK_EQUAL(context.getTop(), 2);
}




BOOST_FIXTURE_TEST_CASE(Comparison, fxContext)
{
	context.runString("val = {1, 2, 3}");
	Table t1 = context.global["val"];
	Table t2(context);
	Value t1a = t1;
	BOOST_CHECK(t1 == t1a);
	BOOST_CHECK(!(t1 != t1a));
	BOOST_CHECK(t1 != t2);
	BOOST_CHECK(!(t1 == t2));
}



BOOST_FIXTURE_TEST_CASE(Miscellaneous, fxContext)
{
	Table mt(context);
	context.runString("val = {1, 2, 3}");
	Table t = context.global["val"];
	t.mt() = mt;
	BOOST_CHECK(t.mt() == mt);
#if(LUAPP_API_VERSION >= 52)
	BOOST_CHECK_EQUAL(t.len().cast<int>(), 3);
#else	// V51-
	BOOST_CHECK_EQUAL(t.len(), 3);
#endif	// V52+
}



BOOST_FIXTURE_TEST_CASE(Iteration, fxContext)
{
	context.runString("val = {1, 2, 3}");
	Table t = context.global["val"];
	t.iterate([] (Valref k, Valref v) {BOOST_CHECK_EQUAL(k.cast<double>(), v.cast<double>()); return true;});
}



BOOST_FIXTURE_TEST_CASE(ArrayCreation, fxContext)
{
	{
		Table t = Table::array(context, 1, 2, 3);
		int count = 0;
		t.iterate([&] (Valref k, Valref v) {
			++count;
			const int kk = k;
			const int vv = v;
			BOOST_CHECK_EQUAL(kk, vv);
			BOOST_CHECK_GE(kk, 1);
			BOOST_CHECK_LE(kk, 3);
			return true;
		});
		BOOST_CHECK_EQUAL(count, 3);
	}
	{
		lua::Valset vs(context);
		vs.push_back(2);
		vs.push_back(3);
		vs.push_back(4);
		BOOST_CHECK_EQUAL(context.getTop(), 3);
		Table t = Table::array(context, 1, vs, 5);
		int count = 0;
#if(LUAPP_API_VERSION >= 52)
		BOOST_CHECK_EQUAL(t.len().cast<int>(), 5);
#else	// V51-
		BOOST_CHECK_EQUAL(t.len(), 5);
#endif	// V52+
		t.iterate([&] (Valref k, Valref v) {
			++count;
			const int kk = k;
			const int vv = v;
			BOOST_CHECK_EQUAL(kk, vv);
			BOOST_CHECK_GE(kk, 1);
			BOOST_CHECK_LE(kk, 5);
			return true;
		});
		BOOST_CHECK_EQUAL(count, 5);
	}
}



BOOST_FIXTURE_TEST_CASE(RecordCreation, fxContext)
{
	Table t = Table::records(context, 1, 1, 2, 2, 3, 3);
	int count = 0;
	t.iterate([&] (Valref k, Valref v) {
		++count;
		const int kk = k;
		const int vv = v;
		BOOST_CHECK_EQUAL(kk, vv);
		BOOST_CHECK_GE(kk, 1);
		BOOST_CHECK_LE(kk, 3);
		return true;
	});
	BOOST_CHECK_EQUAL(count, 3);
}


static int signal = 0;
static int setSignal(lua_State*)
{
	++signal;
	return 0;
}

BOOST_FIXTURE_TEST_CASE(RawAccess, fxContext)
{
	Table t(context);
	t.mt() = Table::records(context, "__index", setSignal, "__newindex", setSignal);
	t.raw[1] = 1;
	BOOST_CHECK_EQUAL(t.raw[1].cast<int>(), 1);
	BOOST_CHECK_EQUAL(signal, 0);
}



BOOST_AUTO_TEST_SUITE_END()
