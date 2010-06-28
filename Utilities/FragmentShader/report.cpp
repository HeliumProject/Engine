#include "report.h"

#include <wx/imaglist.h>
#include <wx/bitmap.h>

#include "debug.h"

#include "xpm/agt_action_success.xpm"
#include "xpm/agt_update_critical.xpm"
#include "xpm/agt_stop.xpm"

BEGIN_EVENT_TABLE(Report, wxListCtrl)
	EVT_SIZE(Report::OnSize)
END_EVENT_TABLE()

Report::Report(wxWindow *parent, wxWindowID id)
	: wxListCtrl(parent, id, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_HRULES | wxLC_VRULES)
	, m_NumErrors(0)
	, m_NumWarnings(0)
	, m_NumInfos(0)
	, m_Width(0)
{
	wxImageList *images = NEW(wxImageList, (16, 16, true, 3));
	images->Add(wxBitmap(agt_action_success_xpm));
	images->Add(wxBitmap(agt_update_critical_xpm));
	images->Add(wxBitmap(agt_stop_xpm));
	AssignImageList(images, wxIMAGE_LIST_SMALL);
	wxListItem col;
	col.SetAlign(wxLIST_FORMAT_LEFT);
	col.SetColumn(0);
	col.SetText(wxT("Messages"));
	col.SetWidth(100);
	InsertColumn(0, col);
}

void
Report::Clear()
{
	wxListCtrl::DeleteAllItems();
	m_NumErrors = m_NumWarnings = m_NumInfos = m_Width = 0;
}

void
Report::Info(const wxString& msg, void *data)
{
	Append(msg, 0, data);
	m_NumInfos++;
}

void
Report::Warning(const wxString& msg, void *data)
{
	Append(msg, 1, data);
	m_NumWarnings++;
}

void
Report::Error(const wxString& msg, void *data)
{
	Append(msg, 2, data);
	m_NumErrors++;
}

int
Report::NumErrors() const
{
	return m_NumErrors;
}

int
Report::NumWarnings() const
{
	return m_NumWarnings;
}

int
Report::NumInfos() const
{
	return m_NumInfos;
}

void
Report::OnSize(wxSizeEvent& evt)
{
	wxSize size = GetClientSize();
	if (size.GetWidth() > m_Width)
	{
		SetColumnWidth(0, size.GetWidth());
	}
	else
	{
		SetColumnWidth(0, m_Width);
	}
}

void
Report::Append(const wxString& msg, int image, void *data)
{
	wxListItem item;
	item.SetText(msg);
	item.SetImage(image);
	item.SetData(data);
	long index = InsertItem(item);
	wxRect rect;
	GetItemRect(index, rect, wxLIST_RECT_LABEL);
	if (rect.GetWidth() > m_Width)
	{
		m_Width = rect.GetWidth();
	}
}
