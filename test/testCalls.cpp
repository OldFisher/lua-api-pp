#include <boost/test/unit_test.hpp>

#include "fixtures.h"
#include <stdexcept>


using std::string;


struct Udata {int x;};
LUAPP_USERDATA(Udata, "Test.Userdata")


struct fxCall: public fxContext{
	fxCall()
	{
		context.mt<Udata>() = lua::Table::records(context);
	}
};

#define l context.global["fn"]


BOOST_AUTO_TEST_SUITE(Calls)



BOOST_FIXTURE_TEST_CASE(Argument0, fxCall)
{
	context.runString("function fn(...) local args = {...}; return (#args == 0); end");
	lua::Value v = l;
	BOOST_CHECK_EQUAL(v().cast<bool>(), true);
	BOOST_CHECK_EQUAL(l().cast<bool>(), true);
	BOOST_CHECK_EQUAL(v(1).cast<bool>(), false);
	BOOST_CHECK_EQUAL(l(v).cast<bool>(), false);

	BOOST_CHECK_EQUAL(v.call().cast<bool>(), true);
	BOOST_CHECK_EQUAL(l.call().cast<bool>(), true);
	BOOST_CHECK_EQUAL(v.call(1).cast<bool>(), false);
	BOOST_CHECK_EQUAL(l.call(v).cast<bool>(), false);

	BOOST_CHECK_EQUAL(v.pcall().cast<bool>(), true);
	BOOST_CHECK_EQUAL(l.pcall().cast<bool>(), true);
	BOOST_CHECK_EQUAL(v.pcall(1).cast<bool>(), false);
	BOOST_CHECK_EQUAL(l.pcall(v).cast<bool>(), false);
}




BOOST_FIXTURE_TEST_CASE(PcallFailure, fxContext)
{
	context.runString("function fn() error(\"Nice little error\") end");
	BOOST_CHECK(context.global["fn"].pcall().cast<string>().find("Nice little error") != string::npos);
}



BOOST_FIXTURE_TEST_CASE(Argument1, fxCall)
{
	context.runString("function fn(...) local args = {...}; return (#args == 1) and type(args[1]) or \"size error\"; end");
	lua::Value v = l;
	BOOST_CHECK_EQUAL(v().cast<string>(), "size error");
	BOOST_CHECK_EQUAL(v(1).cast<string>(), "number");
	BOOST_CHECK_EQUAL(l(v).cast<string>(), "function");
	BOOST_CHECK_EQUAL(l("a string").cast<string>(), "string");
	BOOST_CHECK_EQUAL(l(true).cast<string>(), "boolean");
	context.runString("val = {}");
	BOOST_CHECK_EQUAL(l(context.global["val"]).cast<string>(), "table");
	BOOST_CHECK_EQUAL(l(Udata{7}).cast<string>(), "userdata");
}



BOOST_FIXTURE_TEST_CASE(ArgumentMany, fxCall)
{
	context.runString("function fn(...) local args = {...}; local rv = \"\"; for k = 1, #args do rv = rv .. type(args[k]) .. \" \"; end; return rv; end");
	lua::Value v = l;
	BOOST_CHECK_EQUAL(v().cast<string>(), "");
	BOOST_CHECK_EQUAL(v(1).cast<string>(), "number ");
	BOOST_CHECK_EQUAL(l(v, true).cast<string>(), "function boolean ");
	context.runString("val = {}");
	BOOST_CHECK_EQUAL(l("a string", context.global["val"], Udata{7}).cast<string>(), "string table userdata ");
}



lua::Retval proxyMulti(lua::Context& context)
{
	return context.ret(context.global["giveMulti"]());
}

BOOST_FIXTURE_TEST_CASE(MultipleResultExpansion, fxCall)
{
	context.runString("function giveMulti() return 2, 3, 4; end");
	context.runString("function giveNone() end");
    context.runString("function fn(...) local args = {...}; local rv = \"\"; for k = 1, #args do rv = rv .. args[k] .. \" \"; end; return rv; end");
    BOOST_CHECK_EQUAL(l("1", context.global["giveMulti"](), 5).cast<string>(), "1 2 3 4 5 ");
    BOOST_CHECK_EQUAL(l("1", context.global["giveNone"](), 5).cast<string>(), "1 5 ");
	context.global["proxyMulti"] = lua::mkcf<proxyMulti>;
    BOOST_CHECK_EQUAL(l("1", context.global["proxyMulti"](), 5).cast<string>(), "1 2 3 4 5 ");
    lua::Valset vs(context);
    vs.push_back(2);
    vs.push_back(3);
    vs.push_back(4);
    BOOST_CHECK_EQUAL(l("1", vs, 5).cast<string>(), "1 2 3 4 5 ");
}



BOOST_FIXTURE_TEST_CASE(Discard, fxCall)
{
    context.runString("function fn() signal = \"Okay\"; end");
    l();
    BOOST_CHECK_EQUAL(context.global["signal"].cast<string>(), "Okay");
}



BOOST_FIXTURE_TEST_CASE(SingleResult, fxCall)
{
	context.runString("function fn() return function() return \"Okay\"; end, function() return \"Not okay\"; end; end");
	BOOST_CHECK_EQUAL(l()().cast<string>(), "Okay");
}



BOOST_AUTO_TEST_SUITE_END()
