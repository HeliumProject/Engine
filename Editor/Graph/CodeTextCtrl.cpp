#include "Precompile.h"
#include "Graph/CodeTextCtrl.h"

#include "Graph/Debug.h"

CodeTextCtrl::CodeTextCtrl(wxWindow *parent, wxWindowID id)
	: wxTextCtrl(parent, id, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH, wxDefaultValidator, wxTextCtrlNameStr)
{
	wxTextAttr attr;
	attr.SetFont(wxFont(8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	SetDefaultStyle(attr);
}

CodeTextCtrl&
CodeTextCtrl::Append(const wxString& code)
{
	AppendText(code);
	return *this;
}
