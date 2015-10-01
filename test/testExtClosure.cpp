#include <boost/test/unit_test.hpp>

#include "fixtures.h"
#include <stdexcept>


using lua::Retval;
using lua::Context;
using lua::Value;
using lua::Valset;
using lua::mkcf;
using std::string;




BOOST_AUTO_TEST_SUITE(ExtClosure)





static Retval giveUpvalue(Context& c)
{
	return c.ret(c.upvalues[1]);
}



BOOST_FIXTURE_TEST_CASE(UpvalueConstExt, fxContext)
{
	Value v = context.closure(lua::mkcf<giveUpvalue>, "Upvalue content");
    BOOST_CHECK_EQUAL(v.upvalue(1).cast<string>(), "Upvalue content");
    v.upvalue(1) = "Updated upvalue content";
    BOOST_CHECK_EQUAL(v().cast<string>(), "Updated upvalue content");
}



BOOST_FIXTURE_TEST_CASE(UpvalueTempExt, fxContext)
{
	context.global["fn"] = context.closure(lua::mkcf<giveUpvalue>, "Upvalue content");
	BOOST_REQUIRE_EQUAL(context.getTop(), 0);
    BOOST_CHECK_EQUAL(context.global["fn"].upvalue(1).cast<string>(), "Upvalue content");
	BOOST_REQUIRE_EQUAL(context.getTop(), 0);
    context.global["fn"].upvalue(1) = "Updated upvalue content";
	BOOST_REQUIRE_EQUAL(context.getTop(), 0);
    BOOST_CHECK_EQUAL(context.global["fn"]().cast<string>(), "Updated upvalue content");
	BOOST_REQUIRE_EQUAL(context.getTop(), 0);
}



#ifndef LUAPP_COMPATIBILITY_NO_NRVO
// getUpvalues depends on NRVO
BOOST_FIXTURE_TEST_CASE(ReadUpvaluesConst, fxContext)
{
	context.runString("local uv = \"ExtUpvalue\"; function fn() return uv; end");
	Value v = context.global["fn"];
	Valset vs = v.getUpvalues();
	BOOST_CHECK_EQUAL(vs.size(), 2);
	BOOST_CHECK_EQUAL(vs[0].cast<string>(), "ExtUpvalue");
	BOOST_CHECK_EQUAL(vs[1].cast<string>(), "uv");
}



BOOST_FIXTURE_TEST_CASE(ReadUpvaluesTemp, fxContext)
{
	context.runString("local uv = \"ExtUpvalue\"; function fn() return uv; end");
	Valset vs = context.global["fn"].getUpvalues();
	BOOST_CHECK_EQUAL(vs.size(), 2);
	BOOST_CHECK_EQUAL(vs[0].cast<string>(), "ExtUpvalue");
	BOOST_CHECK_EQUAL(vs[1].cast<string>(), "uv");
}
#endif	// LUAPP_COMPATIBILITY_NO_NRVO



BOOST_FIXTURE_TEST_CASE(ClosureInfoConst, fxContext)
{
	context.runString("local uv = \"ExtUpvalue\"; function fn(a, b, c) return uv; end");
	Value v = context.global["fn"];
	{
		const auto result = v.getClosureInfo();
#if(LUAPP_API_VERSION > 51)
		BOOST_CHECK_EQUAL(result.nUpvalues, 1);
		BOOST_CHECK_EQUAL(result.nParameters, 3);
		BOOST_CHECK_EQUAL(result.variadic, false);
#else
		BOOST_CHECK_EQUAL(result, 1);
#endif
	}

	v = context.closure(mkcf<giveUpvalue>, "one", "two");
	{
		const auto result = v.getClosureInfo();
#if(LUAPP_API_VERSION > 51)
		BOOST_CHECK_EQUAL(result.nUpvalues, 2);
		BOOST_CHECK_EQUAL(result.nParameters, 0);
		BOOST_CHECK_EQUAL(result.variadic, true);
#else
		BOOST_CHECK_EQUAL(result, 2);
#endif
	}
}



BOOST_FIXTURE_TEST_CASE(ClosureInfoTemp, fxContext)
{
	context.runString("local uv = \"ExtUpvalue\"; function fn(a, b, c) return uv; end");
	{
		const auto result = context.global["fn"].getClosureInfo();
#if(LUAPP_API_VERSION > 51)
		BOOST_CHECK_EQUAL(result.nUpvalues, 1);
		BOOST_CHECK_EQUAL(result.nParameters, 3);
		BOOST_CHECK_EQUAL(result.variadic, false);
#else
		BOOST_CHECK_EQUAL(result, 1);
#endif
	}

	context.global["fn"] = context.closure(mkcf<giveUpvalue>, "one", "two");
	{
		const auto result = context.global["fn"].getClosureInfo();
#if(LUAPP_API_VERSION > 51)
		BOOST_CHECK_EQUAL(result.nUpvalues, 2);
		BOOST_CHECK_EQUAL(result.nParameters, 0);
		BOOST_CHECK_EQUAL(result.variadic, true);
#else
		BOOST_CHECK_EQUAL(result, 2);
#endif
	}
}



BOOST_AUTO_TEST_SUITE_END()
