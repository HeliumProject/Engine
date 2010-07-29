#pragma once

class CodeTextCtrl: public wxTextCtrl
{
	public:
		CodeTextCtrl(wxWindow *parent, wxWindowID id);

		CodeTextCtrl& Append(const wxString& code);
};

