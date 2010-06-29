#ifndef __NODE_H__
#define __NODE_H__

#include <wx/wx.h>
#include <wx/xml/xml.h>

#include "shaderobj.h"
#include "property.h"
#include "code.h"
#include "report.h"
#include "luautil.h"

#include <map>

#include "debug.h"

class Node;

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
	static Persistent *Create(const wxXmlNode& root) { return (Port *)NULL; }

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
	static Persistent *Create(const wxXmlNode& root) { return NEW(InputPort, ()); }

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
	///////////////////////////////////////////////////////////////
	// Construction and destruction.
	///////////////////////////////////////////////////////////////

	OutputPort(): Port() {}
	virtual ~OutputPort() {}

	///////////////////////////////////////////////////////////////
	// Connection to other shapes.
	///////////////////////////////////////////////////////////////

	// Returns the connection point.
	virtual wxPoint GetConnPoint() const;
	// Adds a shape as a starting point for a connection starting in this shape.
	virtual void    AddSource(Shape *source) {}

	///////////////////////////////////////////////////////////////
	// Factory.
	///////////////////////////////////////////////////////////////

	// The class name used in serialization.
	virtual wxString   GetClassName() const { return wxT("output"); }
	// Creates an instance of this shape.
	static Persistent *Create(const wxXmlNode& root) { return NEW(OutputPort, ()); }

	///////////////////////////////////////////////////////////////
	// Port-specific stuff.
	///////////////////////////////////////////////////////////////

	virtual wxString GetType() const;
	virtual Status GetStatus() const;
};

class Node: public ShaderObject
{
public:
	///////////////////////////////////////////////////////////////
	// Construction and destruction.
	///////////////////////////////////////////////////////////////

	Node();
	//Node(const Node& from);
	virtual ~Node() {}

	///////////////////////////////////////////////////////////////
	// Position and size.
	///////////////////////////////////////////////////////////////

	virtual void EvalSize();

	///////////////////////////////////////////////////////////////
	// Drawing.
	///////////////////////////////////////////////////////////////

	virtual void Draw(wxDC& dc) const;
	virtual void Redraw() { EvalSize(); Shape::Redraw(); }

	///////////////////////////////////////////////////////////////
	// Connection to other shapes.
	///////////////////////////////////////////////////////////////

	// Adds a shape as a starting point for a connection starting in this shape.
	virtual void AddSource(Shape *source) {}
	// Adds a shape as an ending point for a connection starting in this shape.
	virtual void AddTarget(Shape *target) {}

	///////////////////////////////////////////////////////////////
	// Grouping.
	///////////////////////////////////////////////////////////////

	// Adds a shape as a child of this shape.
	virtual void AddChild(Shape *child) {}

	///////////////////////////////////////////////////////////////
	// Factory.
	///////////////////////////////////////////////////////////////

	// The class name used in serialization.
	virtual wxString  GetClassName() const { return wxT("node"); }
	// Creates an instance of this shape.
	static Persistent *Create(const wxXmlNode& root) { return NEW(Node, ()); }

	///////////////////////////////////////////////////////////////
	// Lua object.
	///////////////////////////////////////////////////////////////

	virtual wxString   GetLuaId() const { return m_id; }
	virtual Metamethod GetLuaIndex() const { return LuaIndex; }

	///////////////////////////////////////////////////////////////
	// Node-specific stuff.
	///////////////////////////////////////////////////////////////

  virtual void           Deserialize(const wxXmlNode& root);

	// Input ports methods.
	void                   AddInput(InputPort *input) { m_inputs.Add(input); Shape::AddChild(input); EvalSize(); }
	size_t                 NumInputs() const          { return m_inputs.Size(); }
	List<InputPort *>::Iterator InputIterator() const  { return m_inputs.Iterate(); }

	// Output ports methods.
	void                   AddOutput(OutputPort *output) { m_outputs.Add(output); Shape::AddChild(output); EvalSize(); }
	size_t                 NumOutputs() const            { return m_outputs.Size(); }
	List<OutputPort *>::Iterator OutputIterator() const   { return m_outputs.Iterate(); }

	enum    Status { Ok, InconsistentInputs };
	virtual Status GetStatus() const;
	virtual void   GenerateCode(Code *codectrl, Report *reportctrl) const;
	virtual bool   CheckInputConstraints() const;

	// Shape-wide metrics.
	static int          GetLineHeight();
	static int          GetSeparatorLength();
	static const wxFont GetFont();

protected:
	virtual void DeserializeChildren(const wxXmlNode& root);
	// __index metamethod.
	static int LuaIndex(lua_State *L);
	// __index metamethod for members.
	static int MemberLuaIndex(lua_State *L);
	// Returns the value of a member.
	static int MemberLuaGetValue(lua_State *L);
	// Sets the value of a member.
	static int MemberLuaSetValue(lua_State *L);
	// Ports.
	List<InputPort *> m_inputs;
	List<OutputPort *> m_outputs;
	// Properties.
	wxString m_path, m_type, m_id, m_description, m_tool_tip;
	bool m_deletable;
	wxString m_name;
};

#endif /* __NODE_H__ */
