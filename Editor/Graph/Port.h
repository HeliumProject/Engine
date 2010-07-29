#pragma once

#include "Graph/ShaderObject.h"
#include "Graph/Property.h"
#include "Graph/CodeTextCtrl.h"
#include "Graph/ReportListCtrl.h"
#include "Graph/LuaUtilities.h"
#include "Graph/Debug.h"

class Port: public ShaderObject
{
public:
	///////////////////////////////////////////////////////////////
	// Construction and destruction.
	///////////////////////////////////////////////////////////////

	// "Regular" constructor.
	Port();
	// Copy constructor.
	//Port(const Port& from): Shape(from), m_is_required(from.m_is_required) {}
	// Virtual destructor.
	virtual ~Port() {}

	void Draw(wxDC& dc) const;

	///////////////////////////////////////////////////////////////
	// Grouping.
	///////////////////////////////////////////////////////////////

	// Adds a shape as a child of this shape.
	virtual void AddChild(Shape *child) {}

	///////////////////////////////////////////////////////////////
	// Factory.
	///////////////////////////////////////////////////////////////

	// The class name used in serialization.
	virtual wxString   GetClassName() const          { return wxT("port"); }
	// Creates an instance of this shape.
	static Serialized *Create(const wxXmlNode& root) { return (Port *)NULL; }

	///////////////////////////////////////////////////////////////
	// Lua object.
	///////////////////////////////////////////////////////////////

	virtual wxString   GetLuaId() const { return m_id; }
	virtual Metamethod GetLuaIndex() const { return LuaIndex; }

	///////////////////////////////////////////////////////////////
	// Port-specific stuff.
	///////////////////////////////////////////////////////////////

	// Possible port statuses.
	enum Status { Ok, Disconnected, Required, TypeMismatch };
	// Get a string describing the type of the port.
	virtual wxString GetType() const = 0;
	// Get the status of the port.
	virtual Status   GetStatus() const = 0;

protected:
	// __index metamethod.
	static int LuaIndex(lua_State *L);
	// Returns the port's type.
	static int LuaGetType(lua_State *L);
	// Properties.
	wxString m_id;
	bool m_is_required;
};

class OutputPort;

class InputPort: public Port
{
public:
	///////////////////////////////////////////////////////////////
	// Construction and destruction.
	///////////////////////////////////////////////////////////////

	InputPort(): Port() {}
	virtual ~InputPort() {}

	///////////////////////////////////////////////////////////////
	// Connection to other shapes.
	///////////////////////////////////////////////////////////////

	// Returns the connection point.
	virtual wxPoint GetConnPoint() const;
	// Adds a shape as a starting point for a connection starting in this shape.
	virtual void    AddSource(Shape *source);
	// Adds a shape as an ending point for a connection starting in this shape.
	virtual void    AddTarget(Shape *target) {}

	///////////////////////////////////////////////////////////////
	// Factory.
	///////////////////////////////////////////////////////////////

	// The class name used in serialization.
	virtual wxString   GetClassName() const { return wxT("input"); }
	virtual wxXmlNode *Serialize() const;
	virtual void       Deserialize(const wxXmlNode& root);
	// Creates an instance of this shape.
	static Serialized *Create(const wxXmlNode& root) { return NEW(InputPort, ()); }

	///////////////////////////////////////////////////////////////
	// Lua object.
	///////////////////////////////////////////////////////////////

	virtual Metamethod GetLuaIndex() const { return LuaIndex; }

	///////////////////////////////////////////////////////////////
	// Port-specific stuff.
	///////////////////////////////////////////////////////////////

	virtual wxString GetType() const;
	virtual Status   GetStatus() const;
	// The source port connected to this port.
	OutputPort *     GetSource() const;
	bool             CheckType() const;

protected:
	// __index metamethod.
	static int LuaIndex(lua_State *L);
	// Returns the port's type.
	static int LuaGetSource(lua_State *L);
};

class OutputPort: public Port
{
public:

	OutputPort()
        : Port()
    {
    }

	virtual ~OutputPort() {}

	// Returns the connection point.
	virtual wxPoint GetConnPoint() const;

	// Adds a shape as a starting point for a connection starting in this shape.
	virtual void    AddSource(Shape *source) {}

	// The class name used in serialization.
	virtual wxString   GetClassName() const { return wxT("output"); }

	// Creates an instance of this shape.
	static Serialized *Create(const wxXmlNode& root) { return NEW(OutputPort, ()); }

	virtual wxString GetType() const;
	virtual Status GetStatus() const;
};