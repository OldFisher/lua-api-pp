#include <boost/test/unit_test.hpp>

#include "fixtures.h"
#include <cstring>
#include <stdexcept>

using lua::Retval;
using lua::Context;
using lua::Valset;
using lua::Table;
using lua::mkcf;
using lua::Nil;



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


BOOST_AUTO_TEST_SUITE_END()
