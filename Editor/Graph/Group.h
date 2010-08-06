#pragma once

#include "Editor/Graph/Node.h"
#include "Editor/Graph/Debug.h"

class Group: public Node
{
public:
	///////////////////////////////////////////////////////////////
	// Construction and destruction.
	///////////////////////////////////////////////////////////////

	Group();
	Group(List<Shape *> *nodes);
	virtual ~Group() {}

	///////////////////////////////////////////////////////////////
	// Position and size.
	///////////////////////////////////////////////////////////////

	// Returns the Shape which contains the given point.
	virtual Shape *IsInside(const wxPoint& pt);

	///////////////////////////////////////////////////////////////
	// Drawing.
	///////////////////////////////////////////////////////////////

	// Draws the connections.
	virtual void DrawConnections(wxDC& dc) const;

	///////////////////////////////////////////////////////////////
	// Factory.
	///////////////////////////////////////////////////////////////

	// The class name used in serialization.
	virtual wxString  GetClassName() const { return wxT("group"); }
	// Creates an instance of this shape.
	static Serialized *Create(const wxXmlNode& root) { return NEW(Group, ()); }

	///////////////////////////////////////////////////////////////
	// Lua object.
	///////////////////////////////////////////////////////////////

	virtual wxString   GetLuaId() const { return m_id; }
	//virtual Metamethod GetLuaIndex() const { return LuaIndex; }

	///////////////////////////////////////////////////////////////
	// Persistance.
	///////////////////////////////////////////////////////////////

	// Deserialize the shape from a wxXmlNode.
	virtual void Deserialize(const wxXmlNode& root);

	///////////////////////////////////////////////////////////////
	// Node-specific stuff.
	///////////////////////////////////////////////////////////////

	virtual void GenerateCode(CodeTextCtrl *codectrl, Report *reportctrl) const;
	virtual bool CheckInputConstraits() const;

protected:
	virtual void DeserializeChildren(const wxXmlNode& root);
	// __index metamethod.
	//static int LuaIndex(lua_State *L);
	// __index metamethod for members.
	//static int MemberLuaIndex(lua_State *L);
	// Returns the value of a member.
	//static int MemberLuaGetValue(lua_State *L);
	// Sets the value of a member.
	//static int MemberLuaSetValue(lua_State *L);
	void AddPorts();
	wxPoint GetTopLeft(List<Shape *> *shapes) const;
};

