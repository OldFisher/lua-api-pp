#ifndef FIXTURES_H_INCLUDED
#define FIXTURES_H_INCLUDED

#include "luapp/lua.hpp"
#include "luapp/luainc.h"


struct fxState{
	lua::State gs;
};


struct fxContext: public fxState {
	lua::Context context;
	fxContext():
		context(gs.getRawState(), lua::Context::initializeExplicitly)
	{
	}
};


struct fxSignal: public fxContext{

	static int signal;

	static int fnSignal(lua_State*)
	{
		signal = 1;
		return 0;
	}


	bool isSignaled() noexcept
	{
		if(signal != 0) {
			signal = 0;
			return true;
		} else
			return false;
	}


	fxSignal()
	{
		lua_pushcfunction(gs.getRawState(), fnSignal);
		lua_setglobal(gs.getRawState(), "fnSignal");
	}
};


struct fxGlobalVal: public fxContext
{
	// Creates global variable "val" and 1-element Valset
	lua::Valset vs;
	fxGlobalVal();
};


struct fxFiles: public fxSignal {

	static size_t entryCount;

    fxFiles();
    ~fxFiles();
};

#endif // FIXTURES_H_INCLUDED
