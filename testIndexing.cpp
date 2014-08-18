#include <boost/test/unit_test.hpp>

#include "fixtures.h"
#include <stdexcept>


struct fxIndexing: public fxContext {
	fxIndexing()
	{
		context.runString("val = {1, 2, {1, 2, {1, 2}}}");
	}
};

#define l context.global["val"]


struct Udata{ int x;};
LUAPP_USERDATA(Udata, "Test.Userdata")



BOOST_AUTO_TEST_SUITE(Indexing)


BOOST_FIXTURE_TEST_CASE(Native, fxIndexing)
{
	l[1] = 42;
	BOOST_CHECK_EQUAL(l[1].cast<int>(), 42);
	lua::Value v = l;
	BOOST_CHECK_EQUAL(v[1].cast<int>(), 42);
	v[2] = 43;
	BOOST_CHECK_EQUAL(v[2].cast<int>(), 43);
}



BOOST_FIXTURE_TEST_CASE(Reference, fxIndexing)
{
	lua::Value val(2, context);
	BOOST_CHECK_EQUAL(l[val].cast<int>(), 2);
	lua::Value v = l;
	v[val] = 43;
	BOOST_CHECK_EQUAL(v[val].cast<int>(), 43);
}



BOOST_FIXTURE_TEST_CASE(Tempval, fxIndexing)
{
	BOOST_CHECK_EQUAL(l[l[2]].cast<int>(), 2);
	lua::Value v = l;
	v[l[2]] = 43;
	BOOST_CHECK_EQUAL(v[l[3][2]].cast<int>(), 43);
}





static int signal = 0;

static lua::Retval setsignal(lua::Context& c)
{
	signal = 1;
	return c.ret();
}




BOOST_FIXTURE_TEST_CASE(Discard, fxIndexing)
{
	signal = 0;
	context.mt<Udata>() = lua::Table::records(context, "__index", lua::mkcf<setsignal>);
	l[3][3][3] = Udata{0};
	l[3][3][3];
#ifdef LUAPP_NODISCARD_INDEX
	BOOST_CHECK_EQUAL(signal, 1);
#else
	BOOST_CHECK_EQUAL(signal, 0);
#endif // LUAPP_NODISCARD_INDEX
}


BOOST_AUTO_TEST_SUITE_END()
