#include "Precompile.h"
#include "Editor/Graph/LuaUtilities.h"

#include <string.h>
#include <stdarg.h>
#include <wx/msgdlg.h>

extern "C"
{
	#include "Editor/Graph/expr.h"
}

#include "Editor/Graph/Debug.h"
#include "Editor/Graph/Base64Encode.h"

namespace LuaUtilities
{
	static const char class_lua[] =
		"local setmetatable = debug.setmetatable\n"
		"local gmatch = string.gmatch\n"
		"local match = string.match\n"
		"local strsub = string.sub\n"
		"local pairs = pairs\n"
		"local type = type\n"
		"local unpack = unpack\n"
		"\n"
		"function class(...)\n"
		"	-- create an empty class\n"
		"	local new_class = {}\n"
		"	-- copy all methods from the super classes\n"
		"	for index = 1, #arg do\n"
		"		for name, method in pairs(arg[index]) do\n"
		"			new_class[name] = method\n"
		"		end\n"
		"	end\n"
		"	\n"
		"	-- insert an additional method to check the class of the instance\n"
		"	new_class.instance_of = function(self, check)\n"
		"		if check == new_class then\n"
		"			return true\n"
		"		end\n"
		"		for index = 1, #arg do\n"
		"			if arg[index].instance_of(self, check) then\n"
		"				return true\n"
		"			end\n"
		"		end\n"
		"		return false\n"
		"	end\n"
		"	\n"
		"	-- insert an additional method to return the class of the instance\n"
		"	new_class.get_class = function(self)\n"
		"		return new_class\n"
		"	end\n"
		"	\n"
		"	-- insert an additional method to return an hex id of the instance\n"
		"	new_class.tohex = function(self)\n"
		"		return match(tostring(self), 'table: (.*)')\n"
		"	end\n"
		"	\n"
		"	-- add hex as a alias for tohex\n"
		"	new_class.hex = new_class.tohex\n"
		"	\n"
		"	-- insert an additional method to clone the instance\n"
		"	new_class.clone = function(self)\n"
		"		local function dup(obj, dupped)\n"
		"			if type(obj) == 'table' then\n"
		"				if dupped[obj] then\n"
		"					return dupped[obj]\n"
		"				end\n"
		"				local tab = {}\n"
		"				dupped[obj] = tab\n"
		"				for key, value in pairs(obj) do\n"
		"					if type(key) ~= 'function' and type(value) ~= 'function' then\n"
		"						tab[dup(key, dupped)] = dup(value, dupped)\n"
		"					end\n"
		"				end\n"
		"				return tab\n"
		"			end\n"
		"			return obj\n"
		"		end\n"
		"		local clone, dupped = {}, {}\n"
		"		for key, value in pairs(self) do\n"
		"			if type(key) ~= 'function' and type(value) ~= 'function' then\n"
		"				clone[dup(key, dupped)] = dup(value, dupped)\n"
		"			end\n"
		"		end\n"
		"		new_class.make_instance(clone)\n"
		"		return clone\n"
		"	end\n"
		"	\n"
		"	-- insert an additional method to return the class name of an instance (only works if the class is accessible from the global space)\n"
		"	new_class.class_name = function()\n"
		"		local function find(space, name, visited)\n"
		"			if visited[space] then\n"
		"				return nil\n"
		"			end\n"
		"			visited[space] = true\n"
		"			for key, value in pairs(space) do\n"
		"				if value == new_class then\n"
		"					return name .. '/' .. key\n"
		"				end\n"
		"				if type(value) == 'table' then\n"
		"					local found = find(value, name .. '/' .. key, visited)\n"
		"					if found ~= nil then\n"
		"						return found\n"
		"					end\n"
		"				end\n"
		"			end\n"
		"			return nil\n"
		"		end\n"
		"		local found = find(_G, '_G', {})\n"
		"		if found then\n"
		"			return strsub(found, 4, -1)\n"
		"		end\n"
		"		return '?'\n"
		"	end\n"
		"	\n"
		"	-- insert an additional method to return a string representation of the instance\n"
		"	if not new_class.tostring then\n"
		"		new_class.tostring = function(self)\n"
		"			return self.class_name() .. '@' .. self.tohex()\n"
		"		end\n"
		"	end\n"
		"	\n"
		"	-- insert an additional method to make any given table an instance of the class\n"
		"	new_class.make_instance = function(self)\n"
		"		-- create closures that automatically pass the instance as the first parameter for all methods\n"
		"		for name, method in pairs(new_class) do\n"
		"			self[name] = function(...)\n"
		"				return method(self, unpack(arg))\n"
		"			end\n"
		"		end\n"
		"		-- remove the new and make_instance methods\n"
		"		self.new = nil\n"
		"		self.make_instance = nil\n"
		"	end\n"
		"	\n"
		"	-- create a __call metamethod that creates a new instance of the class\n"
		"	local meta = {}\n"
		"	meta.__call = function(...)\n"
		"		-- the first argument is the class, shift left all other arguments\n"
		"		for i = 2, #arg do\n"
		"			arg[i - 1] = arg[i]\n"
		"		end\n"
		"		arg[#arg] = nil\n"
		"		-- create an empty instance\n"
		"		local self = {}\n"
		"		new_class.make_instance(self, arg)\n"
		"		-- call the new method to initialize the instance\n"
		"		new_class.new(self, unpack(arg))\n"
		"		-- return the newly created instance\n"
		"		return self\n"
		"	end\n"
		"	\n"
		"	-- debug metamethods\n"
		"	--[[meta.__index = function(tab, key)\n"
		"		if tab.class_name and tab.class_name() == 'Lane' then\n"
		"			message_box('__index', tab, '[', key, '] ?')\n"
		"		end\n"
		"		return rawget(tab, key)\n"
		"	end\n"
		"	meta.__newindex = function(tab, key, value)\n"
		"		if tab.class_name and tab.class_name() == 'Lane' then\n"
		"			message_box('__newindex', tostring(tab), '[', tostring(key), '] = ', tostring(value))\n"
		"		end\n"
		"		return rawset(tab, key, value)\n"
		"	end]]\n"
		"	\n"
		"	-- set the metatable of the class\n"
		"	setmetatable(new_class, meta)\n"
		"	-- return it\n"
		"	return new_class\n"
		"end\n"
		"\n"
		"function class_by_name(name)\n"
		"	local class = _G\n"
		"	for part in gmatch(name, '[^/]+') do\n"
		"		class = class[part]\n"
		"		if not class then\n"
		"			return nil\n"
		"		end\n"
		"	end\n"
		"	return class\n"
		"end\n";

	static const char node_lua[] =
		"Node = class()\n"
		"\n"
		"function Node:new()\n"
		"end\n";

	static int
	OpenIO(lua_State *L)
	{
		luaopen_io(L);
		return 0;
	}

	static int
	Panic(lua_State *L)
	{
        tstring temp;
        Helium::ConvertString( lua_tostring(L, -1), temp);
        wxMessageBox(temp, wxT( "Lua panic!" ) );
		THROW(TXT("%s"), temp.c_str());
	}

	static expr_function_t *
	GetFunction(void *user_data, const char *name)
	{
		return NULL;
	}

	static expr_variable_t *
	GetVariable(void *user_data, const char *name)
	{
		return NULL;
	}

	static int
	Evaluate(lua_State *L)
	{
		const char *expr = luaL_checkstring(L, 1);
		expr_config_t config;
		config.get_function = GetFunction;
		config.get_variable = GetVariable;
		double result;
		if (expr_evaluate_string(&config, expr, &result) != EXPR_OK)
		{
			lua_pushnumber(L, 0);
			return 1;
		}
		lua_pushnumber(L, result);
		return 1;
	}

	static int
	Base64(lua_State *L)
	{
		const char *plain = luaL_checkstring(L, 1);
		const char *encoded = Base64Encode((void *)plain, strlen(plain));
		lua_pushstring(L, encoded);
		DESTROY(encoded);
		return 1;
	}

	static int
	Unbase64(lua_State *L)
	{
		const char *encoded = luaL_checkstring(L, 1);
		size_t len;
		void *plain = Base64Decode(encoded, &len);
		lua_pushlstring(L, (const char *)plain, len);
		DESTROY(plain);
		return 1;
	}

	lua_State *
	NewState()
	{
		lua_State *L;
		// Create the Lua state.
		L = luaL_newstate();
		// Set the panic function.
		lua_atpanic(L, Panic);
		// Open all auxiliary libraries.
		luaopen_base(L);
		luaopen_string(L);
		luaopen_math(L);
		luaopen_os(L);
		luaopen_table(L);
		luaopen_debug(L);
		// Hum... luaopen_io is failing when called directly. Dunno why.
		//luaopen_io(L);
		lua_pushcfunction(L, OpenIO);
		lua_call(L, 0, 0);
		// Register some functions in math.
		lua_getglobal(L, "math");
		lua_pushcfunction(L, Evaluate);
		lua_setfield(L, -2, "evaluate");
		lua_pop(L, 1);
		// string.
		lua_getglobal(L, "string");
		lua_pushcfunction(L, Base64);
		lua_setfield(L, -2, "base64");
		lua_pushcfunction(L, Unbase64);
		lua_setfield(L, -2, "unbase64");
		lua_pop(L, 1);
		// Make the OOP system available.
		LoadBuffer(L, class_lua, sizeof(class_lua) - 1, "class");
		LuaUtilities::Call(L, 0, 0);
		// Load the Lua Node class.
		LoadBuffer(L, node_lua, sizeof(node_lua) - 1, "node");
		LuaUtilities::Call(L, 0, 0);
		// Set top to zero to remove dangling tables on the stack.
		lua_settop(L, 0);
		return L;
	}

	static void
	CheckResult(lua_State *L, int res, const char *chunkname = "")
	{
		if (res != 0)
		{
			// Throw an exception.
			switch (res)
			{
				case LUA_ERRFILE:
				case LUA_ERRSYNTAX:
				case LUA_ERRRUN:
				case LUA_ERRMEM:
				case LUA_ERRERR:
                    {
                        tstring temp;
                        Helium::ConvertString( lua_tostring(L, -1), temp);
					    BREAK(!strcmp(chunkname, ""));
					    THROW(TXT("%s: %s"), chunkname, temp);
                    }
			}
			THROW(TXT("%s: Unknown Lua error."), chunkname);
		}
	}

	void
	LoadBuffer(lua_State *L, const char *buffer, size_t size, const char *chunkname)
	{
		CheckResult(L, luaL_loadbuffer(L, buffer, size, chunkname));
	}

	void
	LoadResource(lua_State *L, HMODULE hModule, const char *resourcename, const char *chunkname)
	{
        tstring temp;
        Helium::ConvertString( resourcename, temp );
        HRSRC src = FindResource(hModule, temp.c_str(), RT_RCDATA);
		if (src == NULL)
		{
			THROW(TXT("Resource not found"));
		}
		HGLOBAL res = LoadResource(NULL, src);
		if (res == NULL)
		{
			THROW(TXT("Error loading resource"));
		}
		DWORD size = SizeofResource(NULL, src);
		const char *code = (const char *)LockResource(res);
		if (code == NULL)
		{
			THROW(TXT("Error locking resource"));
		}
		LoadBuffer(L, code, size, chunkname);
	}

	void
	LoadFile(lua_State *L, const char *filename)
	{
		CheckResult(L, luaL_loadfile(L, filename));
	}

	void
	Push(lua_State *L, const wxVariant& value)
	{
		wxString type = value.GetType();
		if (type == wxT("bool"))
		{
			lua_pushboolean(L, value.GetBool());
		}
		else if (type == wxT("double"))
		{
			lua_pushnumber(L, value.GetDouble());
		}
		else if (type == wxT("long"))
		{
			lua_pushnumber(L, value.GetLong());
		}
		else if (type == wxT("string"))
		{
            std::string temp;
            Helium::ConvertString( (const wxChar*)value.GetString().c_str(), temp );
            lua_pushlstring(L, temp.c_str(), temp.length());
		}
		else
		{
			lua_pushnil(L);
		}
	}

	wxVariant
	Get(lua_State *L, int index)
	{
		wxVariant result;
		switch (lua_type(L, -1))
		{
		case LUA_TNIL:
		case LUA_TTABLE:
		case LUA_TFUNCTION:
		case LUA_TUSERDATA:
		case LUA_TTHREAD:
		case LUA_TLIGHTUSERDATA:
			result = (void *)0;
			break;
		case LUA_TNUMBER:
			result = lua_tonumber(L, -1);
			break;
		case LUA_TBOOLEAN:
			result = (bool)(lua_toboolean(L, -1) != 0);
			break;
		case LUA_TSTRING:
            {
			    tstring temp;
                Helium::ConvertString( lua_tostring(L, -1), temp );
                wxString tempStr( temp );
                result = tempStr;
            }
			break;
		}
		return result;
	}

	// The next two functions were copied verbatim from ldblib.c from the Lua sources

	static lua_State *getthread(lua_State *L, int *arg) {
		if (lua_isthread(L, 1)) {
			*arg = 1;
			return lua_tothread(L, 1);
		}
		else {
			*arg = 0;
			return L;
		}
	}

	#define LEVELS1	12	/* size of the first part of the stack */
	#define LEVELS2	10	/* size of the second part of the stack */

	static int db_errorfb(lua_State *L) {
		int level;
		int firstpart = 1;  /* still before eventual `...' */
		int arg;
		lua_State *L1 = getthread(L, &arg);
		lua_Debug ar;
		if (lua_isnumber(L, arg+2)) {
			level = (int)lua_tointeger(L, arg+2);
			lua_pop(L, 1);
		}
		else
		level = (L == L1) ? 1 : 0;  /* level 0 may be this own function */
		if (lua_gettop(L) == arg)
			lua_pushliteral(L, "");
		else if (!lua_isstring(L, arg+1)) return 1;  /* message is not a string */
		else lua_pushliteral(L, "\n");
		lua_pushliteral(L, "stack traceback:");
		while (lua_getstack(L1, level++, &ar)) {
			if (level > LEVELS1 && firstpart) {
				/* no more than `LEVELS2' more levels? */
				if (!lua_getstack(L1, level+LEVELS2, &ar))
					level--;  /* keep going */
				else {
					lua_pushliteral(L, "\n\t...");  /* too many levels */
					while (lua_getstack(L1, level+LEVELS2, &ar))  /* find last levels */
						level++;
				}
				firstpart = 0;
				continue;
			}
			lua_pushliteral(L, "\n\t");
			lua_getinfo(L1, "Snl", &ar);
			lua_pushfstring(L, "%s:", ar.short_src);
			if (ar.currentline > 0)
				lua_pushfstring(L, "%d:", ar.currentline);
			if (*ar.namewhat != '\0')  /* is there a name? */
				lua_pushfstring(L, " in function " LUA_QS, ar.name);
			else {
				if (*ar.what == 'm')  /* main? */
					lua_pushfstring(L, " in main chunk");
				else if (*ar.what == 'C' || *ar.what == 't')
					lua_pushliteral(L, " ?");  /* C function or tail call */
				else
					lua_pushfstring(L, " in function <%s:%d>", ar.short_src, ar.linedefined);
			}
			lua_concat(L, lua_gettop(L) - arg);
		}
		lua_concat(L, lua_gettop(L) - arg);
		return 1;
	}

	void
	Call(lua_State *L, int nargs, int nresults, const char *chunkname)
	{
		// Push the error handling function...
		lua_pushcfunction(L, db_errorfb);
		// ... and move it to just before the function we want to call.
		int errpos = lua_gettop(L) - nargs - 1;
		lua_insert(L, errpos);
		// Make the call.
		int res = lua_pcall(L, nargs, nresults, errpos);
		if (res == 0)
		{
			// Remove the error handling function.
			lua_remove(L, errpos);
			// Success!
			return;
		}
		// Oops...
		CheckResult(L, res, chunkname);
	}

	void
	Relate(lua_State *L, int selfidx, void *instance)
	{
		lua_pushvalue(L, selfidx);
		lua_pushlightuserdata(L, instance);
		lua_settable(L, LUA_REGISTRYINDEX);
		lua_pushlightuserdata(L, instance);
		lua_pushvalue(L, selfidx);
		lua_settable(L, LUA_REGISTRYINDEX);
	}

	void Unrelate(lua_State *L, int selfidx)
	{
		lua_pushvalue(L, selfidx);
		lua_gettable(L, LUA_REGISTRYINDEX);
		lua_pushnil(L);
		lua_settable(L, LUA_REGISTRYINDEX);
		lua_pushvalue(L, selfidx);
		lua_pushnil(L);
		lua_settable(L, LUA_REGISTRYINDEX);
	}

	void Unrelate(lua_State *L, void *instance)
	{
		lua_pushlightuserdata(L, instance);
		lua_gettable(L, LUA_REGISTRYINDEX);
		lua_pushnil(L);
		lua_settable(L, LUA_REGISTRYINDEX);
		lua_pushlightuserdata(L, instance);
		lua_pushnil(L);
		lua_settable(L, LUA_REGISTRYINDEX);
	}

	void *
	GetInstance(lua_State *L, int selfidx)
	{
		lua_pushvalue(L, selfidx);
		lua_gettable(L, LUA_REGISTRYINDEX);
		void *instance = lua_touserdata(L, -1);
		lua_pop(L, 1);
		return instance;
	}

	void
	PushSelf(lua_State *L, void *instance)
	{
		lua_pushlightuserdata(L, instance);
		lua_gettable(L, LUA_REGISTRYINDEX);
	}

	void
	CreatePath(lua_State *L, const char *path)
	{
		lua_pushvalue(L, LUA_GLOBALSINDEX);		// _G
		const char *begin = path;
		for (;;)
		{
			const char *end = strchr(begin, '/');
			if (end == NULL)
			{
				end = begin + strlen(begin);
			}
			int len = end - begin;
			lua_pushlstring(L, begin, len);		// _G "part"
			lua_gettable(L, -2);				// _G {part}
			if (lua_isnil(L, -1))
			{
				lua_pop(L, 1);					// _G
				lua_newtable(L);				// _G {part}
				lua_pushlstring(L, begin, len);	// _G {part} "part"
				lua_pushvalue(L, -2);			// _G {part} "part" {part}
				lua_settable(L, -4);			// _G {part}
			}
			lua_remove(L, -2);					// {part}
			if (*end == 0)
			{
				break;
			}
			begin = end + 1;
		}
	}
};
