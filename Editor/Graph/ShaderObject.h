#pragma once

#include "Editor/Graph/Shape.h"
#include "Editor/Graph/LuaUtilities.h"

class Method;

class ShaderObject: public Shape
{
public:
	virtual ~ShaderObject();

	// Adds a method to this object.
	void    AddMethod(const wxString& name, const wxString& target, Method *method);
	// Gets a method of this object.
	Method *GetMethod(const wxString& name, const wxString& target) const;

	// Serialize & de-serialize this object.
	virtual wxXmlNode *Serialize() const;
	virtual void       Deserialize(const wxXmlNode& root);

	// Serialize & de-serialize the methods of this object.
	virtual wxXmlNode *SerializeMethods() const;
	virtual void       DeserializeMethods(const wxXmlNode& root);

	// The type of function returned by GetLuaIndex.
	typedef int (*Metamethod)(lua_State *);

	// Returns the id of this object in Lua scripts.
	virtual wxString   GetLuaId() const = 0;
	// Returns the C function which gets a field of this object.
	virtual Metamethod GetLuaIndex() const { return LuaIndex; }
	// Pushes the object onto the Lua stack.
	void               PushLua(lua_State *L) const;

protected:
	// __index metamethod.
	static int LuaIndex(lua_State *L);
	// Returns a unique identifier for this object.
	static int LuaUID(lua_State *L);
	// Maps name:target to methods.
	std::map<wxString, Method *> m_methods;
};

