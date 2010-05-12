#include "code.h"

#include "debug.h"

Code::Code(wxWindow *parent, wxWindowID id)
	: wxTextCtrl(parent, id, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH, wxDefaultValidator, wxTextCtrlNameStr)
{
	wxTextAttr attr;
	attr.SetFont(wxFont(8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	SetDefaultStyle(attr);
}

Code&
Code::Append(const wxString& code)
{
	AppendText(code);
	return *this;
}
