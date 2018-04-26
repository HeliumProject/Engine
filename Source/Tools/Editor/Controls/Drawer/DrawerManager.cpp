#include "Precompile.h"
#include "DrawerManager.h"

//#include "Application/UI/Button.h"
//#include "Application/UI/ImageManager.h"

using namespace Helium;
using namespace Helium::Editor;

DrawerManager::DrawerManager()
: m_CurrentDrawer( NULL )
, m_IsFixedSizeButtons( false )
{
}

DrawerManager::~DrawerManager()
{
    RemoveAllDrawers();
}

void DrawerManager::SetAuiManager( wxAuiManager* auiManager )
{
    for ( M_DrawerButtonIDs::const_iterator drawerItr = m_Drawers.begin(), drawerEnd = m_Drawers.end(); drawerItr != drawerEnd; ++drawerItr )
    {
        Drawer* drawer = drawerItr->second;
        drawer->SetAuiManager( auiManager );
    }
}

void DrawerManager::AddDrawer( Drawer* drawer )
{
    // Once the button has been added to the sizer it should have a valid ID
    HELIUM_ASSERT( drawer->GetButtonID() != wxID_ANY );
    m_Drawers.insert( M_DrawerButtonIDs::value_type( drawer->GetButtonID(), drawer ) );

    // Add listeners
    drawer->e_Opening.AddMethod( this, &DrawerManager::OnDrawerOpening );
    drawer->e_Opened.AddMethod( this, &DrawerManager::OnDrawerOpened );
    drawer->e_Closed.AddMethod( this, &DrawerManager::OnDrawerClosed );
}

void DrawerManager::RemoveDrawer( Drawer* drawer )
{
    // Close and detach from all drawers
    M_DrawerButtonIDs::iterator foundDrawerItr = m_Drawers.find( drawer->GetButtonID() );
    if ( foundDrawerItr != m_Drawers.end() )
    {
        Drawer* foundDrawer = foundDrawerItr->second;
        
        foundDrawer->e_Opening.RemoveMethod( this, &DrawerManager::OnDrawerOpening );
        foundDrawer->e_Opened.RemoveMethod( this, &DrawerManager::OnDrawerOpened );
        foundDrawer->e_Closed.RemoveMethod( this, &DrawerManager::OnDrawerClosed );

        foundDrawer->Close();

        m_Drawers.erase( foundDrawerItr );
    }
}

void DrawerManager::RemoveAllDrawers()
{
    // Close and detach from all drawers
    for ( M_DrawerButtonIDs::const_iterator drawerItr = m_Drawers.begin(), drawerEnd = m_Drawers.end(); drawerItr != drawerEnd; ++drawerItr )
    {
        Drawer* drawer = drawerItr->second;
        
        drawer->e_Opening.RemoveMethod( this, &DrawerManager::OnDrawerOpening );
        drawer->e_Opened.RemoveMethod( this, &DrawerManager::OnDrawerOpened );
        drawer->e_Closed.RemoveMethod( this, &DrawerManager::OnDrawerClosed );

        drawer->Close();

        //delete drawer;
    }
    m_Drawers.clear();
}

Drawer* DrawerManager::GetCurrentDrawer() const
{
    return m_CurrentDrawer;
}

bool DrawerManager::IsFixedSizeButtons() const
{
    return m_IsFixedSizeButtons;
}

void DrawerManager::SetFixedSizeButtons( bool fixedSize )
{
    if ( fixedSize != m_IsFixedSizeButtons )
    {
        // TODO
        //m_IsFixedSizeButtons = fixedSize;
        //Freeze();
        //RemoveAllDrawers();
        //GetSizer()->Clear( true );
        //CreateDrawers();
        //Layout();
        //Thaw();
    }
}

Drawer* DrawerManager::FindDrawer( int32_t drawerID )
{
    M_DrawerButtonIDs::const_iterator findDrawer = m_Drawers.find( drawerID );
    if ( findDrawer != m_Drawers.end() )
    {
        return findDrawer->second;
    }
    return NULL;
}

void DrawerManager::OnDrawerOpening( const DrawerEventArgs& args )
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

void DrawerManager::OnDrawerOpened( const DrawerEventArgs& args )
{
    // set current drawer
    if ( m_CurrentDrawer != args.m_Drawer )
    {
        m_CurrentDrawer = args.m_Drawer;
    }
}

void DrawerManager::OnDrawerClosed( const DrawerEventArgs& args )
{
    if ( args.m_Drawer == m_CurrentDrawer )
    {
        m_CurrentDrawer = NULL;
    }
}
