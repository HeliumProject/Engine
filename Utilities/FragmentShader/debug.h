#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <exception>

extern "C"
{
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

#ifdef _DEBUG

#define NEW(clazz, args)     (clazz *)Debug::DumpHeap("new", new clazz args, sizeof(clazz), __FILE__, __LINE__)
#define DESTROY(arg)         do { Debug::DumpHeap("delete", (void *)arg, 0, __FILE__, __LINE__); delete arg; } while (0)
#define NEWARRAY(type, size) (type *)Debug::DumpHeap("new[]", new type[size], sizeof(type) * size, __FILE__, __LINE__)
#define DESTROYARRAY(arg)    do { Debug::DumpHeap("delete[]", (void *)arg, 0, __FILE__, __LINE__); delete[] arg; } while (0)
#define THROW(...)           do { Debug::Printf("Exception at %s(%d):\n\n", __FILE__, __LINE__); Debug::Printf(__VA_ARGS__); throw NEW(std::exception, (Debug::Format(__VA_ARGS__))); } while (0)
#define BREAK(cond)          do { if (cond) { DebugBreak(); } } while (0)

#else

#define NEW(clazz, args)     new clazz args
#define DESTROY(arg)         delete (arg)
#define NEWARRAY(type, size) new type[size]
#define DESTROYARRAY(arg)    delete[] arg
#define THROW(...)           throw NEW(std::exception, (Debug::Format(__VA_ARGS__)))
#define BREAK(cond)          do {} while (0)

#endif

class Shape;

namespace Debug
{

	const char *Format(const char *fmt, ...);

#ifdef _DEBUG

	void *DumpHeap(const char *op, void *ptr, size_t size, const char *file, int line);
	void Init(lua_State *L);
	void Printf(const char *fmt, ...);
	void DumpLuaStack(lua_State *L);
	void Save(Shape *obj, const char *file);

#else

	void Init(lua_State *L);
	static inline void Printf(const char *fmt, ...) {}
	static inline void DumpLuaStack(lua_State *L) {}
	static inline void Save(Shape *obj, const char *file) {}

#endif

};

#endif // __DEBUG_H__
