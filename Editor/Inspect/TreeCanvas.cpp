#include "Precompile.h"
#include "TreeCanvas.h"

#include "Editor/FileIconsTable.h"
#include "Editor/Inspect/TreeCanvasWidget.h"

using namespace Helium;
using namespace Helium::Editor;

REFLECT_DEFINE_CLASS( TreeCanvas );

const static int SCROLL_INCREMENT = 8;

TreeCanvas::TreeCanvas()
: m_TreeWndCtrl( NULL )
, m_RootId( Helium::TreeWndCtrlItemIdInvalid )
{
    SetWidgetCreator< TreeCanvasWidget, Container >();
}

TreeCanvas::~TreeCanvas()
{

}

void TreeCanvas::SetTreeWndCtrl( TreeWndCtrl* treeWndCtrl )
{
    SetWindow( treeWndCtrl );

    if ( m_TreeWndCtrl )
    {
        m_TreeWndCtrl->Disconnect( m_TreeWndCtrl->GetId(), wxEVT_SIZE, wxSizeEventHandler( TreeCanvas::OnSize ), NULL, this );
        m_TreeWndCtrl->Disconnect( m_TreeWndCtrl->GetId(), wxEVT_COMMAND_TREE_ITEM_EXPANDED, wxTreeEventHandler( TreeCanvas::OnToggle ), NULL, this );
        m_TreeWndCtrl->Disconnect( m_TreeWndCtrl->GetId(), wxEVT_COMMAND_TREE_ITEM_COLLAPSED, wxTreeEventHandler( TreeCanvas::OnToggle ), NULL, this );
    }

    m_TreeWndCtrl = treeWndCtrl;

    if ( m_TreeWndCtrl )
    {
        // for now we must always show scroll bars
        HELIUM_ASSERT( m_TreeWndCtrl->GetWindowStyle() & wxALWAYS_SHOW_SB );

        m_TreeWndCtrl->SetScrollRate(SCROLL_INCREMENT, SCROLL_INCREMENT);
        m_TreeWndCtrl->SetImageList( GlobalFileIconsTable().GetSmallImageList() );
        m_TreeWndCtrl->SetStateImageList( GlobalFileIconsTable().GetSmallImageList() );

        wxTreeItemId root = m_TreeWndCtrl->GetRootItem();
        if ( root == Helium::TreeWndCtrlItemIdInvalid )
        {
            root = m_TreeWndCtrl->AddRoot( TXT( "INVISIBLE_ROOT" ) );
            m_RootId = root;
        }

        // wxALWAYS_SHOW_SB in the constructor will ensure that there is always scroll bars
        //  in the canvas window.  This will prevent the need from doing layout on the window more than once.
        //  This function will ensure that the horizontal bar are never seen.  If a horizontal
        //  bar might be necessary, then the best way would be to remove this and just let both
        //  bars be seen. 
#pragma TODO("Figure out how to properly handle this edge case only calling wx code")
        ::ShowScrollBar( (HWND) m_TreeWndCtrl->GetHandle(), SB_HORZ, FALSE );

        m_TreeWndCtrl->Connect( m_TreeWndCtrl->GetId(), wxEVT_SIZE, wxSizeEventHandler( TreeCanvas::OnSize ), NULL, this );
        m_TreeWndCtrl->Connect( m_TreeWndCtrl->GetId(), wxEVT_COMMAND_TREE_ITEM_EXPANDED, wxTreeEventHandler( TreeCanvas::OnToggle ), NULL, this );
        m_TreeWndCtrl->Connect( m_TreeWndCtrl->GetId(), wxEVT_COMMAND_TREE_ITEM_COLLAPSED, wxTreeEventHandler( TreeCanvas::OnToggle ), NULL, this );
    }
}

void TreeCanvas::Realize( Inspect::Canvas* canvas )
{
    HELIUM_ASSERT( canvas == this || canvas == NULL );

    SmartPtr< TreeCanvasWidget > widget = new TreeCanvasWidget( this );
    widget->SetTreeWndCtrl( m_TreeWndCtrl );
    widget->SetId( m_RootId );
    SetWidget( widget );

    Inspect::V_Control::const_iterator itr = m_Children.begin();
    Inspect::V_Control::const_iterator end = m_Children.end();
    for( ; itr != end; ++itr )
    {
        Inspect::Control* c = *itr;
        c->Realize( this );
    }

    m_TreeWndCtrl->Freeze();
    {
        m_TreeWndCtrl->Scroll( 0, 0 );
        m_TreeWndCtrl->Layout();
        Populate();
        Read();
    }    
    m_TreeWndCtrl->Thaw();
}

void TreeCanvas::Clear()
{
    Base::Clear();

    m_TreeWndCtrl->DeleteChildren( m_TreeWndCtrl->GetRootItem() );
}

void TreeCanvas::OnSize(wxSizeEvent& event)
{
    // to prevent the scroll bar from appearing during an OnSize event, need to 
    // explicitly hide the scroll bar
    ::ShowScrollBar( (HWND) m_TreeWndCtrl->GetHandle(), SB_HORZ, FALSE );

    // to make sure the tree control fits to the width of the canvas, set the
    // virtual size
    int canvasPad = wxSystemSettings::GetMetric( wxSYS_VSCROLL_X ) + GetPad();
    int w = -1;

    int eventWidth = event.GetSize().GetWidth();
    if ( eventWidth > canvasPad )
    {
        w = eventWidth - canvasPad;
    }
    else
    {
        int virtualWidth = m_TreeWndCtrl->GetVirtualSize().GetWidth();
        if ( virtualWidth > canvasPad )
        {
            w = virtualWidth - canvasPad;
        }
    }

    m_TreeWndCtrl->SetVirtualSizeHints( w, -1, w, -1 );
    m_TreeWndCtrl->Layout();

    event.Skip();
}

void TreeCanvas::OnToggle(wxTreeEvent& event)
{
    wxTreeItemId item = event.GetItem();

    if ( item.IsOk() )
    {
        TreeItemData* data = static_cast< TreeItemData* >( m_TreeWndCtrl->GetItemData( item ) );
        Inspect::Container* container = Reflect::AssertCast< Inspect::Container >( data->GetWidget() );
        const tstring& path = container->GetPath();
        if ( !path.empty() )
        {
            if ( m_TreeWndCtrl->IsExpanded( item ) )
            {
                m_Collapsed.erase( path );
            }
            else
            {
                m_Collapsed.insert( path );
            }
        }
    }
}