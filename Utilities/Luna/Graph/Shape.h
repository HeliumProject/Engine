#pragma once

#include "Graph/Serialized.h"
#include "Graph/Member.h"
#include "Graph/Property.h"
#include "Graph/PropertyGrid.h"
#include "Graph/Utilities.h"
#include "Graph/Debug.h"

///////////////////////////////////////////////////////////////
// The basic shape class.
///////////////////////////////////////////////////////////////

class Shape: public Serialized
{
public:
	///////////////////////////////////////////////////////////////
	// Construction and destruction.
	///////////////////////////////////////////////////////////////

	// "Regular" constructor.
	Shape();
	// Virtual destructor.
	virtual ~Shape();

	///////////////////////////////////////////////////////////////
	// Getters and setters.
	///////////////////////////////////////////////////////////////

	Shape *GetParent() const        { return m_parent; }
	void   SetParent(Shape *parent) { m_parent = parent; }

	///////////////////////////////////////////////////////////////
	// Position and size.
	///////////////////////////////////////////////////////////////

	// Returns the shape's bounding box.
	wxRect         GetBBox() const                { return m_bbox; }
	// Returns the Shape which contains the given point.
	virtual Shape *IsInside(const wxPoint& pt);
	Shape *        IsInside(int x, int y)         { return IsInside(wxPoint(x, y)); }
	bool           IsInside(const wxRect& rect)   { return rect.Contains(m_bbox); }
	// Returns the shape's top-left corner.
	wxPoint        GetPosition() const            { return m_bbox.GetPosition(); }
	// Returns the shape's size.
	wxSize         GetSize() const                { return m_bbox.GetSize(); }
	// Sets the shape's top-left corner.
	virtual void   SetPosition(int x, int y);
	void           SetPosition(const wxPoint& pt) { SetPosition(pt.x, pt.y); }
	// Sets the shape's size.
	virtual void   SetSize(const wxSize& size)    { m_bbox.SetSize(size); }
	void           SetSize(int width, int height) { SetSize(wxSize(width, height)); }

	///////////////////////////////////////////////////////////////
	// Drawing.
	///////////////////////////////////////////////////////////////

	// Draws the shape.
	virtual void Draw(wxDC& dc) const;
	// Draws the connections.
	virtual void DrawConnections(wxDC& dc) const;
	// Requests the shape to be redrawn.
	virtual void Redraw();

	///////////////////////////////////////////////////////////////
	// Connection to other shapes.
	///////////////////////////////////////////////////////////////

	// Returns the connection point.
	virtual wxPoint         GetConnPoint() const        { return wxPoint(); }
	// Adds a shape as a starting point for a connection starting in this shape.
	virtual void            AddSource(Shape *source)    { m_sources.Add(source); }
	// Deletes a source.
	void                    DeleteSource(Shape *source) { m_sources.Delete(source); }
	// The number of shapes connected to this shape.
	size_t                  NumSources() const          { return m_sources.Size(); }
	// Returns an ordered iterator for the sources.
	List<Shape *>::Iterator SourceIterator() const      { return m_sources.Iterate(); }
	// Adds a shape as an ending point for a connection starting in this shape.
	virtual void            AddTarget(Shape *target)    { m_targets.Add(target); }
	// Deletes a target.
	void                    DeleteTarget(Shape *target) { m_targets.Delete(target); }
	// The number of shapes connected from this shape.
	size_t                  NumTargets() const          { return m_targets.Size(); }
	// Returns an ordered iterator for the sources.
	List<Shape *>::Iterator TargetIterator() const      { return m_targets.Iterate(); }

	///////////////////////////////////////////////////////////////
	// Grouping.
	///////////////////////////////////////////////////////////////

	// Adds a shape as a child of this shape.
	virtual void            AddChild(Shape *child);
	// Deletes the shape from the list of children.
	void                    DeleteChild(Shape *child) { m_children.Delete(child); }
	// Returns the number of children.
	size_t                  NumChildren() const       { return m_children.Size(); }
	// Returns an ordered iterator for the children.
	List<Shape *>::Iterator ChildIterator() const     { return m_children.Iterate(); }
	// Returns the bounding box enclosing all children.
	wxRect                  GetChildrenBBox() const;

	///////////////////////////////////////////////////////////////
	// Members and properties.
	///////////////////////////////////////////////////////////////

	// Adds a member to this shape.
	Member *                   AddMember(Member *member);
	// Deletes a member from this shape.
	void                       DeleteMember(Member *member);
	// The number of properties.
	size_t                     NumMembers() const       { return m_members.Size(); }
	// Finds a member by id, returns NULL if not found.
	Member *                   FindMember(const wxString& id) const;
	// Returns a member by id.
	Member *                   GetMember(const wxString& id) const;
	// Returns an ordered iterator for the members.
	List<Member *>::Iterator   MemberIterator() const   { return m_members.Iterate(); }
	// MemberChanged is called whenever a member has it's value changed.
	Property *                 AddProperty(Property *prop);
	// The number of properties.
	size_t                     NumProperties() const    { return m_props.Size(); }
	// Returns an ordered iterator for the properties.
	List<Property *>::Iterator PropertyIterator() const { return m_props.Iterate(); }
	// Called when a member has been changed.
	virtual void               MemberChanged(Member *member);
	// Adds all properties to the property grid.
	void                       ShowProperties(PropertyGrid *grid);

	///////////////////////////////////////////////////////////////
	// Persistance.
	///////////////////////////////////////////////////////////////

	// Serialize the shape's properties to a wxXmlNode.
	virtual wxXmlNode *Serialize() const;
	// Deserialize the shape from a wxXmlNode.
	virtual void       Deserialize(const wxXmlNode& root);
	// The class name used in serialization.
	virtual wxString   GetClassName() const          { return wxT("shape"); }
	// Creates an instance of this shape.
	static Serialized *Create(const wxXmlNode& root) { return NEW(Shape, ()); }

protected:
	// (De)serialization helpers.
	virtual wxXmlNode *SerializeChildren() const;
	virtual wxXmlNode *SerializeMembers() const;
	virtual wxXmlNode *SerializeConnections() const;
	virtual void       DeserializeChildren(const wxXmlNode& root);
	virtual void       DeserializeMembers(const wxXmlNode& root);
	virtual void       DeserializeConnections(const wxXmlNode& root);
	// Re-evaluates the bounding box.
	void ReevaluateBBox();
	// The parent of the shape.
	Shape *m_parent;
	// Basic properties.
	wxRect m_bbox;
	// Children of the shape.
	List<Shape *> m_children;
	// Connections.
	List<Shape *> m_sources, m_targets;
	// Members and properties.
	List<Member *> m_members;
	List<Property *> m_props;
	std::map<wxString, Member *> m_members_by_id;
};

