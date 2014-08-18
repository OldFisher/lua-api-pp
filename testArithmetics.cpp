#include <boost/test/unit_test.hpp>

#include "fixtures.h"
#include <stdexcept>

#if LUAPP_API_VERSION >= 52



struct Udata{int x;};
LUAPP_USERDATA(Udata, "Test.Userdata")


enum Ops {
	Unm = 0,
	Add,
	Sub,
	Mul,
	Div,
	Mod,
	Pow,

    Size
};

static int opcount[Ops::Size];
static inline void resetCounts() {for(auto& count: opcount) count = 0;}
static inline bool checkOthers(Ops op)
{
	for(int i = Ops::Unm; i < Ops::Size; ++i)
		if(i != op && opcount[i] != 0)
			return false;
	return true;
}

// We aren't really chasing computational efficiency here
static inline int mypow(int val, int p) {int rv = val; for(int i = 1; i < p; ++i) rv *= val; return rv;}



using lua::Retval;
using lua::Context;
using lua::mkcf;
using lua::Value;



static Retval unm(Context& c) {++ opcount[Ops::Unm]; return c.ret( -c.args[0].cast<Udata>().x);}
static Retval add(Context& c) {++ opcount[Ops::Add]; return c.ret( c.args[0].cast<Udata>().x + c.args[1].cast<int>());}
static Retval sub(Context& c) {++ opcount[Ops::Sub]; return c.ret( c.args[0].cast<Udata>().x - c.args[1].cast<int>());}
static Retval mul(Context& c) {++ opcount[Ops::Mul]; return c.ret( c.args[0].cast<Udata>().x * c.args[1].cast<int>());}
static Retval div(Context& c) {++ opcount[Ops::Div]; return c.ret( c.args[0].cast<Udata>().x / c.args[1].cast<int>());}
static Retval mod(Context& c) {++ opcount[Ops::Mod]; return c.ret( c.args[0].cast<Udata>().x % c.args[1].cast<int>());}
static Retval pow(Context& c) {++ opcount[Ops::Pow]; return c.ret( mypow(c.args[0].cast<Udata>().x, c.args[1].cast<int>()));}




struct fxArith: public fxContext{

	struct Dummy {
		explicit Dummy(Context& context)
		{
        context.mt<Udata>() = lua::Table::records(context,
			"__unm", mkcf<unm>,
			"__add", mkcf<add>,
			"__sub", mkcf<sub>,
			"__mul", mkcf<mul>,
			"__div", mkcf<div>,
			"__mod", mkcf<mod>,
			"__pow", mkcf<pow>
		);
		}
	} dummy {context};

	Value v2, v3, uv;

	fxArith():
		v2(2, context),
		v3(3, context),
		uv(Udata{2}, context)
	{
		resetCounts();
		context.global["l2"] = 2;
		context.global["l3"] = 3;
		context.global["ul"] = Udata{2};
	}
};

#define l2 context.global["l2"]
#define l3 context.global["l3"]
#define ul context.global["ul"]

BOOST_AUTO_TEST_SUITE(Arithmetics)



BOOST_FIXTURE_TEST_CASE(UnaryMinus, fxArith)
{
	BOOST_CHECK_EQUAL((-v2).cast<int>(), -2);
	BOOST_CHECK_EQUAL((-l2).cast<int>(), -2);
	BOOST_CHECK_EQUAL((-uv).cast<int>(), -2);
	BOOST_CHECK_EQUAL((-ul).cast<int>(), -2);
	BOOST_CHECK_EQUAL(opcount[Ops::Unm], 2);
	BOOST_CHECK(checkOthers(Ops::Unm));
}



BOOST_FIXTURE_TEST_CASE(Plus, fxArith)
{
	BOOST_CHECK_EQUAL((v2 + v3).cast<int>(), 5);
	BOOST_CHECK_EQUAL((l2 + l3).cast<int>(), 5);
	BOOST_CHECK_EQUAL((v2 + l3).cast<int>(), 5);
	BOOST_CHECK_EQUAL((l2 + v3).cast<int>(), 5);
	BOOST_CHECK_EQUAL((v2 + 3).cast<int>(), 5);
	BOOST_CHECK_EQUAL((2 + v3).cast<int>(), 5);
	BOOST_CHECK_EQUAL((l2 + 3).cast<int>(), 5);
	BOOST_CHECK_EQUAL((2 + l3).cast<int>(), 5);
	BOOST_CHECK_EQUAL((uv + 3).cast<int>(), 5);
	BOOST_CHECK_EQUAL((ul + 3).cast<int>(), 5);
	BOOST_CHECK_EQUAL(opcount[Ops::Add], 2);
	BOOST_CHECK(checkOthers(Ops::Add));
}




BOOST_FIXTURE_TEST_CASE(Minus, fxArith)
{
	BOOST_CHECK_EQUAL((v2 - v3).cast<int>(), -1);
	BOOST_CHECK_EQUAL((l2 - l3).cast<int>(), -1);
	BOOST_CHECK_EQUAL((v2 - l3).cast<int>(), -1);
	BOOST_CHECK_EQUAL((l2 - v3).cast<int>(), -1);
	BOOST_CHECK_EQUAL((v2 - 3).cast<int>(), -1);
	BOOST_CHECK_EQUAL((2 - v3).cast<int>(), -1);
	BOOST_CHECK_EQUAL((l2 - 3).cast<int>(), -1);
	BOOST_CHECK_EQUAL((2 - l3).cast<int>(), -1);
	BOOST_CHECK_EQUAL((uv - 3).cast<int>(), -1);
	BOOST_CHECK_EQUAL((ul - 3).cast<int>(), -1);
	BOOST_CHECK_EQUAL(opcount[Ops::Sub], 2);
	BOOST_CHECK(checkOthers(Ops::Sub));
}




BOOST_FIXTURE_TEST_CASE(Multiply, fxArith)
{
	BOOST_CHECK_EQUAL((v2 * v3).cast<int>(), 6);
	BOOST_CHECK_EQUAL((l2 * l3).cast<int>(), 6);
	BOOST_CHECK_EQUAL((v2 * l3).cast<int>(), 6);
	BOOST_CHECK_EQUAL((l2 * v3).cast<int>(), 6);
	BOOST_CHECK_EQUAL((v2 * 3).cast<int>(), 6);
	BOOST_CHECK_EQUAL((2 * v3).cast<int>(), 6);
	BOOST_CHECK_EQUAL((l2 * 3).cast<int>(), 6);
	BOOST_CHECK_EQUAL((2 * l3).cast<int>(), 6);
	BOOST_CHECK_EQUAL((uv * 3).cast<int>(), 6);
	BOOST_CHECK_EQUAL((ul * 3).cast<int>(), 6);
	BOOST_CHECK_EQUAL(opcount[Ops::Mul], 2);
	BOOST_CHECK(checkOthers(Ops::Mul));
}




BOOST_FIXTURE_TEST_CASE(Divide, fxArith)
{
	uv = Udata{10};
	ul = Udata{10};
	BOOST_CHECK_EQUAL((v3 / v2).cast<double>(), 1.5);
	BOOST_CHECK_EQUAL((l3 / l2).cast<double>(), 1.5);
	BOOST_CHECK_EQUAL((v3 / l2).cast<double>(), 1.5);
	BOOST_CHECK_EQUAL((l3 / v2).cast<double>(), 1.5);
	BOOST_CHECK_EQUAL((v3 / 2).cast<double>(), 1.5);
	BOOST_CHECK_EQUAL((3 / v2).cast<double>(), 1.5);
	BOOST_CHECK_EQUAL((l3 / 2).cast<double>(), 1.5);
	BOOST_CHECK_EQUAL((3 / l2).cast<double>(), 1.5);
	BOOST_CHECK_EQUAL((uv / 3).cast<double>(), 3);
	BOOST_CHECK_EQUAL((ul / 3).cast<double>(), 3);
	BOOST_CHECK_EQUAL(opcount[Ops::Div], 2);
	BOOST_CHECK(checkOthers(Ops::Div));
}



BOOST_FIXTURE_TEST_CASE(Modulus, fxArith)
{
	uv = Udata{10};
	ul = Udata{10};
	BOOST_CHECK_EQUAL((v3 % v2).cast<int>(), 1);
	BOOST_CHECK_EQUAL((l3 % l2).cast<int>(), 1);
	BOOST_CHECK_EQUAL((v3 % l2).cast<int>(), 1);
	BOOST_CHECK_EQUAL((l3 % v2).cast<int>(), 1);
	BOOST_CHECK_EQUAL((v3 % 2).cast<int>(), 1);
	BOOST_CHECK_EQUAL((3 % v2).cast<int>(), 1);
	BOOST_CHECK_EQUAL((l3 % 2).cast<int>(), 1);
	BOOST_CHECK_EQUAL((3 % l2).cast<int>(), 1);
	BOOST_CHECK_EQUAL((uv % 3).cast<int>(), 1);
	BOOST_CHECK_EQUAL((ul % 3).cast<int>(), 1);
	BOOST_CHECK_EQUAL(opcount[Ops::Mod], 2);
	BOOST_CHECK(checkOthers(Ops::Mod));
}



BOOST_FIXTURE_TEST_CASE(Power, fxArith)
{
	BOOST_CHECK_EQUAL((v2 ^ v3).cast<int>(), 8);
	BOOST_CHECK_EQUAL((l2 ^ l3).cast<int>(), 8);
	BOOST_CHECK_EQUAL((v2 ^ l3).cast<int>(), 8);
	BOOST_CHECK_EQUAL((l2 ^ v3).cast<int>(), 8);
	BOOST_CHECK_EQUAL((v2 ^ 3).cast<int>(), 8);
	BOOST_CHECK_EQUAL((2 ^ v3).cast<int>(), 8);
	BOOST_CHECK_EQUAL((l2 ^ 3).cast<int>(), 8);
	BOOST_CHECK_EQUAL((2 ^ l3).cast<int>(), 8);
	BOOST_CHECK_EQUAL((uv ^ 3).cast<int>(), 8);
	BOOST_CHECK_EQUAL((ul ^ 3).cast<int>(), 8);
	BOOST_CHECK_EQUAL(opcount[Ops::Pow], 2);
	BOOST_CHECK(checkOthers(Ops::Pow));
}



BOOST_FIXTURE_TEST_CASE(Chaining, fxArith)
{
	BOOST_CHECK_EQUAL((v2 + l2*v3  + (l3^v2) - l2).cast<int>(), 15);
}




BOOST_FIXTURE_TEST_CASE(Discard, fxArith)
{
	uv + 1;
	ul + 1;
#ifdef LUAPP_NONDISCARDABLE_ARITHMETICS
	BOOST_CHECK_EQUAL(opcount[Ops::Add], 2);
	BOOST_CHECK(checkOthers(Ops::Add));
#else
	BOOST_CHECK_EQUAL(opcount[Ops::Add], 0);
	BOOST_CHECK(checkOthers(Ops::Add));
#endif // LUAPP_NODISCARD_ARITHMETICS
}

BOOST_AUTO_TEST_SUITE_END()

#endif	// V52+
