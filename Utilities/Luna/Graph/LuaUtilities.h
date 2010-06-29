#pragma once


extern "C"
{
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

#include "Platform/Windows/Windows.h"

class LuaObject
{
public:
	typedef int (*Metamethod)(lua_State *);

	virtual wxString   GetLuaId() const = 0;
	virtual Metamethod GetLuaIndex() const { return LuaIndex; }
	void               PushLua(lua_State *L) const;

protected:
	static int LuaIndex(lua_State *L);
	static int LuaUID(lua_State *L);
};

namespace LuaUtilities
{
	class StackUnwinder
	{
		public:
			StackUnwinder(lua_State *L): m_L(L), m_Unwind(true) { m_Top = lua_gettop(m_L); }
			~StackUnwinder() { if (m_Unwind) lua_settop(m_L, m_Top); }
			void DontUnwind() { m_Unwind = false; }

		private:
			lua_State *m_L;
			int m_Top;
			bool m_Unwind;
	};

	lua_State *NewState();
	void LoadBuffer(lua_State *L, const char *buffer, size_t size, const char *chunkname);
	void LoadResource(lua_State *L, HMODULE hModule, const char *resourcename, const char *chunkname);
	void LoadFile(lua_State *L, const char *filename);
	void Push(lua_State *L, const wxVariant& value);
	wxVariant Get(lua_State *L, int index);
	void Call(lua_State *L, int nargs, int nresults, const char *chunkname = "");
	void Relate(lua_State *L, int selfidx, void *instance);
	void Unrelate(lua_State *L, int selfidx);
	void Unrelate(lua_State *L, void *instance);
	void *GetInstance(lua_State *L, int selfidx);
	void PushSelf(lua_State *L, void *instance);
	void CreatePath(lua_State *L, const char *path);
};

