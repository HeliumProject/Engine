#include "Precompile.h"
#include "Drawer.h"

#include "FloatingWindow.h"

using namespace Luna;

static int s_PopupOffsetY = 0; // Pixel offset for top of popup window

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
