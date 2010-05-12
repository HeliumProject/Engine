#ifndef __CODE_H__
#define __CODE_H__

#include <wx/wx.h>

class Code: public wxTextCtrl
{
	public:
		Code(wxWindow *parent, wxWindowID id);

		Code& Append(const wxString& code);
};

#endif // __CODE_H__
