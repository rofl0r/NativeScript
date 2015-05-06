#include "lua.h"
#include "lua.hpp"
#include "Windows.h"
#include "lua.hpp"

namespace lua
{
	lua_State* init()
	{
		lua_State* L = luaL_newstate();
		luaL_openlibs(L);
		return L;
	}

	void close(lua_State * L)
	{
		lua_close(L);
	}
}