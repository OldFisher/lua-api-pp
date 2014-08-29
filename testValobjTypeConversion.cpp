#include <boost/test/unit_test.hpp>

#include "fixtures.h"
#include <cstring>
#include <stdexcept>


using std::strcmp;
using std::string;


struct Udata{int x;};
LUAPP_USERDATA(Udata, "Test.Userdata");


BOOST_AUTO_TEST_SUITE(Valobj)


#define v vs[0]
#define l context.global["val"]

BOOST_FIXTURE_TEST_CASE(Nil, fxGlobalVal)
{
	v = lua::nil;
	BOOST_CHECK(v.type() == lua::ValueType::Nil);
	BOOST_CHECK(v.is<lua::Nil>());
	BOOST_CHECK_EQUAL(v.cast<bool>(), false);

	l = lua::nil;
	BOOST_CHECK(l.type() == lua::ValueType::Nil);
	BOOST_CHECK(l.is<lua::Nil>());
	BOOST_CHECK_EQUAL(l.cast<bool>(), false);
}



BOOST_FIXTURE_TEST_CASE(Boolean, fxGlobalVal)
{
	v = true;
	BOOST_CHECK(v.type() == lua::ValueType::Boolean);
	BOOST_CHECK(v.is<bool>());
	BOOST_CHECK(v.cast<bool>() == true);
	{
		const bool converted = v;
		BOOST_CHECK(converted);
	}
//	BOOST_CHECK(v.cast<bool>() == true);
	BOOST_CHECK(v.optcast<bool>(false) == true);
	BOOST_CHECK(v.optcast<int>(42) == 42);

	l = true;
	BOOST_CHECK(l.type() == lua::ValueType::Boolean);
	BOOST_CHECK(l.is<bool>());
	BOOST_CHECK(l.cast<bool>() == true);
	{
		const bool converted = l;
		BOOST_CHECK(converted);
	}
//	BOOST_CHECK(l.cast<bool>() == true);
	BOOST_CHECK(l.optcast<bool>(false) == true);
	BOOST_CHECK(l.optcast<int>(42) == 42);
}



BOOST_FIXTURE_TEST_CASE(Int, fxGlobalVal)
{
    const int src = -42;
    v = src;
	BOOST_CHECK(v.type() == lua::ValueType::Number);
	BOOST_CHECK(v.is<int>());
	BOOST_CHECK(v.cast<bool>() == true);
	{
		const int converted = v;
		BOOST_CHECK(converted == src);
	}
	BOOST_CHECK(v.cast<int>() == src);
	BOOST_CHECK(v.optcast<int>(src - 1) == src);
	BOOST_CHECK(v.optcast<lua::LightUserData>(nullptr) == nullptr);

    l = src;
	BOOST_CHECK(l.type() == lua::ValueType::Number);
	BOOST_CHECK(l.is<int>());
	BOOST_CHECK(l.cast<bool>() == true);
	{
		const int converted = l;
		BOOST_CHECK(converted == src);
	}
	BOOST_CHECK(l.cast<int>() == src);
	BOOST_CHECK(l.optcast<int>(src - 1) == src);
	BOOST_CHECK(l.optcast<lua::LightUserData>(nullptr) == nullptr);
}




BOOST_FIXTURE_TEST_CASE(UnsignedInt, fxGlobalVal)
{
    const unsigned int src = 42;
    v = src;
	BOOST_CHECK(v.type() == lua::ValueType::Number);
	BOOST_CHECK(v.is<unsigned int>());
	BOOST_CHECK(v.cast<bool>() == true);
	{
		const unsigned int converted = v;
		BOOST_CHECK(converted == src);
	}
	BOOST_CHECK(v.cast<unsigned int>() == src);
	BOOST_CHECK(v.optcast<unsigned int>(src - 1) == src);
	BOOST_CHECK(v.optcast<lua::LightUserData>(nullptr) == nullptr);

    l = src;
	BOOST_CHECK(l.type() == lua::ValueType::Number);
	BOOST_CHECK(l.is<unsigned int>());
	BOOST_CHECK(l.cast<bool>() == true);
	{
		const unsigned int converted = l;
		BOOST_CHECK(converted == src);
	}
	BOOST_CHECK(l.cast<unsigned int>() == src);
	BOOST_CHECK(l.optcast<unsigned int>(src - 1) == src);
	BOOST_CHECK(l.optcast<lua::LightUserData>(nullptr) == nullptr);
}




BOOST_FIXTURE_TEST_CASE(LongLong, fxGlobalVal)
{
    const long long src = -42;
    v = src;
	BOOST_CHECK(v.type() == lua::ValueType::Number);
	BOOST_CHECK(v.is<long long>());
	BOOST_CHECK(v.cast<bool>() == true);
	{
		const long long converted = v;
		BOOST_CHECK(converted == src);
	}
	BOOST_CHECK(v.cast<long long>() == src);
	BOOST_CHECK(v.optcast<long long>(src - 1) == src);
	BOOST_CHECK(v.optcast<lua::LightUserData>(nullptr) == nullptr);

    l = src;
	BOOST_CHECK(l.type() == lua::ValueType::Number);
	BOOST_CHECK(l.is<long long>());
	BOOST_CHECK(l.cast<bool>() == true);
	{
		const long long converted = l;
		BOOST_CHECK(converted == src);
	}
	BOOST_CHECK(l.cast<long long>() == src);
	BOOST_CHECK(l.optcast<long long>(src - 1) == src);
	BOOST_CHECK(l.optcast<lua::LightUserData>(nullptr) == nullptr);
}




BOOST_FIXTURE_TEST_CASE(UnsignedLongLong, fxGlobalVal)
{
    const unsigned long long src = 42;
    v = src;
	BOOST_CHECK(v.type() == lua::ValueType::Number);
	BOOST_CHECK(v.is<unsigned long long>());
	BOOST_CHECK(v.cast<bool>() == true);
	{
		const unsigned long long converted = v;
		BOOST_CHECK(converted == src);
	}
	BOOST_CHECK(v.cast<unsigned long long>() == src);
	BOOST_CHECK(v.optcast<unsigned long long>(src - 1) == src);
	BOOST_CHECK(v.optcast<lua::LightUserData>(nullptr) == nullptr);

    l = src;
	BOOST_CHECK(l.type() == lua::ValueType::Number);
	BOOST_CHECK(l.is<unsigned long long>());
	BOOST_CHECK(l.cast<bool>() == true);
	{
		const unsigned long long converted = l;
		BOOST_CHECK(converted == src);
	}
	BOOST_CHECK(l.cast<unsigned long long>() == src);
	BOOST_CHECK(l.optcast<unsigned long long>(src - 1) == src);
	BOOST_CHECK(l.optcast<lua::LightUserData>(nullptr) == nullptr);
}




BOOST_FIXTURE_TEST_CASE(Float, fxGlobalVal)
{
    const float src = 3.14f;
    v = src;
	BOOST_CHECK(v.type() == lua::ValueType::Number);
	BOOST_CHECK(v.is<float>());
	BOOST_CHECK(v.cast<bool>() == true);
	{
		const float converted = v;
		BOOST_CHECK(converted == src);
	}
	BOOST_CHECK(v.cast<float>() == src);
	BOOST_CHECK(v.optcast<float>(src - 1) == src);
	BOOST_CHECK(v.optcast<lua::LightUserData>(nullptr) == nullptr);

    l = src;
	BOOST_CHECK(l.type() == lua::ValueType::Number);
	BOOST_CHECK(l.is<float>());
	BOOST_CHECK(l.cast<bool>() == true);
	{
		const float converted = l;
		BOOST_CHECK(converted == src);
	}
	BOOST_CHECK(l.cast<float>() == src);
	BOOST_CHECK(l.optcast<float>(src - 1) == src);
	BOOST_CHECK(l.optcast<lua::LightUserData>(nullptr) == nullptr);
}




BOOST_FIXTURE_TEST_CASE(Double, fxGlobalVal)
{
    const double src = 3.14;
    v = src;
	BOOST_CHECK(v.type() == lua::ValueType::Number);
	BOOST_CHECK(v.is<double>());
	BOOST_CHECK(v.cast<bool>() == true);
	{
		const double converted = v;
		BOOST_CHECK(converted == src);
	}
	BOOST_CHECK(v.cast<double>() == src);
	BOOST_CHECK(v.optcast<double>(src - 1) == src);
	BOOST_CHECK(v.optcast<lua::LightUserData>(nullptr) == nullptr);

    l = src;
	BOOST_CHECK(l.type() == lua::ValueType::Number);
	BOOST_CHECK(l.is<double>());
	BOOST_CHECK(l.cast<bool>() == true);
	{
		const double converted = l;
		BOOST_CHECK(converted == src);
	}
	BOOST_CHECK(l.cast<double>() == src);
	BOOST_CHECK(l.optcast<double>(src - 1) == src);
	BOOST_CHECK(l.optcast<lua::LightUserData>(nullptr) == nullptr);
}




BOOST_FIXTURE_TEST_CASE(String, fxGlobalVal)
{
	const char* const src = "A string";
	v = src;
	BOOST_CHECK(v.type() == lua::ValueType::String);
	BOOST_CHECK(v.is<const char*>());
	BOOST_CHECK(v.is<string>());
	BOOST_CHECK(v.cast<bool>() == true);
	{
		const char* const converted = v;
		BOOST_CHECK(strcmp(converted, src) == 0);
	}
	{
		const string converted = v;
		BOOST_CHECK(converted == src);
	}
	BOOST_CHECK(strcmp(v.optcast<const char*>("Another string"), src) == 0);
	BOOST_CHECK(v.optcast<lua::LightUserData>(nullptr) == nullptr);

	const string src2("A string 2");
	v = src2;
	BOOST_CHECK(v.type() == lua::ValueType::String);
	BOOST_CHECK(v.is<const char*>());
	BOOST_CHECK(v.is<string>());
	BOOST_CHECK(v.cast<bool>() == true);
	{
		const char* const converted = v;
		BOOST_CHECK(converted == src2);
	}
	{
		const string converted = v;
		BOOST_CHECK(converted == src2);
	}
	BOOST_CHECK(strcmp(v.optcast<const char*>("Another string"), src2.c_str()) == 0);
	BOOST_CHECK(v.optcast<lua::LightUserData>(nullptr) == nullptr);


	l = src;
	BOOST_CHECK(l.type() == lua::ValueType::String);
	BOOST_CHECK(l.is<const char*>());
	BOOST_CHECK(l.is<string>());
	BOOST_CHECK(l.cast<bool>() == true);
	{
		const char* const converted = l;
		BOOST_CHECK(strcmp(converted, src) == 0);
	}
	{
		const string converted = l;
		BOOST_CHECK(converted == src);
	}
	BOOST_CHECK(strcmp(l.optcast<const char*>("Another string"), src) == 0);
	BOOST_CHECK(l.optcast<lua::LightUserData>(nullptr) == nullptr);

	l = src2;
	BOOST_CHECK(l.type() == lua::ValueType::String);
	BOOST_CHECK(l.is<const char*>());
	BOOST_CHECK(l.is<string>());
	BOOST_CHECK(l.cast<bool>() == true);
	{
		const char* const converted = l;
		BOOST_CHECK(converted == src2);
	}
	{
		const string converted = l;
		BOOST_CHECK(converted == src2);
	}
	BOOST_CHECK(strcmp(l.optcast<const char*>("Another string"), src2.c_str()) == 0);
	BOOST_CHECK(l.optcast<lua::LightUserData>(nullptr) == nullptr);
}



BOOST_FIXTURE_TEST_CASE(NumberStringConversion, fxGlobalVal)
{
	const double nsrc = 3.14;
	const char* const ssrc = "3.14";

    v = nsrc;
    BOOST_CHECK(v.is<string>());
    BOOST_CHECK_EQUAL(v.cast<string>(), ssrc);

    v = ssrc;
    BOOST_CHECK(v.is<double>());
    BOOST_CHECK(v.cast<double>() == nsrc);

    l = nsrc;
    BOOST_CHECK(l.is<string>());
    BOOST_CHECK(l.cast<string>() == ssrc);

    l = ssrc;
    BOOST_CHECK(l.is<double>());
    BOOST_CHECK(l.cast<double>() == nsrc);

	v = nsrc;
	BOOST_CHECK(v.is<int>() && v.is<unsigned int>() && v.is<float>() && v.is<double>() && v.type() == lua::ValueType::Number);
	BOOST_CHECK(v.is<const char*>() && v.is<std::string>());
	BOOST_CHECK(v.type() == lua::ValueType::Number);
	const char* const x = v;
	BOOST_CHECK(v.type() == lua::ValueType::String);
	BOOST_CHECK(v.is<double>());
	BOOST_CHECK(x == std::string("3.14"));
	v = "some string";
	BOOST_CHECK(!v.is<double>());
}



static int doNothing(lua_State*) {return 0;}
static int doNothing2(lua_State*) {return 0;}

BOOST_FIXTURE_TEST_CASE(CFunction, fxGlobalVal)
{
    const lua::CFunction src = doNothing;
    v = src;
	BOOST_CHECK(v.type() == lua::ValueType::C_Function);
	BOOST_CHECK(v.is<lua::CFunction>());
	BOOST_CHECK(v.cast<bool>() == true);
	{
		const lua::CFunction converted = v;
		BOOST_CHECK(converted == src);
	}
	BOOST_CHECK(v.cast<lua::CFunction>() == src);
	BOOST_CHECK(v.optcast<lua::CFunction>(doNothing2) == src);
	BOOST_CHECK(v.optcast<lua::LightUserData>(nullptr) == nullptr);

    l = src;
	BOOST_CHECK(l.type() == lua::ValueType::C_Function);
	BOOST_CHECK(l.is<lua::CFunction>());
	BOOST_CHECK(l.cast<bool>() == true);
	{
		const lua::CFunction converted = l;
		BOOST_CHECK(converted == src);
	}
	BOOST_CHECK(l.cast<lua::CFunction>() == src);
	BOOST_CHECK(l.optcast<lua::CFunction>(doNothing2) == src);
	BOOST_CHECK(l.optcast<lua::LightUserData>(nullptr) == nullptr);
}



BOOST_FIXTURE_TEST_CASE(Function, fxGlobalVal)
{
	context.runString("val = function () end");
    v = l;
	BOOST_CHECK(v.type() == lua::ValueType::Function);
	BOOST_CHECK(v.is<lua::LFunction>());
	BOOST_CHECK(v.cast<bool>() == true);
	BOOST_CHECK(v.optcast<lua::LightUserData>(nullptr) == nullptr);

	BOOST_CHECK(l.type() == lua::ValueType::Function);
	BOOST_CHECK(l.is<lua::LFunction>());
	BOOST_CHECK(l.cast<bool>() == true);
	BOOST_CHECK(l.optcast<lua::LightUserData>(nullptr) == nullptr);
}



BOOST_FIXTURE_TEST_CASE(LightUserdata, fxGlobalVal)
{
	int someObject = 1, someObject2 = 2;
	const lua::LightUserData src = &someObject;
    v = src;
	BOOST_CHECK(v.type() == lua::ValueType::LightUserdata);
	BOOST_CHECK(v.is<lua::LightUserData>());
	BOOST_CHECK(v.cast<bool>() == true);
	{
		const lua::LightUserData converted = v;
		BOOST_CHECK(converted == src);
	}
	BOOST_CHECK(v.cast<lua::LightUserData>() == src);
	BOOST_CHECK(v.optcast<lua::LightUserData>(&someObject2) == src);
	BOOST_CHECK(v.optcast<int>(42) == 42);

    l = src;
	BOOST_CHECK(l.type() == lua::ValueType::LightUserdata);
	BOOST_CHECK(l.is<lua::LightUserData>());
	BOOST_CHECK(l.cast<bool>() == true);
	{
		const lua::LightUserData converted = l;
		BOOST_CHECK(converted == src);
	}
	BOOST_CHECK(l.cast<lua::LightUserData>() == src);
	BOOST_CHECK(l.optcast<lua::LightUserData>(&someObject2) == src);
	BOOST_CHECK(l.optcast<int>(42) == 42);
}



BOOST_FIXTURE_TEST_CASE(ConversionFailure, fxGlobalVal)
{
	context.mt<Udata>() = lua::Table::records(context);
	v = lua::nil;
	BOOST_CHECK_THROW(v.cast<lua::LightUserData>(), std::runtime_error);
	v = true;
	BOOST_CHECK_THROW(v.cast<lua::LightUserData>(), std::runtime_error);
	v = 3.14;
	BOOST_CHECK_THROW(v.cast<lua::LightUserData>(), std::runtime_error);
	v = "some string";
	BOOST_CHECK_THROW(v.cast<lua::LightUserData>(), std::runtime_error);
	v = lua::Table::records(context);
	BOOST_CHECK_THROW(v.cast<lua::LightUserData>(), std::runtime_error);
	v = doNothing;
	BOOST_CHECK_THROW(v.cast<lua::LightUserData>(), std::runtime_error);
	v = context.chunk("return coroutine.create(function () end)")();
	BOOST_CHECK_THROW(v.cast<int>(), std::runtime_error);
	v = Udata{0};
	BOOST_CHECK_THROW(v.cast<int>(), std::runtime_error);
	void* const ptr = nullptr;
	v = ptr;
	BOOST_CHECK_THROW(v.cast<int>(), std::runtime_error);
}




BOOST_AUTO_TEST_SUITE_END()
