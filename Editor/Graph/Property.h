#pragma once

#include "Editor/Graph/PropertyGrid.h"
#include "Editor/Graph/Member.h"

#include "Platform/Types.h"

class Method;

class Property: public Member
{
public:
	Property(Shape *owner, const wxString& name, int *value, const wxString& id = wxEmptyString)
		: Member(owner, name, value, id), m_validator(NULL), m_onchanged(NULL) {}
	Property(Shape *owner, const wxString& name, bool *value, const wxString& id = wxEmptyString)
		: Member(owner, name, value, id), m_validator(NULL), m_onchanged(NULL) {}
	Property(Shape *owner, const wxString& name, double *value, const wxString& id = wxEmptyString)
		: Member(owner, name, value, id), m_validator(NULL), m_onchanged(NULL) {}
	Property(Shape *owner, const wxString& name, wxString *value, const wxString& id = wxEmptyString)
		: Member(owner, name, value, id), m_validator(NULL), m_onchanged(NULL) {}
	Property(Shape *owner, const wxString& name, wxColor *value, const wxString& id = wxEmptyString)
		: Member(owner, name, value, id), m_validator(NULL), m_onchanged(NULL) {}
	Property(Shape *owner, const wxString& name, wxRect *value, const wxString& id = wxEmptyString)
		: Member(owner, name, value, id), m_validator(NULL), m_onchanged(NULL) {}

	virtual ~Property();

	// Sets the validator for this property.
	void    SetValidator(Method *validator) { m_validator = validator; }
	// Gets the validator of this property.
	Method *GetValidator() const            { return m_validator; }
	// Sets the method that will be called when the property's value changes.
	void SetOnChanged(Method *onchanged) { m_onchanged = onchanged; }

	virtual void OnChanged();

	// Add this property to a property editor.
	virtual void Show(PropertyGrid *props) = 0;

	// Serialization.
	virtual wxString   GetClassName() const { return wxT("property"); }
	virtual wxXmlNode *Serialize() const;
	virtual void       Deserialize(const wxXmlNode& root);

protected:
	Method *m_validator;
	Method *m_onchanged;
};

class ListProperty: public Property
{
public:
	struct Item { wxString m_Label, m_Value; };

	ListProperty(): Property(NULL, wxT(""), &m_int) {}
	ListProperty(Shape *owner, const wxString& name, std::vector<Item> *items, const wxString& id = wxEmptyString);

	virtual void Show(PropertyGrid *props);
	virtual wxString GetValue() const { return m_values.GetLabel(m_int); }

	virtual wxString   GetClassName() const { return wxT("list"); }
	virtual wxXmlNode *Serialize() const;
	virtual void       Deserialize(const wxXmlNode& root);

private:
	int m_int;
	wxPGChoices m_labels;
	wxPGChoices m_values;
};

class BoolProperty: public Property
{
public:
	BoolProperty(): Property(NULL, wxT(""), &m_bool) {}
	BoolProperty(Shape *owner, const wxString& name, const wxString& id = wxEmptyString)
		: Property(owner, name, &m_bool, id) {}

	virtual void Show(PropertyGrid *props);

	virtual wxString GetClassName() const { return wxT("bool"); }

private:
	bool m_bool;
};

class TextProperty: public Property
{
public:
	TextProperty(): Property(NULL, wxT(""), &m_string) {}
	TextProperty(Shape *owner, const wxString& name, const wxString& id = wxEmptyString)
		: Property(owner, name, &m_string, id) {}
	
	virtual void Show(PropertyGrid *props) { props->Append(NEW(wxStringProperty, (m_name, wxPG_LABEL, m_string)), this); }

	virtual wxString GetClassName() const { return wxT("text"); }

private:
	wxString m_string;
};

class MemoProperty: public Property
{
public:
	MemoProperty(): Property(NULL, wxT(""), &m_string) {}
	MemoProperty(Shape *owner, const wxString& name, const wxString& id = wxEmptyString)
		: Property(owner, name, &m_string, id) { SetCData(true); }

	virtual void Show(PropertyGrid *props) { props->Append(NEW(wxLongStringProperty, (m_name, wxPG_LABEL, m_string)), this); }

	virtual wxString GetClassName() const { return wxT("memo"); }

private:
	wxString m_string;
};

class FileProperty: public Property
{
public:
	FileProperty(): Property(NULL, wxT(""), &m_string) {}
	FileProperty(Shape *owner, const wxString& name, const wxString& wildcard, const wxString& id = wxEmptyString)
		: Property(owner, name, &m_string, id), m_wildcard(wildcard) {}

	virtual void Show(PropertyGrid *props);

	virtual wxString GetClassName() const { return wxT("file"); }

private:
	wxString m_string, m_wildcard;
};

class ColorProperty: public Property
{
public:
	ColorProperty(): Property(NULL, wxT(""), &m_color) {}
	ColorProperty(Shape *owner, const wxString& name, const wxString& id = wxEmptyString)
		: Property(owner, name, &m_color, id) {}

	virtual void Show(PropertyGrid *props) { props->Append(NEW(wxColourProperty, (m_name, wxPG_LABEL, m_color)), this); }

	virtual wxString GetClassName() const { return wxT("color"); }

private:
	wxColor m_color;
};

Property *
DeserializeProperty(const wxXmlNode& root);

