#include <boost/test/unit_test.hpp>

#include "fixtures.h"
#include <cstring>
#include <stdexcept>

using std::string;



void* const customAllocatorPointer = reinterpret_cast<void*>(0xDEADBEEF);

static size_t customAllocatorCalls = 0;

static void* customLuaAllocator (void* ud, void* ptr, size_t oldSize, size_t newSize)
{
	if(ud != customAllocatorPointer)
		throw std::runtime_error("Custom allocator: user-pointer is wrong");
	++customAllocatorCalls;
	if ((!ptr || oldSize == 0) && newSize > 0)
		return new char [newSize];
	else if (newSize == 0) {
		if(oldSize != 0){
			delete [] static_cast<char*>(ptr);
		}
		return nullptr;
	} else {
		if (newSize <= oldSize)
			return ptr;
		void* newptr = new char [newSize];
		if(newptr)
			std::memcpy(newptr, ptr, oldSize);
		delete [] static_cast<char*>(ptr);
		return newptr;
	}
}



BOOST_AUTO_TEST_SUITE(State)



BOOST_FIXTURE_TEST_CASE( creationAndDestruction, fxState )
{
	BOOST_CHECK(true);
}



BOOST_AUTO_TEST_CASE(customAllocatorCreation)
{
	lua::State gs(&customLuaAllocator, customAllocatorPointer);
	BOOST_CHECK_GT(customAllocatorCalls, 0);
}



static int fnBad(lua_State* s)
{
	luaL_error(s, "Error test.");
	return 0;
}



BOOST_FIXTURE_TEST_CASE(directCallGood, fxSignal)
{
	BOOST_REQUIRE_EQUAL(signal, 0);
	gs.call(&fnSignal);
	BOOST_REQUIRE_EQUAL(signal, 1);
	signal = 0;
}



BOOST_FIXTURE_TEST_CASE(directCallBad, fxState)
{
	BOOST_REQUIRE_THROW(gs.call(&fnBad), std::runtime_error);
}



BOOST_FIXTURE_TEST_CASE(fileExecuteGood, fxFiles)
{
	gs.runFile("test_good.lua");
	BOOST_REQUIRE(isSignaled());
	signal = 0;
	gs.runFile(string("test_good.lua"));
	BOOST_REQUIRE(isSignaled());
	signal = 0;
}



BOOST_FIXTURE_TEST_CASE(fileExecuteBad, fxFiles)
{
	BOOST_REQUIRE_THROW(gs.runFile("test_bad.lua"), std::runtime_error);
	BOOST_REQUIRE_THROW(gs.runFile(string("test_bad.lua")), std::runtime_error);
}



BOOST_FIXTURE_TEST_CASE(fileExecuteMissing, fxSignal)
{
	BOOST_REQUIRE_THROW(gs.runFile("nosuchfile.lua"), std::runtime_error);
	BOOST_REQUIRE_THROW(gs.runFile(string("nosuchfile.lua")), std::runtime_error);
}



BOOST_FIXTURE_TEST_CASE(stringExecuteGood, fxSignal)
{
	gs.runString("fnSignal()");
	BOOST_REQUIRE(isSignaled());
	signal = 0;
	gs.runString(string("fnSignal()"));
	BOOST_REQUIRE(isSignaled());
	signal = 0;
}



BOOST_FIXTURE_TEST_CASE(stringExecuteBad, fxState)
{
	BOOST_REQUIRE_THROW(gs.runString("{"), std::runtime_error);
	BOOST_REQUIRE_THROW(gs.runString(string("{")), std::runtime_error);
}



#ifdef LUAPP_SAFE_EXCEPTIONS
static void fnNestedError()
{
	throw std::logic_error("Nested error");
}

lua::Retval lNestedError(lua::Context& c)
{
	fnNestedError();
	return c.ret();
}

BOOST_FIXTURE_TEST_CASE(nestedErrors, fxContext)
{
	BOOST_REQUIRE_THROW(gs.call(lua::mkcf<lNestedError>), std::runtime_error);
	BOOST_MESSAGE("mkcf call successful");
	context.global["fn"] = fnNestedError;
	BOOST_REQUIRE_THROW(gs.runString("fn()"), std::runtime_error);
}
#endif // LUAPP_SAFE_EXCEPTIONS

BOOST_AUTO_TEST_SUITE_END()
