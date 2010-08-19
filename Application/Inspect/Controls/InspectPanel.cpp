#include "Application/Inspect/Controls/InspectInspectPanel.h"
#include "Application/Inspect/Controls/InspectCanvas.h"
#include "Application/Inspect/Controls/InspectLabel.h"

using namespace Helium::Reflect;
using namespace Helium::Inspect;

Panel::Panel()
: m_ItemData( this )
, m_Expanded( false)
, m_Expandable( true )
, m_Collapsable( true )
, m_Selected( false )
, m_Selectable( true )
, m_ShowTreeNode( true )
, m_RefreshControls( true )
, m_OwnWindow( false )
{

}

Panel::~Panel()
{
    Clear();
}

void Panel::SetName(const tstring& text)
{
    m_Name = text;
    m_Path.clear();

    if ( m_Canvas )
    {
        wxTreeItemId item = m_ItemData.GetId();
        if ( item != Helium::TreeWndCtrlItemIdInvalid )
        {
            Helium::TreeWndCtrl* treeWndCtrl = (Helium::TreeWndCtrl*) m_Window;
            treeWndCtrl->SetItemText( item, m_Name );
        }
    }
}

bool Panel::Process(const tstring& key, const tstring& value)
{
    if ( __super::Process( key, value ) )
        return true;

    if ( key == PANEL_ATTR_NAME )
    {
        SetName( m_Name = value );
        return true;
    }

    return false;
}

void Panel::Realize(Container* parent)
{
    if ( IsRealized() )
    {
        for ( V_Control::iterator itr = m_Controls.begin(), end = m_Controls.end(); itr != end; ++itr )
        {
            (*itr)->Realize( this );
        }
        return;
    }

    INSPECT_SCOPE_TIMER( ("") );

    ExpandState cachedState = m_Canvas->GetPanelExpandState( GetPath() );
    switch ( cachedState )
    {
        // The state was cached to be expanded
    case ExpandStates::Expanded:
        m_Expanded = true;
        break;

        // The state was cached to be collapsed
    case ExpandStates::Collapsed:
        m_Expanded = false;
        break;
    }

    m_Parent = parent;

    Helium::TreeWndCtrl* treeWndCtrl = NULL;
    if ( Reflect::ObjectCast<Panel>( m_Parent ) || Reflect::ObjectCast<Canvas>( m_Parent ) )
    {
        treeWndCtrl = (Helium::TreeWndCtrl*) m_Parent->GetWindow();
        m_OwnWindow = false;
    }
    else
    {
        treeWndCtrl = new Helium::TreeWndCtrl( m_Parent->GetWindow() );
        treeWndCtrl->AddRoot( TXT( "Panel Root (hidden)" ) );
        treeWndCtrl->SetHideRoot( true );
#ifdef INSPECT_REFACTOR
        treeWndCtrl->SetImageList( Helium::GlobalFileIconsTable().GetSmallImageList() );
        treeWndCtrl->SetStateImageList( Helium::GlobalFileIconsTable().GetSmallImageList() );
#endif
        m_OwnWindow = true;
    }

    m_Window = treeWndCtrl;
    treeWndCtrl->Freeze();

#ifdef INSPECT_REFACTOR
    int collapsedIndex = Helium::GlobalFileIconsTable().GetIconID( TXT( "ms_folder_closed" ) );
    int expandedIndex = Helium::GlobalFileIconsTable().GetIconID( TXT( "ms_folder_open" ) );
#else
    int collapsedIndex = -1;
    int expandedIndex = -1;
#endif

    wxTreeItemId item = m_ItemData.GetId();
    if ( item == Helium::TreeWndCtrlItemIdInvalid )
    {
        treeWndCtrl->AppendItem( GetParentTreeNode( m_Parent ), m_Name, collapsedIndex, expandedIndex, &m_ItemData );
        item = m_ItemData.GetId();
    }

    V_Control::const_iterator beg = m_Controls.begin();
    V_Control::const_iterator end = m_Controls.end();
    for( V_Control::const_iterator itr = beg; itr != end; ++itr )
    {
        Control* c = *itr;

        c->Realize( this );

        wxWindow* window = c->GetWindow();

        if ( window != treeWndCtrl )
        {
            treeWndCtrl->AppendItem( item, window );
        }
    }

    m_Window->Layout();

    treeWndCtrl->Thaw();

    __super::Realize( parent );
}

void Panel::Unrealize()
{
    wxTreeItemId item = m_ItemData.GetId();
    if ( item != Helium::TreeWndCtrlItemIdInvalid )
    {
        Helium::TreeWndCtrl* treeWndCtrl = (Helium::TreeWndCtrl*) m_Window;
        treeWndCtrl->Delete( item );
        m_ItemData.SetId( Helium::TreeWndCtrlItemIdInvalid );
    }

    if ( !m_OwnWindow )
    {
        m_Window = NULL;
    }

    __super::Unrealize();
}

void Panel::SetEnabled(bool enabled)
{
    if ( m_OwnWindow )
    {
        __super::SetEnabled( enabled );
    }
    else
    {
        // Do not set m_Window to be enabled/disabled because we do not own m_Window.
        wxWindow* window = m_Window;

        m_Window = NULL;
        __super::SetEnabled( enabled );
        m_Window = window;
    }
}

void Panel::BuildPath(tstring& path)
{
    if (m_Parent != NULL && m_Parent->HasType(Reflect::GetType<Panel>()))
    {
        static_cast<Panel*>(m_Parent)->BuildPath(path);
    }

    path += TXT( "|" ) + m_Name;
}

wxTreeItemId Panel::GetParentTreeNode(Container* parent)
{
    for ( ; parent != NULL; parent = parent->GetParent() )
    {
        Panel* parentPanel = Reflect::ObjectCast<Panel>( m_Parent );
        if ( parentPanel )
        {
            wxTreeItemId parentItem = parentPanel->m_ItemData.GetId();
            if ( parentItem != Helium::TreeWndCtrlItemIdInvalid )
            {
                return parentItem;
            }
        }
    }

    Helium::TreeWndCtrl* treeWndCtrl = (Helium::TreeWndCtrl*) m_Window;
    return treeWndCtrl->GetRootItem();
}

void Panel::Read()
{
    if ( IsBound() )
    {
        ReadData( m_Name );

        if ( m_Canvas )
        {
            wxTreeItemId item = m_ItemData.GetId();
            if ( item != Helium::TreeWndCtrlItemIdInvalid )
            {
                Helium::TreeWndCtrl* treeWndCtrl = (Helium::TreeWndCtrl*) m_Window;
                treeWndCtrl->SetItemText( item, m_Name );
            }
        }
    }

    __super::Read();
}
