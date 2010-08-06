#pragma once

#include "Editor/Graph/Serialized.h"

class Shape;

class Member: public Serialized
{
public:
	// Possible property types.
	enum Type { Nil, Int, Bool, Double, String, Color, Rect };

	// Constructors
	Member(): m_owner(NULL), m_name(wxT("")), m_id(wxT("")), m_cdata(false), m_type(Nil) {}
	Member(Shape *owner, const wxString& name, int *value, const wxString& id = wxEmptyString);
	Member(Shape *owner, const wxString& name, bool *value, const wxString& id = wxEmptyString);
	Member(Shape *owner, const wxString& name, double *value, const wxString& id = wxEmptyString);
	Member(Shape *owner, const wxString& name, wxString *value, const wxString& id = wxEmptyString);
	Member(Shape *owner, const wxString& name, wxColor *value, const wxString& id = wxEmptyString);
	Member(Shape *owner, const wxString& name, wxRect *value, const wxString& id = wxEmptyString);
	// Destructor
	virtual ~Member() {}

	// Getters.
	Shape *  GetOwner() const  { return m_owner; }
	wxString GetName() const   { return m_name; }
	wxString GetId() const     { return m_id; }
	Type     GetType() const   { return m_type; }

	int      GetInt() const    { return m_type == Int    ? *m_int    : 0; }
	bool     GetBool() const   { return m_type == Bool   ? *m_bool   : false; }
	double   GetDouble() const { return m_type == Double ? *m_double : 0; }
	wxString GetString() const { return m_type == String ? *m_string : wxT(""); }
	wxColor  GetColor() const  { return m_type == Color  ? *m_color  : wxColor(); }
	wxRect   GetRect() const   { return m_type == Rect   ? *m_rect   : wxRect(); }

	virtual wxString GetValue() const;

	// Setters.
	void SetOwner(Shape *owner)          { m_owner = owner; }
	void SetName(const wxString& name)   { m_name = name; }
	void SetId(const wxString& id)       { m_id = id; }

	void SetPointer(int *value)          { m_int    = value; m_type = Int; }
	void SetPointer(bool *value)         { m_bool   = value; m_type = Bool; }
	void SetPointer(double *value)       { m_double = value; m_type = Double; }
	void SetPointer(wxString *value)     { m_string = value; m_type = String; }
	void SetPointer(wxColor *value)      { m_color  = value; m_type = Color; }
	void SetPointer(wxRect *value)       { m_rect   = value; m_type = Rect; }

	void SetValue(const int value);
	void SetValue(const bool value);
	void SetValue(const double value);
	void SetValue(const wxString& value);
	void SetValue(const wxColor& value);
	void SetValue(const wxRect& value);
	void SetValue(const wxVariant& value);

	// Called when the members's value has changed.
	virtual void OnChanged() { }

	void SetCData(bool cdata = true)     { m_cdata = cdata; }

	void     FromString(const wxString& value);
	wxString ToString() const;

	// Serialization.
	virtual wxString   GetClassName() const { return wxT("member"); }
	virtual wxXmlNode *Serialize() const;
	virtual void       Deserialize(const wxXmlNode& root);

protected:
	// Make an id out of a name.
	void BuildId();
	// Redraws the owner and calls OnChanged method.
	void Updated();
	// Owner of the member.
	Shape *m_owner;
	// Name and id of the member.
	wxString m_name, m_id;
	// Use CDATA?
	bool m_cdata;
	// Type of the member.
	Type m_type;
	// Pointer to the member.
	union
	{
		int *m_int;
		bool *m_bool;
		double *m_double;
		wxString *m_string;
		wxColor *m_color;
		wxRect *m_rect;
	};
};

