#include "Precompile.h"
#include "DrawerPanel.h"

//#include "Application/UI/Button.h"
//#include "Application/UI/ImageManager.h"

using namespace Helium;
using namespace Helium::Editor;

DrawerPanel::DrawerPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: wxPanel( parent, id, pos, size, style )
, m_CurrentDrawer( NULL )
, m_IsFixedSizeButtons( false )
{
    wxBoxSizer* sizer = new wxBoxSizer( wxHORIZONTAL );
    SetSizer( sizer );
}

DrawerPanel::~DrawerPanel()
{
    DestroyDrawers();
}

void DrawerPanel::SetAuiManager( wxAuiManager* auiManager )
{
    for ( M_DrawerButtonIDs::const_iterator drawerItr = m_Drawers.begin(), drawerEnd = m_Drawers.end(); drawerItr != drawerEnd; ++drawerItr )
    {
        Drawer* drawer = drawerItr->second;
        drawer->SetAuiManager( auiManager );
    }
}

void DrawerPanel::AddDrawer( Drawer* drawer )
{
    // Add the button to the sizer
    GetSizer()->Add( drawer->GetButton(), m_IsFixedSizeButtons ? 0 : 1, wxEXPAND | wxALL, 0 );

    // Once the button has been added to the sizer it should have a valid ID
    HELIUM_ASSERT( drawer->GetButtonID() != wxID_ANY );
    m_Drawers.insert( M_DrawerButtonIDs::value_type( drawer->GetButtonID(), drawer ) );

    // Add listeners
    drawer->e_Opening.AddMethod( this, &DrawerPanel::OnDrawerOpening );
    drawer->e_Opened.AddMethod( this, &DrawerPanel::OnDrawerOpened );
    drawer->e_Closed.AddMethod( this, &DrawerPanel::OnDrawerClosed );
}

Drawer* DrawerPanel::GetCurrentDrawer() const
{
    return m_CurrentDrawer;
}

bool DrawerPanel::IsFixedSizeButtons() const
{
    return m_IsFixedSizeButtons;
}

void DrawerPanel::SetFixedSizeButtons( bool fixedSize )
{
    if ( fixedSize != m_IsFixedSizeButtons )
    {
        // TODO
        //m_IsFixedSizeButtons = fixedSize;
        //Freeze();
        //DestroyDrawers();
        //GetSizer()->Clear( true );
        //CreateDrawers();
        //Layout();
        //Thaw();
    }
}

void DrawerPanel::DestroyDrawers()
{
    // Close and detach from all drawers
    for ( M_DrawerButtonIDs::const_iterator drawerItr = m_Drawers.begin(), drawerEnd = m_Drawers.end(); drawerItr != drawerEnd; ++drawerItr )
    {
        Drawer* drawer = drawerItr->second;
        
        drawer->e_Opening.RemoveMethod( this, &DrawerPanel::OnDrawerOpening );
        drawer->e_Opened.RemoveMethod( this, &DrawerPanel::OnDrawerOpened );
        drawer->e_Closed.RemoveMethod( this, &DrawerPanel::OnDrawerClosed );

        drawer->Close();

        //delete drawer;
    }
    m_Drawers.clear();
}

Drawer* DrawerPanel::FindDrawer( int32_t drawerID )
{
    M_DrawerButtonIDs::const_iterator findDrawer = m_Drawers.find( drawerID );
    if ( findDrawer != m_Drawers.end() )
    {
        return findDrawer->second;
    }
    return NULL;
}

void DrawerPanel::OnDrawerOpening( const DrawerEventArgs& args )
{
    // close all other drawers
    for ( M_DrawerButtonIDs::const_iterator drawerItr = m_Drawers.begin(), drawerEnd = m_Drawers.end(); drawerItr != drawerEnd; ++drawerItr )
    {
        if ( drawerItr->first != args.m_Drawer->GetButtonID() )
        {
            drawerItr->second->Close();
        }
    }
}

void DrawerPanel::OnDrawerOpened( const DrawerEventArgs& args )
{
    // set current drawer
    if ( m_CurrentDrawer != args.m_Drawer )
    {
        m_CurrentDrawer = args.m_Drawer;
    }
}

void DrawerPanel::OnDrawerClosed( const DrawerEventArgs& args )
{
    if ( args.m_Drawer == m_CurrentDrawer )
    {
        m_CurrentDrawer = NULL;
    }
}
