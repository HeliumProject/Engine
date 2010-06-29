#ifndef __REPORT_H__
#define __REPORT_H__

#include <wx/listctrl.h>

class Report: public wxListCtrl
{
	public:
		Report(wxWindow *parent, wxWindowID id);

		void Clear();
		void Info(const wxString& msg, void *data = 0);
		void Warning(const wxString& msg, void *data = 0);
		void Error(const wxString& msg, void *data = 0);

		int NumErrors() const;
		int NumWarnings() const;
		int NumInfos() const;

		void OnSize(wxSizeEvent& evt);

	private:
		void Append(const wxString& msg, int image, void *data);
		int m_NumErrors, m_NumWarnings, m_NumInfos, m_Width;

	DECLARE_NO_COPY_CLASS(Report)
	DECLARE_EVENT_TABLE()
};

#endif // __REPORT_H__
