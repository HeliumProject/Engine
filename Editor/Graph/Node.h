#pragma once

#include "Graph/ShaderObject.h"
#include "Graph/Property.h"
#include "Graph/CodeTextCtrl.h"
#include "Graph/ReportListCtrl.h"
#include "Graph/LuaUtilities.h"
#include "Graph/Port.h"
#include "Graph/Debug.h"

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
	static Serialized *Create(const wxXmlNode& root) { return NEW(Node, ()); }

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
	virtual void   GenerateCode(CodeTextCtrl *codectrl, Report *reportctrl) const;
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

