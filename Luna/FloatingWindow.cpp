#include "Precompile.h"
#include "FloatingWindow.h"

#include "Drawer.h"

using namespace Luna;

static int s_DismissMilliseconds = 500;


#define MARSHALL_EVENT( func, evt ) \
void func( evt& args ) \
{ \
  m_FloatingWindow->func( args ); \
}

namespace Luna
{
  class DrawerEventMarshaller : public wxEvtHandler
  {
  public:
    DrawerEventMarshaller( FloatingWindow* window )
      : m_FloatingWindow( window )
    {
    }

  private:
    MARSHALL_EVENT( OnMouseEnter, wxMouseEvent );
    MARSHALL_EVENT( OnMouseLeave, wxMouseEvent );

  private:
    FloatingWindow* m_FloatingWindow;

  private:
    DECLARE_EVENT_TABLE()
  };
}

BEGIN_EVENT_TABLE( DrawerEventMarshaller, wxEvtHandler )
EVT_ENTER_WINDOW( DrawerEventMarshaller::OnMouseEnter )
EVT_LEAVE_WINDOW( DrawerEventMarshaller::OnMouseLeave )
END_EVENT_TABLE()

FloatingWindow::FloatingWindow( wxWindow* parent, wxAuiManager* ownerMgr, const wxAuiPaneInfo& pane )
: wxAuiFloatingFrame( parent, ownerMgr, pane, wxID_ANY, wxRESIZE_BORDER | wxSYSTEM_MENU | wxCAPTION | wxFRAME_NO_TASKBAR | wxFRAME_FLOAT_ON_PARENT |  wxCLIP_CHILDREN )
, m_Drawer( NULL )
{
  // Listeners
  Connect( GetId(), wxEVT_ENTER_WINDOW, wxMouseEventHandler( FloatingWindow::OnMouseEnter ), NULL, this );
  Connect( GetId(), wxEVT_LEAVE_WINDOW, wxMouseEventHandler( FloatingWindow::OnMouseLeave ), NULL, this );
  Connect( GetId(), wxEVT_CLOSE_WINDOW, wxCloseEventHandler( FloatingWindow::OnClose ), NULL, this );

  m_Timer.SetOwner( this );
  Connect( m_Timer.GetId(), wxEVT_TIMER, wxTimerEventHandler( FloatingWindow::OnTimer ), NULL, this );
}

FloatingWindow::~FloatingWindow()
{
  Disconnect( m_Timer.GetId(), wxEVT_TIMER, wxTimerEventHandler( FloatingWindow::OnTimer ), NULL, this );

  Disconnect( GetId(), wxEVT_ENTER_WINDOW, wxMouseEventHandler( FloatingWindow::OnMouseEnter ), NULL, this );
  Disconnect( GetId(), wxEVT_LEAVE_WINDOW, wxMouseEventHandler( FloatingWindow::OnMouseLeave ), NULL, this );
  Disconnect( GetId(), wxEVT_CLOSE_WINDOW, wxCloseEventHandler( FloatingWindow::OnClose ), NULL, this );
}

void FloatingWindow::AttachToDrawer( Drawer* drawer )
{
  if ( m_Drawer != drawer ) 
  {
    m_Drawer = drawer;
    wxWindowList::const_iterator childItr = GetChildren().begin();
    wxWindowList::const_iterator childEnd = GetChildren().end();
    for ( ; childItr != childEnd; ++childItr )
    {
      PushEventMarshaller( *childItr );
    }
  }
}

void FloatingWindow::DetachFromDrawer()
{
  if ( m_Drawer != NULL )
  {
    m_Drawer = NULL;
    wxWindowList::const_iterator childItr = GetChildren().begin();
    wxWindowList::const_iterator childEnd = GetChildren().end();
    for ( ; childItr != childEnd; ++childItr )
    {
      PopEventMarshaller( *childItr );
    }
  }
}

bool FloatingWindow::IsAttachedToDrawer() const
{
  return m_Drawer != NULL;
}

void FloatingWindow::PushEventMarshaller( wxWindow* window )
{
  // THIS IS CAUSING A MEMORY LEAK,
  // plus it is flaky, so I'm taking it out for now.

  //window->PushEventHandler( new DrawerEventMarshaller( this ) );

  //wxWindowList::const_iterator childItr = window->GetChildren().begin();
  //wxWindowList::const_iterator childEnd = window->GetChildren().end();
  //for ( ; childItr != childEnd; ++childItr )
  //{
  //  PushEventMarshaller( *childItr );
  //}
}

void FloatingWindow::PopEventMarshaller( wxWindow* window )
{
  //window->PopEventHandler( true );

  //wxWindowList::const_iterator childItr = window->GetChildren().begin();
  //wxWindowList::const_iterator childEnd = window->GetChildren().end();
  //for ( ; childItr != childEnd; ++childItr )
  //{
  //  PopEventMarshaller( *childItr );
  //}
}

void FloatingWindow::OnMouseEnter( wxMouseEvent& event )
{
  m_Timer.Stop();
  event.Skip();
}

void FloatingWindow::OnMouseLeave( wxMouseEvent& event )
{
  event.Skip();

  wxRect rect = GetScreenRect();
  rect.Deflate( 1 );
  wxPoint mousePos = ::wxGetMousePosition();
  if ( !rect.Contains( mousePos ) )
  {
    m_Timer.Start( s_DismissMilliseconds, true );
  }
}

void FloatingWindow::OnTimer( wxTimerEvent& event )
{
  if ( !HasCapture() )
  {
    m_Drawer->CloseDrawer();
  }
}

void FloatingWindow::OnClose( wxCloseEvent& event )
{
  event.Skip();
  wxWindowList::const_iterator childItr = GetChildren().begin();
  wxWindowList::const_iterator childEnd = GetChildren().end();
  for ( ; childItr != childEnd; ++childItr )
  {
    PopEventMarshaller( *childItr );
  }
}
