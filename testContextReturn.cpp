#include <boost/test/unit_test.hpp>

#include "fixtures.h"
#include <stdexcept>

using std::string;

using lua::Retval;
using lua::Context;
using lua::Valset;
using lua::Value;
using lua::Table;
using lua::mkcf;
using lua::Nil;
using lua::nil;
using lua::CFunction;



struct Udata{int x;};
LUAPP_USERDATA(Udata, "Test.Userdata")



BOOST_AUTO_TEST_SUITE(ContextReturn)


static Retval ret0(Context& context) { return context.ret(); }
static Retval ret1(Context& context) { return context.ret(1); }
static Retval ret3(Context& context) { return context.ret(1, 2, 3); }



BOOST_FIXTURE_TEST_CASE(returnAmount, fxSignal)
{
	gs.runString("function Test(fn) local rv = {fn()}; if #rv == 0 then fnSignal() end end");
	context.global["Test"](mkcf<ret0>, 0);
	BOOST_CHECK(isSignaled());

	gs.runString("function Test(fn) local rv = {fn()}; if #rv == 1 and rv[1] == 1 then fnSignal() end end");
	context.global["Test"](mkcf<ret1>, 1);
	BOOST_CHECK(isSignaled());

	gs.runString("function Test(fn) local rv = {fn()}; if #rv == 3 and rv[1] == 1 and rv[2] == 2 and rv[3] == 3 then fnSignal() end end");
	context.global["Test"](mkcf<ret3>, 3);
	BOOST_CHECK(isSignaled());
}



static Retval doReturn(Context& c)
{
	c.global["val"] = 3.14;
	Valset vs(c);
	vs.push_back(42);
	vs.push_back(43);
	Value v(6.4, c);
	Table t = Table::array(c, 2.718);
	return c.ret(
		nil,
		true,
		1,
		2u,
		3.0f,
		4.0,
		"a string",
		string("another string"),
		mkcf<doReturn>,
		Udata{108},
		vs,
		vs[0],
		v,
		t,
		c.global["val"],
		-64ll,
		65ull
	);
}

BOOST_FIXTURE_TEST_CASE(ReturnedTypes, fxContext)
{
	context.mt<Udata>() = Table::records(context);
	Valset rv = context.closure(mkcf<doReturn>)();
	BOOST_CHECK_EQUAL(rv.size(), 18);
	BOOST_CHECK(rv[0].is<Nil>());
	BOOST_CHECK(rv[1].is<bool>());
	BOOST_CHECK(rv[1].cast<bool>());
	BOOST_CHECK(rv[2].is<int>());
	BOOST_CHECK_EQUAL(rv[2].cast<int>(), 1);
	BOOST_CHECK(rv[3].is<unsigned int>());
	BOOST_CHECK_EQUAL(rv[3].cast<unsigned int>(), 2u);
	BOOST_CHECK(rv[4].is<float>());
	BOOST_CHECK_EQUAL(rv[4].cast<float>(), 3.0f);
	BOOST_CHECK(rv[5].is<double>());
	BOOST_CHECK_EQUAL(rv[5].cast<double>(), 4.0);
	BOOST_CHECK(rv[6].is<string>());
	BOOST_CHECK_EQUAL(rv[6].cast<string>(), "a string");
	BOOST_CHECK(rv[7].is<string>());
	BOOST_CHECK_EQUAL(rv[7].cast<string>(), "another string");
	BOOST_CHECK(rv[8].is<CFunction>());
	BOOST_CHECK(rv[8].cast<CFunction>() == &mkcf<doReturn>);
	BOOST_CHECK(rv[9].is<Udata>());
	BOOST_CHECK_EQUAL(rv[9].cast<Udata>().x, 108);
	BOOST_CHECK(rv[10].is<int>());
	BOOST_CHECK_EQUAL(rv[10].cast<int>(), 42);
	BOOST_CHECK(rv[11].is<int>());
	BOOST_CHECK_EQUAL(rv[11].cast<int>(), 43);
	BOOST_CHECK(rv[12].is<int>());
	BOOST_CHECK_EQUAL(rv[12].cast<int>(), 42);
	BOOST_CHECK(rv[13].is<double>());
	BOOST_CHECK_EQUAL(rv[13].cast<double>(), 6.4);
	BOOST_CHECK(rv[14].is<Table>());
	BOOST_CHECK(rv[15].is<double>());
	BOOST_CHECK_EQUAL(rv[15].cast<double>(), 3.14);
	BOOST_CHECK(rv[16].is<long long>());
	BOOST_CHECK_EQUAL(rv[16].cast<long long>(), -64ll);
	BOOST_CHECK(rv[17].is<unsigned long long>());
	BOOST_CHECK_EQUAL(rv[17].cast<unsigned long long>(), 65ull);
}



BOOST_AUTO_TEST_SUITE_END()
