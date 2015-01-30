#include <luapp/lua.hpp>
#include <luapp/luainc.h>
#include "fixtures.h"
#include <fstream>
#include <cstdio>

using std::ofstream;



int fxSignal::signal = 0;
size_t fxFiles::entryCount = 0;


fxGlobalVal::fxGlobalVal():
	vs(context)
{
	lua_pushnumber(context, 3.14);
	lua_setglobal(context, "val");
	vs.push_back(3.14);
}


fxFiles::fxFiles()
{
	if(entryCount == 0) {
		ofstream goodFile("test_good.lua"), badFile("test_bad.lua");
		if(!goodFile.is_open() || ! badFile.is_open())
			throw std::runtime_error("Could not create test files!");
		goodFile << "fnSignal()";
		badFile << "TestBadTable = {";
	}
	++entryCount;
}



fxFiles::~fxFiles()
{
	--entryCount;
	if(entryCount == 0) {
		std::remove("test_good.lua");
		std::remove("test_bad.lua");
	}
}
