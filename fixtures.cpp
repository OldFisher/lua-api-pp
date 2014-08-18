#include "luapp/lua.hpp"
#include "luapp/luainc.h"
#include "fixtures.h"

int fxSignal::signal = 0;



fxGlobalVal::fxGlobalVal():
	vs(context)
{
	lua_pushnumber(context, 3.14);
	lua_setglobal(context, "val");
	vs.push_back(3.14);
}
