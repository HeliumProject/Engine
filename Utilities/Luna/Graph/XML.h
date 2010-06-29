#pragma once

namespace XML
{
	// De-serialization helper methods.
	wxXmlNode *    FindChild(const wxXmlNode& node, const wxString& name);
	void           DeleteChild(wxXmlNode& node, const wxString& name);
	void           DeleteChild(wxXmlNode& node, wxXmlNodeType type);
	void           DeleteAttribute(wxXmlNode& node, const wxString& name);
	wxXmlProperty *FindAttribute(const wxXmlNode& node, const wxString& name);
	wxString       GetStringAttribute(const wxXmlNode& node, const wxString& name, const wxString& def = wxEmptyString);
	bool           GetBoolAttribute(const wxXmlNode& node, const wxString& name, const wxString& def = wxEmptyString);
	int            GetIntAttribute(const wxXmlNode& node, const wxString& name, const wxString& def = wxEmptyString);
};
