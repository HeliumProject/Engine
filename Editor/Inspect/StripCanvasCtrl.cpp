#include "TreeCanvasCtrl.h"

using namespace Helium;
using namespace Helium::Inspect;

BEGIN_EVENT_TABLE( StripCanvasCtrl, wxPanel )
EVT_SHOW( StripCanvasCtrl::OnShow )
EVT_SIZE( StripCanvasCtrl::OnSize )
EVT_LEFT_DOWN( StripCanvasCtrl::OnClick )
END_EVENT_TABLE();

StripCanvasCtrl::StripCanvasCtrl(wxWindow *parent, wxWindowID winid, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: wxPanel( parent, winid, pos, size, style, name )
, m_Canvas( NULL )
{
}

void StripCanvasCtrl::OnShow(wxShowEvent& event)
{
    if (m_Canvas)
    {
        m_Canvas->RaiseShow( event.GetShow() );
    }
}

void StripCanvasCtrl::OnSize(wxSizeEvent& event)
{
    Layout();

    event.Skip();
}

void StripCanvasCtrl::OnClick(wxMouseEvent& event)
{
    SetFocus();

    event.Skip();
}
