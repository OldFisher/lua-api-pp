#include <boost/test/unit_test.hpp>

#include "fixtures.h"
#include <cstring>
#include <stdexcept>

using std::string;

using lua::Retval;
using lua::Context;
using lua::Valset;
using lua::Table;
using lua::mkcf;
using lua::Nil;
using lua::nil;



struct Udata;
LUAPP_USERDATA(Udata, "Test.Userdata")



BOOST_AUTO_TEST_SUITE(ContextAccessors)



BOOST_FIXTURE_TEST_CASE(GlobalAccessor, fxContext)
{
	context.global["val"] = 3.14;
	BOOST_CHECK(context.global["val"].is<double>());
	BOOST_CHECK_EQUAL(context.global["val"].cast<double>(), 3.14);
	BOOST_CHECK(context.global["nosuchval"].is<Nil>());
}



BOOST_FIXTURE_TEST_CASE(RegistryAccessor, fxContext)
{
	Table t(context);
	context.mt<Udata>() = t;
	BOOST_CHECK(t == context.registry["Test.Userdata"]);
	Table t2(context);
	BOOST_CHECK(t != t2);
	context.registry["Test.Userdata"] = t2;
	BOOST_CHECK(context.registry["Test.Userdata"] == t2);
	BOOST_CHECK(context.mt<Udata>() == t2);

	const auto key = context.registry.store(t2);
	BOOST_CHECK(context.registry[key] == t2);
	context.registry[key] = t;
	BOOST_CHECK(context.registry[key] == t);
}



static Retval argChecker(Context& c)
{
	return c.ret(c.checkArgs<double, const char*, lua::LFunction>(4));
}

BOOST_FIXTURE_TEST_CASE(ArgumentTypeCheck, fxContext)
{
	context.global["f"] = mkcf<argChecker>;
	BOOST_CHECK(!context.global["f"]().cast<bool>());
	BOOST_CHECK(!context.global["f"](1, nil, context.global["f"], true, false).cast<bool>());
	BOOST_CHECK(context.global["f"](1, "2", context.global["f"], true).cast<bool>());
}



static Retval argRequire(Context& c)
{
	c.requireArgs<double, const char*, lua::LFunction>(4);
	return c.ret();
}

BOOST_FIXTURE_TEST_CASE(ArgumentTypeRequire, fxContext)
{
	context.global["f"] = mkcf<argRequire>;
	{
		Valset rv = context.global["f"].pcall();
		BOOST_CHECK(!rv.success());
		BOOST_CHECK(rv[0].cast<string>().find("Insufficient number of arguments (4 expected, 0 passed).") != string::npos);
	}
	{
		Valset rv = context.global["f"].pcall(1, "2", nil, true, false);
		BOOST_CHECK(!rv.success());
		BOOST_CHECK(rv[0].cast<string>().find("Argument 3 type is incompatible.") != string::npos);
	}
	{
		Valset rv = context.global["f"].pcall(1, "2", context.global["f"], true, false);
		BOOST_CHECK(rv.success());
	}
}


BOOST_AUTO_TEST_SUITE_END()
