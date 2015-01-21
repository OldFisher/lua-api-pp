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
#if(LUAPP_API_VERSION >= 53)
	Idiv,
	Band,
	Bor,
	Bxor,
	Bneg,
	Shl,
	Shr,
#endif	// V53+

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
static Retval div(Context& c) {++ opcount[Ops::Div]; return c.ret( c.args[0].cast<Udata>().x / c.args[1].cast<double>());}
static Retval mod(Context& c) {++ opcount[Ops::Mod]; return c.ret( c.args[0].cast<Udata>().x % c.args[1].cast<int>());}
static Retval pow(Context& c) {++ opcount[Ops::Pow]; return c.ret( mypow(c.args[0].cast<Udata>().x, c.args[1].cast<int>()));}
#if(LUAPP_API_VERSION >= 53)
static Retval do_idiv(Context& c) {++ opcount[Ops::Idiv]; return c.ret( c.args[0].cast<Udata>().x / c.args[1].cast<int>());}
static Retval do_band(Context& c) {++ opcount[Ops::Band]; return c.ret( c.args[0].cast<Udata>().x & c.args[1].cast<int>());}
static Retval do_bor (Context& c) {++ opcount[Ops::Bor ]; return c.ret( c.args[0].cast<Udata>().x | c.args[1].cast<int>());}
static Retval do_bxor(Context& c) {++ opcount[Ops::Bxor]; return c.ret( c.args[0].cast<Udata>().x ^ c.args[1].cast<int>());}
static Retval do_bneg(Context& c) {++ opcount[Ops::Bneg]; return c.ret( ~static_cast<unsigned long long>(c.args[0].cast<Udata>().x));}
static Retval do_shl (Context& c) {++ opcount[Ops::Shl ]; return c.ret( c.args[0].cast<Udata>().x << c.args[1].cast<int>());}
static Retval do_shr (Context& c) {++ opcount[Ops::Shr ];	return c.ret( c.args[0].cast<Udata>().x >> c.args[1].cast<int>());}
#endif	// V53+



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
#if(LUAPP_API_VERSION >= 53)
			,
			"__idiv", mkcf<do_idiv>,
			"__band", mkcf<do_band>,
			"__bor" , mkcf<do_bor >,
			"__bxor", mkcf<do_bxor>,
			"__bnot", mkcf<do_bneg>,
			"__shl" , mkcf<do_shl>,
			"__shr" , mkcf<do_shr>
#endif	// V53+
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
	BOOST_CHECK_EQUAL((uv / 3).cast<double>(), 10.0 / 3);
	BOOST_CHECK_EQUAL((ul / 3).cast<double>(), 10.0 / 3);
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


#if(LUAPP_API_VERSION >= 53)
BOOST_FIXTURE_TEST_CASE( IntegerDivision, fxArith)
{
	uv = Udata{10};
	ul = Udata{10};
	BOOST_CHECK_EQUAL(idiv(v3, v2).cast<double>(), 1);
	BOOST_CHECK_EQUAL(idiv(l3, l2).cast<double>(), 1);
	BOOST_CHECK_EQUAL(idiv(v3, l2).cast<double>(), 1);
	BOOST_CHECK_EQUAL(idiv(l3, v2).cast<double>(), 1);
	BOOST_CHECK_EQUAL(idiv(v3, 2).cast<double>(), 1);
	BOOST_CHECK_EQUAL(idiv(3, v2).cast<double>(), 1);
	BOOST_CHECK_EQUAL(idiv(l3, 2).cast<double>(), 1);
	BOOST_CHECK_EQUAL(idiv(3, l2).cast<double>(), 1);
	BOOST_CHECK_EQUAL(idiv(uv, 3).cast<double>(), 3);
	BOOST_CHECK_EQUAL(idiv(ul, 3).cast<double>(), 3);
	BOOST_CHECK_EQUAL(opcount[Ops::Idiv], 2);
	BOOST_CHECK(checkOthers(Ops::Idiv));
}



BOOST_FIXTURE_TEST_CASE(BinaryAnd, fxArith)
{
    BOOST_CHECK_EQUAL(band(v3, v2).to<int>(), 2);
    BOOST_CHECK_EQUAL(band(l3, l2).to<int>(), 2);
    BOOST_CHECK_EQUAL(band(v3, l2).to<int>(), 2);
    BOOST_CHECK_EQUAL(band(l3, v2).to<int>(), 2);
	BOOST_CHECK_EQUAL(band(v3, 2).to<int>(), 2);
	BOOST_CHECK_EQUAL(band(3, v2).to<int>(), 2);
	BOOST_CHECK_EQUAL(band(l3, 2).to<int>(), 2);
	BOOST_CHECK_EQUAL(band(3, l2).to<int>(), 2);
	BOOST_CHECK_EQUAL(band(uv, 3).to<int>(), 2);
	BOOST_CHECK_EQUAL(band(ul, 3).to<int>(), 2);
	BOOST_CHECK_EQUAL(opcount[Ops::Band], 2);
	BOOST_CHECK(checkOthers(Ops::Band));
}



BOOST_FIXTURE_TEST_CASE(BinaryOr, fxArith)
{
    BOOST_CHECK_EQUAL(bor(v3, v2).to<int>(), 3);
    BOOST_CHECK_EQUAL(bor(l3, l2).to<int>(), 3);
    BOOST_CHECK_EQUAL(bor(v3, l2).to<int>(), 3);
    BOOST_CHECK_EQUAL(bor(l3, v2).to<int>(), 3);
	BOOST_CHECK_EQUAL(bor(v3, 2).to<int>(), 3);
	BOOST_CHECK_EQUAL(bor(3, v2).to<int>(), 3);
	BOOST_CHECK_EQUAL(bor(l3, 2).to<int>(), 3);
	BOOST_CHECK_EQUAL(bor(3, l2).to<int>(), 3);
	BOOST_CHECK_EQUAL(bor(uv, 3).to<int>(), 3);
	BOOST_CHECK_EQUAL(bor(ul, 3).to<int>(), 3);
	BOOST_CHECK_EQUAL(opcount[Ops::Bor], 2);
	BOOST_CHECK(checkOthers(Ops::Bor));
}



BOOST_FIXTURE_TEST_CASE(BinaryXor, fxArith)
{
    BOOST_CHECK_EQUAL(bxor(v3, v2).to<int>(), 1);
    BOOST_CHECK_EQUAL(bxor(l3, l2).to<int>(), 1);
    BOOST_CHECK_EQUAL(bxor(v3, l2).to<int>(), 1);
    BOOST_CHECK_EQUAL(bxor(l3, v2).to<int>(), 1);
	BOOST_CHECK_EQUAL(bxor(v3,  2).to<int>(), 1);
	BOOST_CHECK_EQUAL(bxor( 3, v2).to<int>(), 1);
	BOOST_CHECK_EQUAL(bxor(l3,  2).to<int>(), 1);
	BOOST_CHECK_EQUAL(bxor( 3, l2).to<int>(), 1);
	BOOST_CHECK_EQUAL(bxor(uv,  3).to<int>(), 1);
	BOOST_CHECK_EQUAL(bxor(ul,  3).to<int>(), 1);
	BOOST_CHECK_EQUAL(opcount[Ops::Bxor], 2);
	BOOST_CHECK(checkOthers(Ops::Bxor));
}



BOOST_FIXTURE_TEST_CASE(BinaryNeg, fxArith)
{
    BOOST_CHECK_EQUAL(bneg(v2).to<unsigned long long>(), 0xFFFFFFFFFFFFFFFDull);
    BOOST_CHECK_EQUAL(bneg(l2).to<unsigned long long>(), 0xFFFFFFFFFFFFFFFDull);
	BOOST_CHECK_EQUAL(bneg(uv).to<unsigned long long>(), 0xFFFFFFFFFFFFFFFDull);
	BOOST_CHECK_EQUAL(bneg(ul).to<unsigned long long>(), 0xFFFFFFFFFFFFFFFDull);
	BOOST_CHECK_EQUAL(opcount[Ops::Bneg], 2);
	BOOST_CHECK(checkOthers(Ops::Bneg));
}



BOOST_FIXTURE_TEST_CASE(ShiftLeft, fxArith)
{
    BOOST_CHECK_EQUAL(shl(v3, v2).to<int>(), 12);
    BOOST_CHECK_EQUAL(shl(l3, l2).to<int>(), 12);
    BOOST_CHECK_EQUAL(shl(v3, l2).to<int>(), 12);
    BOOST_CHECK_EQUAL(shl(l3, v2).to<int>(), 12);
	BOOST_CHECK_EQUAL(shl(v3,  2).to<int>(), 12);
	BOOST_CHECK_EQUAL(shl( 3, v2).to<int>(), 12);
	BOOST_CHECK_EQUAL(shl(l3,  2).to<int>(), 12);
	BOOST_CHECK_EQUAL(shl( 3, l2).to<int>(), 12);
	BOOST_CHECK_EQUAL(shl(uv,  3).to<int>(), 16);
	BOOST_CHECK_EQUAL(shl(ul,  3).to<int>(), 16);
	BOOST_CHECK_EQUAL(opcount[Ops::Shl], 2);
	BOOST_CHECK(checkOthers(Ops::Shl));
}



BOOST_FIXTURE_TEST_CASE(ShiftRight, fxArith)
{
	v3 = 13;
	l3 = 13;
	uv = Udata{17};
	ul = Udata{17};
    BOOST_CHECK_EQUAL(shr(v3, v2).to<int>(), 3);
    BOOST_CHECK_EQUAL(shr(l3, l2).to<int>(), 3);
    BOOST_CHECK_EQUAL(shr(v3, l2).to<int>(), 3);
    BOOST_CHECK_EQUAL(shr(l3, v2).to<int>(), 3);
	BOOST_CHECK_EQUAL(shr(v3,  2).to<int>(), 3);
	BOOST_CHECK_EQUAL(shr(13, v2).to<int>(), 3);
	BOOST_CHECK_EQUAL(shr(l3,  2).to<int>(), 3);
	BOOST_CHECK_EQUAL(shr(13, l2).to<int>(), 3);
	BOOST_CHECK_EQUAL(shr(uv,  3).to<int>(), 2);
	BOOST_CHECK_EQUAL(opcount[Ops::Shr], 1);
	BOOST_CHECK_EQUAL(shr(ul,  3).to<int>(), 2);
	BOOST_CHECK_EQUAL(opcount[Ops::Shr], 2);
	BOOST_CHECK(checkOthers(Ops::Shr));
}
#endif	// V53+

BOOST_AUTO_TEST_SUITE_END()

#endif	// V52+
