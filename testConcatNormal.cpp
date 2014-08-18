#include <boost/test/unit_test.hpp>

#include "fixtures.h"
#include <cstring>
#include <stdexcept>
using std::string;


struct Udata{
	int x;
};

LUAPP_USERDATA(Udata, "Test.Userdata")


static int signal_ = 0;

static lua::Retval concat(lua::Context& c)
{
	++signal_;
	return c.ret("userdata concatenated");
}



struct fxConcatNormal: public fxGlobalVal {
	fxConcatNormal()
	{
		signal_ = 0;
		vs[0] = "abc";
		context.mt<Udata>() = lua::Table::records(context, "__concat", lua::mkcf<concat>);
	}
};

#define v vs[0]
#define l context.global["val"]




BOOST_AUTO_TEST_SUITE(ConcatNormal)


BOOST_FIXTURE_TEST_CASE(Single, fxConcatNormal)
{
	BOOST_CHECK_EQUAL((l & v).cast<string>(), "3.14abc");
	BOOST_CHECK_EQUAL((v & l).cast<string>(), "abc3.14");
	BOOST_CHECK_EQUAL((v & "def").cast<string>(), "abcdef");
	BOOST_CHECK_EQUAL((l & "def").cast<string>(), "3.14def");
	BOOST_CHECK_EQUAL(("abc" & v).cast<string>(), "abcabc");
	BOOST_CHECK_EQUAL(("abc" & l).cast<string>(), "abc3.14");
}



BOOST_FIXTURE_TEST_CASE(Multiple, fxConcatNormal)
{
	BOOST_CHECK_EQUAL((l & v & 1 & "A" & 2.2).cast<string>(), "3.14abc1A2.2");
}



BOOST_FIXTURE_TEST_CASE(Userdata, fxConcatNormal)
{
	v = Udata{0};
	BOOST_CHECK_EQUAL((l & v).cast<string>(), "userdata concatenated");
	BOOST_CHECK_EQUAL(signal_, 1);
}


BOOST_FIXTURE_TEST_CASE(Discard, fxConcatNormal)
{
	v = Udata{0};
	l & v;
#ifdef LUAPP_NONDISCARDABLE_CONCAT
	BOOST_CHECK_EQUAL(signal_, 1);
#else
	BOOST_CHECK_EQUAL(signal_, 0);
#endif
}



BOOST_AUTO_TEST_SUITE_END()
