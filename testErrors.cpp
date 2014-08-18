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



BOOST_AUTO_TEST_SUITE(ContextErrorReporting)



static Retval testWhere(Context& c)
{
	return c.ret(c.where());
}

BOOST_FIXTURE_TEST_CASE(Where, fxContext)
{
	Value v = context.closure(mkcf<testWhere>)();
	BOOST_CHECK(v.is<string>());
	BOOST_CHECK_EQUAL(v.cast<string>(), "");
}



static Retval testErrMsg(Context& c)
{
	Value rv("Error ", c);
	return c.error(rv & "message");
}

BOOST_FIXTURE_TEST_CASE(ErrorMessage, fxContext)
{
	Valset rv = context.closure(mkcf<testErrMsg>).pcall();
	BOOST_CHECK(!rv.success());
	BOOST_CHECK(rv[0].is<string>());
	BOOST_CHECK_EQUAL(rv[0].cast<string>(), "Error message");
}



static Retval testErrDefault(Context& c)
{
	return c.error();
}

BOOST_FIXTURE_TEST_CASE(ErrorDefault, fxContext)
{
	Valset rv = context.closure(mkcf<testErrDefault>).pcall();
	BOOST_CHECK(!rv.success());
	BOOST_CHECK(rv[0].is<string>());
	BOOST_CHECK_EQUAL(rv[0].cast<string>(), "");
}

// Catching thrown errors is covered by mkcf tests in testWrappers.cpp

BOOST_AUTO_TEST_SUITE_END()
