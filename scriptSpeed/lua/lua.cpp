#include <string.h>
#include <cstdlib>

#include "lua.h"

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

	int loadScriptFromFile(lua_State * L, char* fileName)
	{
		char *s = (char*)malloc(strlen(fileName) + 15);
		sprintf(s, "%s/%s", SS_LUA_SCRIPT_PATH, fileName);
		
		// load script with functions
		if (luaL_dofile(L, s) != 0) {
			printf("Lua Error: %s\n", lua_tostring(L, -1));
			return 1;
		}
		free(s);

		return 0;
	}
}
