#include <boost/test/unit_test.hpp>

#include "fixtures.h"
#include <stdexcept>
using std::string;



struct fxComp: public fxContext
{
	lua::Value v0, v1, v1a;
	fxComp():
		v0(0.1, context),
		v1(1.1, context),
		v1a(1.1, context)
	{
		context.global["val0"] = 0.1;
		context.global["val1"] = 1.1;
		context.global["val1a"] = 1.1;
	}
};


#define l0 context.global["val0"]
#define l1 context.global["val1"]
#define l1a context.global["val1a"]


BOOST_AUTO_TEST_SUITE(Comparisons)


BOOST_FIXTURE_TEST_CASE(Equality, fxComp)
{
	BOOST_CHECK(v1 == v1a);
	BOOST_CHECK(v1 ==  l1);
	BOOST_CHECK(l1 ==  v1);
	BOOST_CHECK(l1 == l1a);

	BOOST_CHECK(v1 == 1.1);
	BOOST_CHECK(1.1 == v1);
	BOOST_CHECK(l1 == 1.1);
	BOOST_CHECK(1.1 == l1);

	BOOST_CHECK(!(v0 == v1a));
	BOOST_CHECK(!(v0 ==  l1));
	BOOST_CHECK(!(l0 ==  v1));
	BOOST_CHECK(!(l0 == l1a));

	BOOST_CHECK(!(v0 == 1.1));
	BOOST_CHECK(!(1.1 == v0));
	BOOST_CHECK(!(l0 == 1.1));
	BOOST_CHECK(!(1.1 == l0));
}




BOOST_FIXTURE_TEST_CASE(Inequality, fxComp)
{
	BOOST_CHECK(!(v1 != v1a));
	BOOST_CHECK(!(v1 !=  l1));
	BOOST_CHECK(!(l1 !=  v1));
	BOOST_CHECK(!(l1 != l1a));

	BOOST_CHECK(!(v1 != 1.1));
	BOOST_CHECK(!(1.1 != v1));
	BOOST_CHECK(!(l1 != 1.1));
	BOOST_CHECK(!(1.1 != l1));

	BOOST_CHECK(v0 != v1a);
	BOOST_CHECK(v0 !=  l1);
	BOOST_CHECK(l0 !=  v1);
	BOOST_CHECK(l0 != l1a);

	BOOST_CHECK(v0 != 1.1);
	BOOST_CHECK(1.1 != v0);
	BOOST_CHECK(l0 != 1.1);
	BOOST_CHECK(1.1 != l0);
}



BOOST_FIXTURE_TEST_CASE(Less, fxComp)
{
	BOOST_CHECK(!(v1 < v1a));
	BOOST_CHECK(!(v1 <  l1));
	BOOST_CHECK(!(l1 <  v1));
	BOOST_CHECK(!(l1 < l1a));

	BOOST_CHECK(!(v1 < 1.1));
	BOOST_CHECK(!(1.1 < v1));
	BOOST_CHECK(!(l1 < 1.1));
	BOOST_CHECK(!(1.1 < l1));

	BOOST_CHECK(v0 < v1a);
	BOOST_CHECK(v0 <  l1);
	BOOST_CHECK(l0 <  v1);
	BOOST_CHECK(l0 < l1a);

	BOOST_CHECK(v0 < 1.1);
	BOOST_CHECK(0.1 < v1);
	BOOST_CHECK(l0 < 1.1);
	BOOST_CHECK(0.1 < l1);
}




BOOST_FIXTURE_TEST_CASE(LessEqual, fxComp)
{
	BOOST_CHECK(v1 <= v1a);
	BOOST_CHECK(v1 <=  l1);
	BOOST_CHECK(l1 <=  v1);
	BOOST_CHECK(l1 <= l1a);

	BOOST_CHECK(v1 <= 1.1);
	BOOST_CHECK(1.1 <= v1);
	BOOST_CHECK(l1 <= 1.1);
	BOOST_CHECK(1.1 <= l1);

	BOOST_CHECK(v0 <= v1a);
	BOOST_CHECK(v0 <=  l1);
	BOOST_CHECK(l0 <=  v1);
	BOOST_CHECK(l0 <= l1a);

	BOOST_CHECK(v0 <= 1.1);
	BOOST_CHECK(0.1 <= v1);
	BOOST_CHECK(l0 <= 1.1);
	BOOST_CHECK(0.1 <= l1);

	BOOST_CHECK(!(v1 <= v0));
	BOOST_CHECK(!(v1 <= l0));
	BOOST_CHECK(!(l1 <= v0));
	BOOST_CHECK(!(l1 <= l0));
}



BOOST_FIXTURE_TEST_CASE(Greater, fxComp)
{
	BOOST_CHECK(!(v1 > v1a));
	BOOST_CHECK(!(v1 >  l1));
	BOOST_CHECK(!(l1 >  v1));
	BOOST_CHECK(!(l1 > l1a));

	BOOST_CHECK(!(v1 > 1.1));
	BOOST_CHECK(!(1.1 > v1));
	BOOST_CHECK(!(l1 > 1.1));
	BOOST_CHECK(!(1.1 > l1));

	BOOST_CHECK(v1 > v0);
	BOOST_CHECK(l1 > v0);
	BOOST_CHECK(v1 > l0);
	BOOST_CHECK(l1 > l0);

	BOOST_CHECK(v1 > 0.1);
	BOOST_CHECK(1.1 > v0);
	BOOST_CHECK(l1 > 0.1);
	BOOST_CHECK(1.1 > l0);
}




BOOST_FIXTURE_TEST_CASE(GreaterEqual, fxComp)
{
	BOOST_CHECK(v1 >= v1a);
	BOOST_CHECK(v1 >=  l1);
	BOOST_CHECK(l1 >=  v1);
	BOOST_CHECK(l1 >= l1a);

	BOOST_CHECK(v1 >= 1.1);
	BOOST_CHECK(1.1 >= v1);
	BOOST_CHECK(l1 >= 1.1);
	BOOST_CHECK(1.1 >= l1);

	BOOST_CHECK(v1 >= v0);
	BOOST_CHECK(l1 >= v0);
	BOOST_CHECK(v1 >= l0);
	BOOST_CHECK(l1 >= l0);

	BOOST_CHECK(v1 >= 0.1);
	BOOST_CHECK(1.1 >= v0);
	BOOST_CHECK(l1 >= 0.1);
	BOOST_CHECK(1.1 >= l0);

	BOOST_CHECK(!(v0 >= v1));
	BOOST_CHECK(!(l0 >= v1));
	BOOST_CHECK(!(v0 >= l1));
	BOOST_CHECK(!(l0 >= l1));
}





BOOST_AUTO_TEST_SUITE_END()
