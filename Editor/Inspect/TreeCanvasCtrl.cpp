#include "TreeCanvasCtrl.h"

#include "Application/Inspect/Controls/InspectContainer.h"

using namespace Helium;
using namespace Helium::Inspect;

const static int SCROLL_INCREMENT = 8;

class PanelItemData : public wxTreeItemData
{
public:
    PanelItemData( Panel* panel )
        : wxTreeItemData()
        , m_Panel( panel )
        , m_IgnoreToggle( false )
    {
    }

    Panel* GetPanel() { return m_Panel; }

    void StartIgnoreToggle() { m_IgnoreToggle = true; }
    void EndIgnoreToggle() { m_IgnoreToggle = false; }
    bool GetIgnoreToggle() { return m_IgnoreToggle; }

protected:
    Panel* m_Panel;
    bool m_IgnoreToggle;
};

BEGIN_EVENT_TABLE( TreeCanvasCtrl, Helium::TreeWndCtrl )
EVT_SHOW( TreeCanvasCtrl::OnShow )
EVT_SIZE( TreeCanvasCtrl::OnSize )
EVT_LEFT_DOWN( TreeCanvasCtrl::OnClick )
EVT_TREE_ITEM_EXPANDED( wxID_ANY, TreeCanvasCtrl::OnToggle )
EVT_TREE_ITEM_COLLAPSED( wxID_ANY, TreeCanvasCtrl::OnToggle )
END_EVENT_TABLE();

TreeCanvasCtrl::TreeCanvasCtrl(wxWindow *parent,
                               wxWindowID winid,
                               const wxPoint& pos,
                               const wxSize& size,
                               long style,
                               const wxString& name,
                               int treeStyle,
                               unsigned int columnSize,
                               wxBitmap expandedBitmap,
                               wxBitmap collapsedBitmap,
                               wxPen pen,
                               unsigned int clickTolerance)
                               : Helium::TreeWndCtrl(parent, winid, pos, size, wxALWAYS_SHOW_SB | style, name, treeStyle, columnSize, expandedBitmap, collapsedBitmap, pen, clickTolerance)
                               , m_Canvas(NULL)
{
    SetScrollRate(SCROLL_INCREMENT, SCROLL_INCREMENT);

    //wxALWAYS_SHOW_SB in the constructor will ensure that there is always scroll bars
    //in the canvas window.  This will prevent the need from doing layout on the window
    //more than once.
    //This function will ensure that the horizontal bar are never seen.  If a horizontal
    //bar might be necessary, then the best way would be to remove this and just let both
    //bars be seen. 
    ::ShowScrollBar( (HWND) GetHandle(), SB_HORZ, FALSE );
}

Canvas* TreeCanvasCtrl::GetCanvas()
{
    return m_Canvas;
}

void TreeCanvasCtrl::SetCanvas(Canvas* canvas)
{
    m_Canvas = canvas;
}

void TreeCanvasCtrl::OnShow(wxShowEvent& event)
{
    if (m_Canvas)
    {
        m_Canvas->RaiseShow( event.GetShow() );
    }
}

void TreeCanvasCtrl::OnSize(wxSizeEvent& event)
{
    // to prevent the scroll bar from appearing during an OnSize event, need to 
    // explicitly hide the scroll bar
    ::ShowScrollBar( (HWND) GetHandle(), SB_HORZ, FALSE );

    // to make sure the tree control fits to the width of the canvas, set the
    // virtual size
    int canvasPad = wxSystemSettings::GetMetric( wxSYS_VSCROLL_X ) + ( m_Canvas ? m_Canvas->GetPad() : 0 );
    int w = -1;

    int eventWidth = event.GetSize().GetWidth();
    if ( eventWidth > canvasPad )
    {
        w = eventWidth - canvasPad;
    }
    else
    {
        int virtualWidth = GetVirtualSize().GetWidth();
        if ( virtualWidth > canvasPad )
        {
            w = virtualWidth - canvasPad;
        }
    }

    SetVirtualSizeHints( w, -1, w, -1 );

    Layout();

    event.Skip();
}

void TreeCanvasCtrl::OnClick(wxMouseEvent& event)
{
    SetFocus();

    event.Skip();
}

void TreeCanvasCtrl::OnToggle(wxTreeEvent& event)
{
    wxTreeItemId item = event.GetItem();

    PanelItemData* panelItemData = (PanelItemData*) GetItemData( item );
    if ( !panelItemData->GetIgnoreToggle() )
    {
        Panel* panel = panelItemData->GetPanel();
        if ( panel )
        {
            bool newExpandState = !panel->IsExpanded();

            Canvas* canvas = panel->GetCanvas();
            if ( canvas )
            {
                canvas->SetPanelExpandState( panel->GetPath(), newExpandState ? ExpandStates::Expanded : ExpandStates::Collapsed );
            }

            panel->SetExpanded( newExpandState );
        }
    }

    event.Skip();
}
