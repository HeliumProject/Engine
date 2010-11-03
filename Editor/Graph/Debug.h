#pragma once

#include <exception>

extern "C"
{
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

#include "Platform/Types.h"
#include "Platform/String.h"

#ifdef _DEBUG

#define NEW(clazz, args)     (clazz *)Debug::DumpHeap(TXT("new"), new clazz args, sizeof(clazz), __FILE__, __LINE__ )
#define DESTROY(arg)         do { Debug::DumpHeap(TXT("delete"), (void *)arg, 0, __FILE__, __LINE__ ); delete arg; } while (0)
#define NEWARRAY(type, size) (type *)Debug::DumpHeap(TXT("new[]"), new type[size], sizeof(type) * size, __FILE__, __LINE__)
#define DESTROYARRAY(arg)    do { Debug::DumpHeap(TXT("delete[]"), (void *)arg, 0, __FILE__, __LINE__); delete[] arg; } while (0)
#define THROW(...)           do { Debug::Printf(TXT("Exception at %s(%d):\n\n"), __FILE__, __LINE__); Debug::Printf(__VA_ARGS__); std::string temp; Helium::ConvertString( Debug::Format(__VA_ARGS__), temp ); throw NEW(std::exception, (temp.c_str())); } while (0)
#define BREAK(cond)          do { if (cond) { DebugBreak(); } } while (0)

#else

#define NEW(clazz, args)     new clazz args
#define DESTROY(arg)         delete (arg)
#define NEWARRAY(type, size) new type[size]
#define DESTROYARRAY(arg)    delete[] arg
#define THROW(...)           do { std::string temp; Helium::ConvertString( Debug::Format(__VA_ARGS__), temp ); throw NEW(std::exception, (temp.c_str())); } while (0)
#define BREAK(cond)          do {} while (0)

#endif

class Shape;

namespace Debug
{

	const tchar_t *Format(const tchar_t *fmt, ...);

#ifdef _DEBUG

	void *DumpHeap(const tchar_t *op, void *ptr, size_t size, const char *file, int line);
	void Init(lua_State *L);
	void Printf(const tchar_t *fmt, ...);
	void DumpLuaStack(lua_State *L);
	void Save(Shape *obj, const tchar_t *file);

#else

	void Init(lua_State *L);
	static inline void Printf(const tchar_t *fmt, ...) {}
	static inline void DumpLuaStack(lua_State *L) {}
	static inline void Save(Shape *obj, const tchar_t *file) {}

#endif

};
