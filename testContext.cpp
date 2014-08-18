#include <boost/test/unit_test.hpp>

#include "fixtures.h"
#include <stdexcept>


BOOST_AUTO_TEST_SUITE(Context)



BOOST_FIXTURE_TEST_CASE(explicitInitialization, fxContext)
{
	BOOST_CHECK(true);
}


#if(LUAPP_API_VERSION >= 52)
BOOST_FIXTURE_TEST_CASE(versionQuery, fxContext)
{
	BOOST_CHECK(context.getVersion() == 502);
}
#endif	// V52+


BOOST_FIXTURE_TEST_CASE(garbageCollector, fxContext)
{
#if(LUAPP_API_VERSION >= 52)
	BOOST_CHECK(context.queryMemoryTotal() > 0);
	context.gcCollect();
	BOOST_CHECK(context.gcIsRunning());
	context.gcStop();
	BOOST_CHECK(!context.gcIsRunning());
	context.gcResume();
	BOOST_CHECK(context.gcIsRunning());
#else	// V51-
	BOOST_CHECK(context.queryMemoryTotal() > 0);
	context.gcCollect();
	context.gcStop();
	context.gcResume();
#endif	// V52+
}



BOOST_FIXTURE_TEST_CASE(directLua, fxContext)
{
	BOOST_CHECK_EQUAL(gs.getRawState(), context);
	BOOST_CHECK_EQUAL(context.getTop(), 0);
}


BOOST_AUTO_TEST_SUITE_END()
