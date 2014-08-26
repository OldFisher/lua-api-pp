#include <boost/test/unit_test.hpp>

#include "fixtures.h"
#include <stdexcept>

using std::string;

using lua::Retval;
using lua::Context;
using lua::Value;
using lua::Table;
using lua::Valset;
using lua::mkcf;


struct Udata{int x;};
LUAPP_USERDATA(Udata, "Test.Userdata")


struct SpecialType{int x;};
LUAPP_ARG_CONVERT(SpecialType, {return SpecialType{val.cast<int>()};})
LUAPP_RV_CONVERT(SpecialType, {return context.ret(val.x);})



BOOST_AUTO_TEST_SUITE(Wrappers)



static int signal = 0;

static Retval fnSignal(Context& c)
{
	signal = 1;
	return c.ret();
}

static Retval fnThrow(Context& c)
{
	throw std::runtime_error("Just as planned");
}

BOOST_FIXTURE_TEST_CASE(mkcf_, fxState)
{
	signal = 0;
	gs.call(lua::mkcf<fnSignal>);
	BOOST_CHECK_EQUAL(signal, 1);
	Context context(gs.getRawState(), Context::initializeExplicitly);
	context.global["fn"] = mkcf<fnThrow>;
	Valset vs = context.global["fn"].pcall();
	BOOST_REQUIRE(!vs.success());
	BOOST_CHECK_EQUAL(vs[0].cast<string>(), "Lua function terminated with an exception: Just as planned");
}



static Retval fnAutoPromoted(Context& c)
{
	const int u1 = c.upvalues[2];
	const int u2 = c.upvalues[3];
	const int a1 = c.args.at(0);
	return c.ret(a1 * (u2 - u1));
}


BOOST_FIXTURE_TEST_CASE(LFunctionPromotion, fxContext)
{
	context.global["fn"] = fnSignal;
	BOOST_CHECK(context.global["fn"].is<lua::CFunction>());
	context.global["fn"].call();
	BOOST_CHECK_EQUAL(signal, 1);

	context.global["fn"] = context.closure(fnAutoPromoted, 3, 6);
	BOOST_CHECK(context.global["fn"].is<lua::CFunction>());
	const int rv = context.global["fn"].call(4);
	BOOST_CHECK_EQUAL(rv, 12);
}



static int wrapped1(int x)
{
	return x*3;
}

static void wrapped0(const Udata& x)
{
	signal = x.x;
}

static int vwrapped(const string& x, int y)
{
	signal = x.size() + y;
	return 0;
}

BOOST_FIXTURE_TEST_CASE(SimpleWrappers, fxContext)
{
	signal = 0;
	context.mt<Udata>() = Table::records(context);
	context.global["fn"] = context.wrap(wrapped1);
	BOOST_CHECK(context.global["fn"].is<lua::CFunction>());
	{
		Valset vs = context.global["fn"].pcall(3);
		BOOST_CHECK(vs.success());
		BOOST_REQUIRE_EQUAL(vs.size(), 1);
		BOOST_CHECK(vs[0].is<int>());
	}
	BOOST_CHECK_EQUAL(context.global["fn"](3).cast<int>(), 9);
	{
		Valset vs = context.global["fn"].pcall();
		BOOST_CHECK(!vs.success());
	}

	context.global["fn"] = context.wrap(wrapped0);
	context.global["fn"](Udata{3});
	BOOST_CHECK_EQUAL(signal, 3);
	{
		Valset vs = context.global["fn"].pcall(1);
		BOOST_REQUIRE(!vs.success());
	}

	context.global["fn"] = context.vwrap(vwrapped);
	context.global["fn"]("12345", 4);
	BOOST_CHECK_EQUAL(signal, 9);
#ifdef LUAPP_AUTOWRAP
	context.global["fn"] = wrapped1;
	BOOST_CHECK(context.global["fn"].is<lua::CFunction>());
	{
		Valset vs = context.global["fn"].pcall(3);
		BOOST_CHECK(vs.success());
		BOOST_REQUIRE_EQUAL(vs.size(), 1);
		BOOST_CHECK(vs[0].is<int>());
	}
	BOOST_CHECK_EQUAL(context.global["fn"](3).cast<int>(), 9);
	{
		Valset vs = context.global["fn"].pcall();
		BOOST_CHECK(!vs.success());
	}
#endif // LUAPP_AUTOWRAP
}



static SpecialType swrapped1(SpecialType x) {return SpecialType{x.x*3};}
static void swrapped0(const SpecialType& x) {signal = x.x;}
static int svwrapped(const SpecialType& x, int y) {signal = x.x + y; return 0;}

BOOST_FIXTURE_TEST_CASE(CustomWrappers, fxContext)
{
	context.global["fn"] = context.wrap(swrapped1);
	BOOST_CHECK_EQUAL(context.global["fn"](3).cast<int>(), 9);
	{
		Valset vs = context.global["fn"].pcall();
		BOOST_CHECK(!vs.success());
	}

	signal = 0;
	context.global["fn"] = context.wrap(swrapped0);
	context.global["fn"](3);
	BOOST_CHECK_EQUAL(signal, 3);

	context.global["fn"] = context.vwrap(svwrapped);
	context.global["fn"](5, 4);
	BOOST_CHECK_EQUAL(signal, 9);
}




BOOST_AUTO_TEST_SUITE_END()
