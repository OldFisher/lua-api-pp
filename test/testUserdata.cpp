#include <boost/test/unit_test.hpp>

#include "fixtures.h"
#include <cstring>
#include <stdexcept>


struct Udata{int x;};
LUAPP_USERDATA(Udata, "Test.Userdata")
struct Otherdata{int x;};
LUAPP_USERDATA(Otherdata, "Test.Otherdata")




struct fxud: public fxContext
{
	fxud()
	{
		context.mt<Udata>() = lua::Table::records( context,
			1, 1,
			2, 2
		);
		context.mt<Otherdata>() = lua::Table::records(context);
	}
};




BOOST_AUTO_TEST_SUITE(Userdata)



BOOST_FIXTURE_TEST_CASE(Registration, fxud)
{
	BOOST_CHECK(context.mt<Udata>()[2].cast<int>() == 2);
}



BOOST_FIXTURE_TEST_CASE(TypeControl, fxud)
{
	context.global["val"] = Udata{42};
	BOOST_CHECK(context.global["val"].type() == lua::ValueType::UserData);
	BOOST_CHECK(context.global["val"].is<Udata>());
	BOOST_CHECK(!context.global["val"].is<Otherdata>());
}




BOOST_FIXTURE_TEST_CASE(Transfer, fxud)
{
	context.global["val"] = Udata{42};
	BOOST_CHECK_EQUAL(context.global["val"].cast<Udata>().x, 42);
}



BOOST_AUTO_TEST_SUITE_END()
