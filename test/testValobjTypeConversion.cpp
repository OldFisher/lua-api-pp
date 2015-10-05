#include <boost/test/unit_test.hpp>

#include "fixtures.h"
#include <cstring>
#include <stdexcept>


using std::strcmp;
using std::string;

using lua::LightUserData;
using lua::ValueType;
using lua::Table;
using lua::CFunction;
using lua::LFunction;
using lua::Nil;
using lua::nil;


struct Udata{int x;};
LUAPP_USERDATA(Udata, "Test.Userdata");


BOOST_AUTO_TEST_SUITE(Valobj)


#define v vs[0]
#define l context.global["val"]

BOOST_FIXTURE_TEST_CASE(Nil_, fxGlobalVal)
{
	v = nil;
	BOOST_CHECK(v.type() == ValueType::Nil);
	BOOST_CHECK(v.is<Nil>());
	BOOST_CHECK_EQUAL(v.cast<bool>(), false);
	BOOST_CHECK_EQUAL(v.to<bool>(), false);
#if(LUAPP_API_VERSION >= 53)
	BOOST_CHECK(!v.isInteger());
#endif

	l = nil;
	BOOST_CHECK(l.type() == ValueType::Nil);
	BOOST_CHECK(l.is<Nil>());
	BOOST_CHECK_EQUAL(l.cast<bool>(), false);
	BOOST_CHECK_EQUAL(l.to<bool>(), false);
#if(LUAPP_API_VERSION >= 53)
	BOOST_CHECK(!l.isInteger());
#endif
}



BOOST_FIXTURE_TEST_CASE(Boolean, fxGlobalVal)
{
	v = true;
	BOOST_CHECK(v.type() == ValueType::Boolean);
	BOOST_CHECK(v.is<bool>());
	BOOST_CHECK_EQUAL(v.cast<bool>(), true);
	BOOST_CHECK_EQUAL(v.to<bool>(), true);
	{
		const bool converted = v;
		BOOST_CHECK(converted);
	}

	BOOST_CHECK_EQUAL(v.to<bool>(false), true);
	BOOST_CHECK_EQUAL(v.to<int>(42), 42);
#if(LUAPP_API_VERSION >= 53)
	BOOST_CHECK(!v.isInteger());
#endif

	l = true;
	BOOST_CHECK(l.type() == ValueType::Boolean);
	BOOST_CHECK(l.is<bool>());
	BOOST_CHECK_EQUAL(l.cast<bool>(), true);
	BOOST_CHECK_EQUAL(l.to<bool>(), true);
	{
		const bool converted = l;
		BOOST_CHECK(converted);
	}
	BOOST_CHECK_EQUAL(l.to<bool>(false), true);
	BOOST_CHECK_EQUAL(l.to<int>(42), 42);
#if(LUAPP_API_VERSION >= 53)
	BOOST_CHECK(!l.isInteger());
#endif
}



BOOST_FIXTURE_TEST_CASE(Int, fxGlobalVal)
{
    const int src = -42;
    v = src;
	BOOST_CHECK(v.type() == ValueType::Number);
	BOOST_CHECK(v.is<int>());
	BOOST_CHECK_EQUAL(v.cast<bool>(), true);
	BOOST_CHECK_EQUAL(v.to<bool>(), true);
	{
		const int converted = v;
		BOOST_CHECK_EQUAL(converted, src);
	}
	BOOST_CHECK_EQUAL(v.cast<int>(), src);
	BOOST_CHECK_EQUAL(v.to<int>(), src);
	BOOST_CHECK_EQUAL(v.to<int>(src - 1), src);
	BOOST_CHECK(v.to<LightUserData>(nullptr) == nullptr);
#if(LUAPP_API_VERSION >= 53)
	BOOST_CHECK(v.isInteger());
#endif

    l = src;
	BOOST_CHECK(l.type() == ValueType::Number);
	BOOST_CHECK(l.is<int>());
	BOOST_CHECK_EQUAL(l.cast<bool>(), true);
	BOOST_CHECK_EQUAL(l.to<bool>(), true);
	{
		const int converted = l;
		BOOST_CHECK_EQUAL(converted, src);
	}
	BOOST_CHECK_EQUAL(l.cast<int>(), src);
	BOOST_CHECK_EQUAL(l.to<int>(), src);
	BOOST_CHECK_EQUAL(l.to<int>(src - 1), src);
	BOOST_CHECK(l.to<LightUserData>(nullptr) == nullptr);
#if(LUAPP_API_VERSION >= 53)
	BOOST_CHECK(l.isInteger());
#endif
}




BOOST_FIXTURE_TEST_CASE(UnsignedInt, fxGlobalVal)
{
    const unsigned int src = 42;
    v = src;
	BOOST_CHECK(v.type() == ValueType::Number);
	BOOST_CHECK(v.is<unsigned int>());
	BOOST_CHECK_EQUAL(v.cast<bool>(), true);
	BOOST_CHECK_EQUAL(v.to<bool>(), true);
	{
		const unsigned int converted = v;
		BOOST_CHECK_EQUAL(converted, src);
	}
	BOOST_CHECK_EQUAL(v.cast<unsigned int>(), src);
	BOOST_CHECK_EQUAL(v.to<unsigned int>(), src);
	BOOST_CHECK_EQUAL(v.to<unsigned int>(src - 1), src);
	BOOST_CHECK(v.to<LightUserData>(nullptr) == nullptr);
#if(LUAPP_API_VERSION >= 53)
	BOOST_CHECK(v.isInteger());
#endif

    l = src;
	BOOST_CHECK(l.type() == ValueType::Number);
	BOOST_CHECK(l.is<unsigned int>());
	BOOST_CHECK_EQUAL(l.cast<bool>(), true);
	BOOST_CHECK_EQUAL(l.to<bool>(), true);
	{
		const unsigned int converted = l;
		BOOST_CHECK_EQUAL(converted, src);
	}
	BOOST_CHECK_EQUAL(l.cast<unsigned int>(), src);
	BOOST_CHECK_EQUAL(l.to<unsigned int>(), src);
	BOOST_CHECK_EQUAL(l.to<unsigned int>(src - 1), src);
	BOOST_CHECK(l.to<LightUserData>(nullptr) == nullptr);
#if(LUAPP_API_VERSION >= 53)
	BOOST_CHECK(l.isInteger());
#endif
}




BOOST_FIXTURE_TEST_CASE(LongLong, fxGlobalVal)
{
    const long long src = -42;
    v = src;
	BOOST_CHECK(v.type() == ValueType::Number);
	BOOST_CHECK(v.is<long long>());
	BOOST_CHECK_EQUAL(v.cast<bool>(), true);
	BOOST_CHECK_EQUAL(v.to<bool>(), true);
	{
		const long long converted = v;
		BOOST_CHECK_EQUAL(converted, src);
	}
	BOOST_CHECK_EQUAL(v.cast<long long>(), src);
	BOOST_CHECK_EQUAL(v.to<long long>(), src);
	BOOST_CHECK(v.to<long long>(src - 1) == src);
	BOOST_CHECK(v.to<LightUserData>(nullptr) == nullptr);
#if(LUAPP_API_VERSION >= 53)
	BOOST_CHECK(v.isInteger());
#endif

    l = src;
	BOOST_CHECK(l.type() == ValueType::Number);
	BOOST_CHECK(l.is<long long>());
	BOOST_CHECK_EQUAL(l.cast<bool>(), true);
	BOOST_CHECK_EQUAL(l.to<bool>(), true);
	{
		const long long converted = l;
		BOOST_CHECK_EQUAL(converted, src);
	}
	BOOST_CHECK_EQUAL(l.cast<long long>(), src);
	BOOST_CHECK_EQUAL(l.to<long long>(), src);
	BOOST_CHECK_EQUAL(l.to<long long>(src - 1), src);
	BOOST_CHECK(l.to<LightUserData>(nullptr) == nullptr);
#if(LUAPP_API_VERSION >= 53)
	BOOST_CHECK(l.isInteger());
#endif
}




BOOST_FIXTURE_TEST_CASE(UnsignedLongLong, fxGlobalVal)
{
    const unsigned long long src = 42;
    v = src;
	BOOST_CHECK(v.type() == ValueType::Number);
	BOOST_CHECK(v.is<unsigned long long>());
	BOOST_CHECK_EQUAL(v.cast<bool>(), true);
	BOOST_CHECK_EQUAL(v.to<bool>(), true);
	{
		const unsigned long long converted = v;
		BOOST_CHECK_EQUAL(converted, src);
	}
	BOOST_CHECK_EQUAL(v.cast<unsigned long long>(), src);
	BOOST_CHECK_EQUAL(v.to<unsigned long long>(), src);
	BOOST_CHECK_EQUAL(v.to<unsigned long long>(src - 1), src);
	BOOST_CHECK(v.to<LightUserData>(nullptr) == nullptr);
#if(LUAPP_API_VERSION >= 53)
	BOOST_CHECK(v.isInteger());
#endif

    l = src;
	BOOST_CHECK(l.type() == ValueType::Number);
	BOOST_CHECK(l.is<unsigned long long>());
	BOOST_CHECK_EQUAL(l.cast<bool>(), true);
	BOOST_CHECK_EQUAL(l.to<bool>(), true);
	{
		const unsigned long long converted = l;
		BOOST_CHECK_EQUAL(converted, src);
	}
	BOOST_CHECK_EQUAL(l.cast<unsigned long long>(), src);
	BOOST_CHECK_EQUAL(l.to<unsigned long long>(), src);
	BOOST_CHECK_EQUAL(l.to<unsigned long long>(src - 1), src);
	BOOST_CHECK(l.to<LightUserData>(nullptr) == nullptr);
#if(LUAPP_API_VERSION >= 53)
	BOOST_CHECK(l.isInteger());
#endif
}




BOOST_FIXTURE_TEST_CASE(Float, fxGlobalVal)
{
    const float src = 3.14f;
    v = src;
	BOOST_CHECK(v.type() == ValueType::Number);
	BOOST_CHECK(v.is<float>());
	BOOST_CHECK_EQUAL(v.cast<bool>(), true);
	BOOST_CHECK_EQUAL(v.to<bool>(), true);
	{
		const float converted = v;
		BOOST_CHECK_EQUAL(converted, src);
	}
	BOOST_CHECK_EQUAL(v.cast<float>(), src);
	BOOST_CHECK_EQUAL(v.to<float>(), src);
	BOOST_CHECK_EQUAL(v.to<float>(src - 1), src);
	BOOST_CHECK(v.to<LightUserData>(nullptr) == nullptr);
#if(LUAPP_API_VERSION >= 53)
	BOOST_CHECK(!v.isInteger());
#endif

    l = src;
	BOOST_CHECK(l.type() == ValueType::Number);
	BOOST_CHECK(l.is<float>());
	BOOST_CHECK_EQUAL(l.cast<bool>(), true);
	BOOST_CHECK_EQUAL(l.to<bool>(), true);
	{
		const float converted = l;
		BOOST_CHECK_EQUAL(converted, src);
	}
	BOOST_CHECK_EQUAL(l.cast<float>(), src);
	BOOST_CHECK_EQUAL(l.to<float>(), src);
	BOOST_CHECK_EQUAL(l.to<float>(src - 1), src);
	BOOST_CHECK(l.to<LightUserData>(nullptr) == nullptr);
#if(LUAPP_API_VERSION >= 53)
	BOOST_CHECK(!l.isInteger());
#endif
}




BOOST_FIXTURE_TEST_CASE(Double, fxGlobalVal)
{
    const double src = 3.14;
    v = src;
	BOOST_CHECK(v.type() == ValueType::Number);
	BOOST_CHECK(v.is<double>());
	BOOST_CHECK_EQUAL(v.cast<bool>(), true);
	BOOST_CHECK_EQUAL(v.to<bool>(), true);
	{
		const double converted = v;
		BOOST_CHECK_EQUAL(converted, src);
	}
	BOOST_CHECK_EQUAL(v.cast<double>(), src);
	BOOST_CHECK_EQUAL(v.to<double>(), src);
	BOOST_CHECK_EQUAL(v.to<double>(src - 1), src);
	BOOST_CHECK(v.to<LightUserData>(nullptr) == nullptr);
#if(LUAPP_API_VERSION >= 53)
	BOOST_CHECK(!v.isInteger());
#endif

    l = src;
	BOOST_CHECK(l.type() == ValueType::Number);
	BOOST_CHECK(l.is<double>());
	BOOST_CHECK_EQUAL(l.cast<bool>(), true);
	BOOST_CHECK_EQUAL(l.to<bool>(), true);
	{
		const double converted = l;
		BOOST_CHECK_EQUAL(converted, src);
	}
	BOOST_CHECK_EQUAL(l.cast<double>(), src);
	BOOST_CHECK_EQUAL(l.to<double>(), src);
	BOOST_CHECK_EQUAL(l.to<double>(src - 1), src);
	BOOST_CHECK(l.to<LightUserData>(nullptr) == nullptr);
#if(LUAPP_API_VERSION >= 53)
	BOOST_CHECK(!l.isInteger());
#endif
}




BOOST_FIXTURE_TEST_CASE(String, fxGlobalVal)
{
	const char* const src = "A string";
	v = src;
	BOOST_CHECK(v.type() == ValueType::String);
	BOOST_CHECK(v.is<const char*>());
	BOOST_CHECK(v.is<string>());
	BOOST_CHECK_EQUAL(v.cast<bool>(), true);
	BOOST_CHECK_EQUAL(v.to<bool>(), true);
	{
		const char* const converted = v;
		BOOST_CHECK(strcmp(converted, src) == 0);
	}
	{
		const string converted = v;
		BOOST_CHECK_EQUAL(converted, src);
	}
	BOOST_CHECK(strcmp(v.to<const char*>("Another string"), src) == 0);
	BOOST_CHECK(v.to<LightUserData>(nullptr) == nullptr);
#if(LUAPP_API_VERSION >= 53)
	BOOST_CHECK(!v.isInteger());
#endif

	const string src2("A string 2");
	v = src2;
	BOOST_CHECK(v.type() == ValueType::String);
	BOOST_CHECK(v.is<const char*>());
	BOOST_CHECK(v.is<string>());
	BOOST_CHECK_EQUAL(v.cast<bool>(), true);
	BOOST_CHECK_EQUAL(v.to<bool>(), true);
	{
		const char* const converted = v;
		BOOST_CHECK_EQUAL(converted, src2);
	}
	{
		const string converted = v;
		BOOST_CHECK_EQUAL(converted, src2);
	}
	BOOST_CHECK(strcmp(v.to<const char*>("Another string"), src2.c_str()) == 0);
	BOOST_CHECK(v.to<LightUserData>(nullptr) == nullptr);


	l = src;
	BOOST_CHECK(l.type() == ValueType::String);
	BOOST_CHECK(l.is<const char*>());
	BOOST_CHECK(l.is<string>());
	BOOST_CHECK_EQUAL(l.cast<bool>(), true);
	BOOST_CHECK_EQUAL(l.to<bool>(), true);
	{
		const char* const converted = l;
		BOOST_CHECK(strcmp(converted, src) == 0);
	}
	{
		const string converted = l;
		BOOST_CHECK_EQUAL(converted, src);
	}
	BOOST_CHECK(strcmp(l.to<const char*>("Another string"), src) == 0);
	BOOST_CHECK(l.to<LightUserData>(nullptr) == nullptr);
#if(LUAPP_API_VERSION >= 53)
	BOOST_CHECK(!l.isInteger());
#endif

	l = src2;
	BOOST_CHECK(l.type() == ValueType::String);
	BOOST_CHECK(l.is<const char*>());
	BOOST_CHECK(l.is<string>());
	BOOST_CHECK_EQUAL(l.cast<bool>(), true);
	BOOST_CHECK_EQUAL(l.to<bool>(), true);
	{
		const char* const converted = l;
		BOOST_CHECK_EQUAL(converted, src2);
	}
	{
		const string converted = l;
		BOOST_CHECK_EQUAL(converted, src2);
	}
	BOOST_CHECK(strcmp(l.to<const char*>("Another string"), src2.c_str()) == 0);
	BOOST_CHECK(l.to<LightUserData>(nullptr) == nullptr);
}



BOOST_FIXTURE_TEST_CASE(NumberStringConversion, fxGlobalVal)
{
	const double nsrc = 3.14;
	const char* const ssrc = "3.14";

    v = nsrc;
    BOOST_CHECK(v.is<string>());
    BOOST_CHECK_EQUAL(v.cast<string>(), ssrc);
    v = nsrc;
    BOOST_CHECK_EQUAL(v.to<string>(), ssrc);

    v = ssrc;
    BOOST_CHECK(v.is<double>());
    BOOST_CHECK_EQUAL(v.cast<double>(), nsrc);
    v = ssrc;
    BOOST_CHECK_EQUAL(v.to<double>(), nsrc);

    l = nsrc;
    BOOST_CHECK(l.is<string>());
    BOOST_CHECK_EQUAL(l.cast<string>(), ssrc);
    l = nsrc;
    BOOST_CHECK_EQUAL(l.to<string>(), ssrc);

    l = ssrc;
    BOOST_CHECK(l.is<double>());
    BOOST_CHECK_EQUAL(l.cast<double>(), nsrc);
    l = ssrc;
    BOOST_CHECK_EQUAL(l.to<double>(), nsrc);

	v = nsrc;
	BOOST_CHECK(v.is<int>() && v.is<unsigned int>() && v.is<float>() && v.is<double>() && v.type() == ValueType::Number);
	BOOST_CHECK(v.is<const char*>() && v.is<std::string>());
	BOOST_CHECK(v.type() == ValueType::Number);
	const char* const x = v;
	BOOST_CHECK(v.type() == ValueType::String);
	BOOST_CHECK(v.is<double>());
	BOOST_CHECK_EQUAL(x, std::string("3.14"));
	v = "some string";
	BOOST_CHECK(!v.is<double>());
}



static int doNothing(lua_State*) {return 0;}
static int doNothing2(lua_State*) {return 0;}

BOOST_FIXTURE_TEST_CASE(C_Function, fxGlobalVal)
{
    const CFunction src = doNothing;
    v = src;
	BOOST_CHECK(v.type() == ValueType::C_Function);
	BOOST_CHECK(v.is<CFunction>());
	BOOST_CHECK_EQUAL(v.cast<bool>(), true);
	BOOST_CHECK_EQUAL(v.to<bool>(), true);
	{
		const CFunction converted = v;
		BOOST_CHECK(converted == src);
	}
	BOOST_CHECK(v.cast<CFunction>() == src);
	BOOST_CHECK(v.to<CFunction>() == src);
	BOOST_CHECK(v.to<CFunction>(doNothing2) == src);
	BOOST_CHECK(v.to<LightUserData>(nullptr) == nullptr);
#if(LUAPP_API_VERSION >= 53)
	BOOST_CHECK(!v.isInteger());
#endif

    l = src;
	BOOST_CHECK(l.type() == ValueType::C_Function);
	BOOST_CHECK(l.is<CFunction>());
	BOOST_CHECK_EQUAL(l.cast<bool>(), true);
	BOOST_CHECK_EQUAL(l.to<bool>(), true);
	{
		const CFunction converted = l;
		BOOST_CHECK(converted == src);
	}
	BOOST_CHECK(l.cast<CFunction>() == src);
	BOOST_CHECK(l.to<CFunction>() == src);
	BOOST_CHECK(l.to<CFunction>(doNothing2) == src);
	BOOST_CHECK(l.to<LightUserData>(nullptr) == nullptr);
#if(LUAPP_API_VERSION >= 53)
	BOOST_CHECK(!l.isInteger());
#endif
}



BOOST_FIXTURE_TEST_CASE(Function, fxGlobalVal)
{
	context.runString("val = function () end");
    v = l;
	BOOST_CHECK(v.type() == ValueType::Function);
	BOOST_CHECK(v.is<LFunction>());
	BOOST_CHECK_EQUAL(v.cast<bool>(), true);
	BOOST_CHECK_EQUAL(v.to<bool>(), true);
	BOOST_CHECK(v.to<LightUserData>(nullptr) == nullptr);
#if(LUAPP_API_VERSION >= 53)
	BOOST_CHECK(!v.isInteger());
#endif

	BOOST_CHECK(l.type() == ValueType::Function);
	BOOST_CHECK(l.is<LFunction>());
	BOOST_CHECK_EQUAL(l.cast<bool>(), true);
	BOOST_CHECK_EQUAL(l.to<bool>(), true);
	BOOST_CHECK(l.to<LightUserData>(nullptr) == nullptr);
#if(LUAPP_API_VERSION >= 53)
	BOOST_CHECK(!l.isInteger());
#endif
}



BOOST_FIXTURE_TEST_CASE(LightUserdata, fxGlobalVal)
{
	int someObject = 1, someObject2 = 2;
	const LightUserData src = &someObject;
    v = src;
	BOOST_CHECK(v.type() == ValueType::LightUserdata);
	BOOST_CHECK(v.is<LightUserData>());
	BOOST_CHECK_EQUAL(v.cast<bool>(), true);
	BOOST_CHECK_EQUAL(v.to<bool>(), true);
	{
		const LightUserData converted = v;
		BOOST_CHECK(converted == src);
	}
	BOOST_CHECK(v.cast<LightUserData>() == src);
	BOOST_CHECK(v.to<LightUserData>() == src);
	BOOST_CHECK(v.to<LightUserData>(&someObject2) == src);
	BOOST_CHECK_EQUAL(v.to<int>(42), 42);
#if(LUAPP_API_VERSION >= 53)
	BOOST_CHECK(!v.isInteger());
#endif

    l = src;
	BOOST_CHECK(l.type() == ValueType::LightUserdata);
	BOOST_CHECK(l.is<LightUserData>());
	BOOST_CHECK_EQUAL(l.cast<bool>(), true);
	BOOST_CHECK_EQUAL(l.to<bool>(), true);
	{
		const LightUserData converted = l;
		BOOST_CHECK(converted == src);
	}
	BOOST_CHECK(l.cast<LightUserData>() == src);
	BOOST_CHECK(l.to<LightUserData>() == src);
	BOOST_CHECK(l.to<LightUserData>(&someObject2) == src);
	BOOST_CHECK_EQUAL(l.to<int>(42), 42);
#if(LUAPP_API_VERSION >= 53)
	BOOST_CHECK(!l.isInteger());
#endif
}



BOOST_FIXTURE_TEST_CASE(ConversionFailure, fxGlobalVal)
{
	context.mt<Udata>() = Table::records(context);
	v = nil;
	BOOST_CHECK_THROW(v.cast<LightUserData>(), std::runtime_error);
	v = true;
	BOOST_CHECK_THROW(v.cast<LightUserData>(), std::runtime_error);
	v = 3.14;
	BOOST_CHECK_THROW(v.cast<LightUserData>(), std::runtime_error);
	v = "some string";
	BOOST_CHECK_THROW(v.cast<LightUserData>(), std::runtime_error);
	v = Table::records(context);
	BOOST_CHECK_THROW(v.cast<LightUserData>(), std::runtime_error);
	v = doNothing;
	BOOST_CHECK_THROW(v.cast<LightUserData>(), std::runtime_error);
	v = context.chunk("return coroutine.create(function () end)")();
	BOOST_CHECK_THROW(v.cast<int>(), std::runtime_error);
	v = Udata{0};
	BOOST_CHECK_THROW(v.cast<int>(), std::runtime_error);
	void* const ptr = nullptr;
	v = ptr;
	BOOST_CHECK_THROW(v.cast<int>(), std::runtime_error);
}




BOOST_AUTO_TEST_SUITE_END()
