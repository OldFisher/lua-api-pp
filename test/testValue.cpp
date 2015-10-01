#include <boost/test/unit_test.hpp>

#include "fixtures.h"
#include <stdexcept>

using lua::Value;
using std::string;


static int cf(lua_State*) {return 0;}

struct Udata{int x;};
LUAPP_USERDATA(Udata, "Test.Userdata")


BOOST_AUTO_TEST_SUITE(ValueObject)



BOOST_FIXTURE_TEST_CASE(CreateFromReference, fxGlobalVal)
{
	BOOST_CHECK_EQUAL(context.getTop(), 1);
	{
		Value v = vs[0];
		BOOST_CHECK_EQUAL(context.getTop(), 2);
		BOOST_CHECK_EQUAL(v.cast<double>(), 3.14);
		{
			Value v2 = v;
			BOOST_CHECK_EQUAL(context.getTop(), 3);
			BOOST_CHECK_EQUAL(v2.cast<double>(), 3.14);
		}
		BOOST_CHECK_EQUAL(context.getTop(), 2);
	}
	BOOST_CHECK_EQUAL(context.getTop(), 1);
}



BOOST_FIXTURE_TEST_CASE(CreateFromNative, fxContext)
{
	{
		Value v(true, context);
		BOOST_CHECK(v.type() == lua::ValueType::Boolean);
		BOOST_CHECK_EQUAL(v.cast<bool>(), true);
	}
	{
		Value v(42, context);
		BOOST_CHECK(v.type() == lua::ValueType::Number);
		BOOST_CHECK_EQUAL(v.cast<int>(), 42);
	}	{
		Value v(42u, context);
		BOOST_CHECK(v.type() == lua::ValueType::Number);
		BOOST_CHECK_EQUAL(v.cast<unsigned int>(), 42u);
	}
	{
		Value v(4.2f, context);
		BOOST_CHECK(v.type() == lua::ValueType::Number);
		BOOST_CHECK_EQUAL(v.cast<float>(), 4.2f);
	}
	{
		Value v(4.2, context);
		BOOST_CHECK(v.type() == lua::ValueType::Number);
		BOOST_CHECK_EQUAL(v.cast<double>(), 4.2);
	}
	{
		const char* const src = "stringval";
		Value v(src, context);
		BOOST_CHECK(v.type() == lua::ValueType::String);
		BOOST_CHECK_EQUAL(v.cast<string>(), src);
	}
	{
		Value v("stringval", context);
		BOOST_CHECK(v.type() == lua::ValueType::String);
		BOOST_CHECK_EQUAL(v.cast<string>(), "stringval");
	}
	{
		Value v(string("stringval"), context);
		BOOST_CHECK(v.type() == lua::ValueType::String);
		BOOST_CHECK_EQUAL(v.cast<string>(), "stringval");
	}
	{
		Value v(cf, context);
		BOOST_CHECK(v.type() == lua::ValueType::C_Function);
		BOOST_CHECK_EQUAL(v.cast<lua::CFunction>(), cf);
	}
	{
		context.mt<Udata>() = lua::Table::records(context);
		Value v(Udata{42}, context);
		BOOST_CHECK(v.type() == lua::ValueType::UserData);
		BOOST_CHECK_EQUAL(v.cast<Udata>().x, 42);
	}
}



BOOST_FIXTURE_TEST_CASE(CreateFromTemporary, fxContext)
{
	BOOST_CHECK_EQUAL(context.getTop(), 0);
	{
		context.runString("val = 3.14");
		Value v = context.global["val"];
		BOOST_CHECK_EQUAL(context.getTop(), 1);
		BOOST_CHECK_EQUAL(v.cast<double>(), 3.14);
		{
			context.runString("function fn() return function() return \"Okay\"; end, function() return \"Not okay\"; end; end");
			Value v2 = context.global["fn"]();
			BOOST_CHECK_EQUAL(context.getTop(), 2);
			BOOST_CHECK_EQUAL(v2().cast<string>(), "Okay");
		}
		BOOST_CHECK_EQUAL(context.getTop(), 1);
	}
	BOOST_CHECK_EQUAL(context.getTop(), 0);
}




BOOST_FIXTURE_TEST_CASE(Assign, fxContext)
{
	context.mt<Udata>() = lua::Table::records(context);
	Value v(lua::nil, context);
	BOOST_CHECK_EQUAL(context.getTop(), 1);
	v = 3.14;
	BOOST_CHECK(v.type() == lua::ValueType::Number);
	BOOST_CHECK_EQUAL(v.cast<double>(), 3.14);
	v = Udata{42};
	BOOST_CHECK(v.type() == lua::ValueType::UserData);
	BOOST_CHECK_EQUAL(v.cast<Udata>().x, 42);
}


#ifndef LUAPP_COMPATIBILITY_NO_NRVO
static Value testReturn1(lua::Context& context)
{
	Value rv(3.14, context);
	return rv;
}
#endif	// LUAPP_COMPATIBILITY_NO_NRVO

Value testReturn2(lua::Context& context)
{
	return Value(3.14, context);
}

BOOST_FIXTURE_TEST_CASE(ReturnFromFunction, fxContext)
{
#ifndef LUAPP_COMPATIBILITY_NO_NRVO
	Value v1 = testReturn1(context);
	BOOST_CHECK(v1.type() == lua::ValueType::Number);
	BOOST_CHECK_EQUAL(v1.cast<double>(), 3.14);
#endif	// LUAPP_COMPATIBILITY_NO_NRVO
	Value v2 = testReturn2(context);
	BOOST_CHECK(v2.type() == lua::ValueType::Number);
	BOOST_CHECK_EQUAL(v2.cast<double>(), 3.14);
}




BOOST_FIXTURE_TEST_CASE(TableHandling, fxContext)
{
	Value v = lua::Table::array(context, 1, 2, 3, 4);
	BOOST_CHECK_EQUAL(context.getTop(), 1);
	Value v2 = lua::Table::records(context, "one", 1, "two", 2, "three", 3, "four", 4);
	BOOST_CHECK_EQUAL(context.getTop(), 2);
	lua::Table t(context);
	Value v3 = t;
	BOOST_CHECK_EQUAL(context.getTop(), 4);
}




BOOST_AUTO_TEST_SUITE_END()
