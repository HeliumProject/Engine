#include "Drawer.h"

#include "FloatingWindow.h"

using namespace Nocturnal;

static int s_PopupOffsetY = 0; // Pixel offset for top of popup window

//m_DrawerPanel = new DrawerPanel( this );

//wxAuiPaneInfo info;
//info.MinSize( GetSize().x, -1 );
//info.Name( wxT( "DrawerToolBar" ) );
//info.DestroyOnClose( false );
//info.Caption( wxT( "Drawers" ) );
//info.ToolbarPane();
//info.Gripper( false );
//info.Top();
//info.Floatable( false );
//info.BottomDockable( false );
//info.LeftDockable( false );
//info.RightDockable( false );

//AddDrawer( new Drawer( m_FrameManager.GetPane( TXT("directory") ), wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Unknown ) ) );
//AddDrawer( new Drawer( m_FrameManager.GetPane( TXT("properties") ), wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Unknown ) ) );
//AddDrawer( new Drawer( m_FrameManager.GetPane( TXT("types") ), wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Unknown ) ) );
//AddDrawer( new Drawer( m_FrameManager.GetPane( TXT("layers") ), wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Unknown ) ) );

//m_FrameManager.AddPane( m_DrawerPanel, info );


///////////////////////////////////////////////////////////////////////////////
// Callback for when the pane on a drawer is modified (meaning that we need to 
// update the FrameManager).
//
//void SceneEditor::OnDrawerPaneModified( const DrawerArgs& args )
//{
//    m_FrameManager.Update();
//}

///////////////////////////////////////////////////////////////////////////////
// Adds the drawer to the panel and does other setup work.
// 
//void SceneEditor::AddDrawer( Drawer* drawer )
//{
//    m_DrawerPanel->AddDrawer( drawer );
//    drawer->AddPaneModifiedListener( DrawerSignature::Delegate( this, &SceneEditor::OnDrawerPaneModified ) );
//}

Drawer::Drawer( wxAuiPaneInfo& pane, const wxBitmap& icon ) 
: m_Pane( &pane )
, m_Icon( icon )
, m_ID( -1 )
, m_PopupRect( wxPoint( 0, 0 ), wxSize( 200, 385 ) )
, m_FloatingWindow( NULL )
{
}

Drawer::~Drawer()
{
  if ( m_FloatingWindow )
  {
    m_FloatingWindow->Disconnect( m_FloatingWindow->GetId(), wxEVT_CLOSE_WINDOW, wxCloseEventHandler( Drawer::OnCloseFloatingWindow ), NULL, this );
  }
}

void Drawer::OpenDrawer( const wxPoint& loc )
{
  if ( !IsOpen() )
  {
    if ( loc != wxDefaultPosition )
    {
      m_PopupRect.SetTopLeft( loc );
    }

    m_Pane->Show();
    m_PaneModifiedEvent.Raise( this );

    if ( m_FloatingWindow == NULL )
    {
      m_FloatingWindow = wxDynamicCast( m_Pane->frame, FloatingWindow );
      if ( m_FloatingWindow )
      {
        m_FloatingWindow->Connect( m_FloatingWindow->GetId(), wxEVT_CLOSE_WINDOW, wxCloseEventHandler( Drawer::OnCloseFloatingWindow ), NULL, this );
        m_FloatingWindow->AttachToDrawer( this );
      }
    }

    m_OpenEvent.Raise( this );
  }
}

void Drawer::CloseDrawer()
{
  if ( IsOpen() )
  {
    m_Pane->Hide();
    m_PaneModifiedEvent.Raise( this );
    m_CloseEvent.Raise( this );
  }
}

bool Drawer::IsOpen() const
{
  return m_Pane->IsShown();
}

wxAuiPaneInfo* Drawer::GetPane()
{
  return m_Pane;
}

i32 Drawer::GetDrawerID() const
{
  return m_ID;
}

void Drawer::SetDrawerID( i32 id )
{
  m_ID = id;
}

const wxString& Drawer::GetTitle() const
{
  return m_Pane->caption;
}

const wxBitmap& Drawer::GetIcon() const
{
  return m_Icon;
}

void Drawer::OnCloseFloatingWindow( wxCloseEvent& args )
{
  args.Skip();
  m_FloatingWindow->Disconnect( m_FloatingWindow->GetId(), wxEVT_CLOSE_WINDOW, wxCloseEventHandler( Drawer::OnCloseFloatingWindow ), NULL, this );
  m_FloatingWindow = NULL;
  m_CloseEvent.Raise( this );
}
