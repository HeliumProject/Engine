#ifndef __XML_H__
#define __XML_H__

#include <wx/wx.h>
#include <wx/xml/xml.h>

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

#endif // __XML_H__
