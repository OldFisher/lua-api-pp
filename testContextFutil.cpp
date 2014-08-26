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



BOOST_AUTO_TEST_SUITE(ContextFunctionUtility)



static Retval closureTest(Context& context)
{
	return context.ret(
		context.args.size() == 3 &&
		context.args[0].is<int>() &&
		context.args[0].cast<int>() == 1 &&
		context.args[1].is<string>() &&
		context.args[1].cast<string>() == "a string" &&
		context.args[2].is<Udata>() &&
		context.args[2].cast<Udata>().x == 3 &&
		context.upvalues[1].is<int>() &&
		context.upvalues[1].cast<int>() == 1 &&
		context.upvalues[2].is<string>() &&
		context.upvalues[2].cast<string>() == "a string" &&
		context.upvalues[3].is<Udata>() &&
		context.upvalues[3].cast<Udata>().x == 3
	);
}

BOOST_FIXTURE_TEST_CASE(CreateClosure, fxContext)
{
	context.mt<Udata>() = Table::records(context);
	Value c = context.closure(mkcf<closureTest>, 1, "a string", Udata{3});
	const bool result = c(1, "a string", Udata{3});
	BOOST_CHECK(result);
}



BOOST_FIXTURE_TEST_CASE(CreateChunkFromString, fxContext)
{
	Value c = context.chunk("return 1, \"a string\"");
	Valset result = c.pcall();
	BOOST_CHECK(result.success());
	BOOST_CHECK_EQUAL(result.size(), 2);
	BOOST_CHECK_EQUAL(result[0].cast<int>(), 1);
	BOOST_CHECK_EQUAL(result[1].cast<string>(), "a string");
	BOOST_CHECK_THROW(Value v = context.chunk("{"), std::runtime_error);		// Chunk creators are discardable
}



BOOST_FIXTURE_TEST_CASE(CreateChunkFromStdString, fxContext)
{
	Value c = context.chunk(string("return 1, \"a string\""));
	Valset result = c.pcall();
	BOOST_CHECK(result.success());
	BOOST_CHECK_EQUAL(result.size(), 2);
	BOOST_CHECK_EQUAL(result[0].cast<int>(), 1);
	BOOST_CHECK_EQUAL(result[1].cast<string>(), "a string");
	BOOST_CHECK_THROW(Value v = context.chunk(string("{")), std::runtime_error);		// Chunk creators are discardable
}



BOOST_FIXTURE_TEST_CASE(CreateChunkFromFile, fxContext)
{
	context.runString("function fnSignal() signal = true end");
	Value c = context.load("test_good.lua");
	c();
	BOOST_CHECK(context.global["signal"].cast<bool>());
	BOOST_CHECK_THROW(Value v = context.load("test_bad.lua"), std::runtime_error);	// Chunk creators are discardable
	BOOST_CHECK_THROW(Value v = context.load("nosuchfile.lua"), std::runtime_error);
}



BOOST_FIXTURE_TEST_CASE(CreateChunkFromFileStdStr, fxContext)
{
	context.runString(string("function fnSignal() signal = true end"));
	Value c = context.load(string("test_good.lua"));
	c();
	BOOST_CHECK(context.global["signal"].cast<bool>());
	BOOST_CHECK_THROW(Value v = context.load(string("test_bad.lua")), std::runtime_error);	// Chunk creators are discardable
	BOOST_CHECK_THROW(Value v = context.load(string("nosuchfile.lua")), std::runtime_error);
}



BOOST_FIXTURE_TEST_CASE(RunString, fxContext)
{
	BOOST_CHECK_THROW(context.runString("{"), std::runtime_error);
	BOOST_CHECK_THROW(context.runString(string("{")), std::runtime_error);
}



BOOST_FIXTURE_TEST_CASE(RunFile, fxContext)
{
	context.runString("function fnSignal() signal = true end");
	context.runFile("test_good.lua");
	BOOST_CHECK(context.global["signal"].cast<bool>());
	BOOST_CHECK_THROW(context.runFile("test_bad.lua"), std::runtime_error);
	BOOST_CHECK_THROW(context.runFile("nosuchfile.lua"), std::runtime_error);
}



BOOST_FIXTURE_TEST_CASE(RunFileStdStr, fxContext)
{
	context.runString(string("function fnSignal() signal = true end"));
	context.runFile(string("test_good.lua"));
	BOOST_CHECK(context.global["signal"].cast<bool>());
	BOOST_CHECK_THROW(context.runFile(string("test_bad.lua")), std::runtime_error);
	BOOST_CHECK_THROW(context.runFile(string("nosuchfile.lua")), std::runtime_error);
}



static int signal = 0;
static int wrapped1(int x) {return x*3;}
static void wrapped0(const Udata& x) {signal = x.x;}
static int vwrapped(const string& x, int y) {signal = x.size() + y; return 0;}


BOOST_FIXTURE_TEST_CASE(SimpleWrappers, fxContext)
{
	context.mt<Udata>() = Table::records(context);
	context.global["fn"] = context.wrap(wrapped1);
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
