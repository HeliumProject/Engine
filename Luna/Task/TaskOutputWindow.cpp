#include "Precompile.h"

#include <cstring>

#include "TaskOutputWindow.h"

#include "Foundation/Log.h"
#include "Application/UI/ImageManager.h"

using namespace Luna;

enum
{
  BUTTON_Close = wxID_HIGHEST + 1 // declares an id which will be used to call our button
};

BEGIN_EVENT_TABLE(TaskOutputWindow, wxFrame)
EVT_BUTTON( wxID_CLOSE, TaskOutputWindow::OnClose )
EVT_BUTTON( BUTTON_Close, TaskOutputWindow::OnClose )
END_EVENT_TABLE()

UpdateTimer::UpdateTimer( TaskOutputWindow* window )
  : m_Window( window )
{

}

void UpdateTimer::Notify()
{
  m_Window->EnterMessageSection();

  Log::V_Statement& statements = m_Window->GetPendingStatements();

  Log::V_Statement::iterator itr = statements.begin();
  Log::V_Statement::iterator end = statements.end();
  for ( ; itr != end; ++itr )
  {
    wxTextAttr attr ( m_Window->TranslateColor( Log::GetStreamColor( itr->m_Stream ) ) );
    m_Window->GetTextCtrl()->SetDefaultStyle( attr );
    m_Window->GetTextCtrl()->AppendText( itr->m_String );
  }

  statements.clear();

  m_Window->LeaveMessageSection();
}

TaskOutputWindow::TaskOutputWindow( wxWindow* parent, const tstring& title, int xpos, int ypos, int width, int height )
  : wxFrame( parent, -1, title.c_str(), wxPoint( xpos, ypos ), wxSize( width, height ), wxCAPTION|wxMINIMIZE_BOX|wxMAXIMIZE_BOX|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLIP_CHILDREN  )
{
  wxBoxSizer* bSizer1;
  bSizer1 = new wxBoxSizer( wxVERTICAL );

  wxBoxSizer* bSizer3;
  bSizer3 = new wxBoxSizer( wxHORIZONTAL );

  m_TextControl = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH|wxTE_CHARWRAP );
  wxTextAttr attr;
  attr.SetFlags(wxTEXT_ATTR_FONT_FACE);
  attr.SetFont(wxFont(8, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
  m_TextControl->SetDefaultStyle( attr );
  bSizer3->Add( m_TextControl, 1, wxALL|wxEXPAND, 5 );

  bSizer1->Add( bSizer3, 1, wxEXPAND, 5 );

  wxBoxSizer* bSizer4;
  bSizer4 = new wxBoxSizer( wxHORIZONTAL );

  m_CloseButton = new wxButton( this, BUTTON_Close, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0 );
  bSizer4->Add( m_CloseButton, 0, wxALIGN_BOTTOM|wxALL, 5 );

  bSizer1->Add( bSizer4, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

  wxIconBundle iconBundle;
  wxIcon tempIcon;
  tempIcon.CopyFromBitmap( Nocturnal::GlobalImageManager().GetBitmap( TXT( "build.png" ), Nocturnal::IconSizes::Size32 ) );
  iconBundle.AddIcon( tempIcon );
  tempIcon.CopyFromBitmap( Nocturnal::GlobalImageManager().GetBitmap( TXT( "build.png" ) ) );
  iconBundle.AddIcon( tempIcon );
  SetIcons( iconBundle );

  this->SetSizer( bSizer1 );
  this->Layout();

  m_CloseButton->Enable( false );
  m_TextControl->SetBackgroundColour( *wxBLACK );

  AddTaskStartedListener( TaskStartedSignature::Delegate ( this, &TaskOutputWindow::TaskStarted ) );
  AddTaskFinishedListener( TaskFinishedSignature::Delegate ( this, &TaskOutputWindow::TaskFinished ) );

  InitializeCriticalSection( &m_CriticalSection );

  m_MessageTimer = new UpdateTimer( this );
}

TaskOutputWindow::~TaskOutputWindow()
{
  RemoveTaskStartedListener( TaskStartedSignature::Delegate ( this, &TaskOutputWindow::TaskStarted ) );
  RemoveTaskFinishedListener( TaskFinishedSignature::Delegate ( this, &TaskOutputWindow::TaskFinished ) );

  DeleteCriticalSection( &m_CriticalSection );

  delete m_MessageTimer;
}

void TaskOutputWindow::EnterMessageSection()
{
  ::EnterCriticalSection(&m_CriticalSection);
}

void TaskOutputWindow::LeaveMessageSection()
{
  ::LeaveCriticalSection(&m_CriticalSection);
}

const wxColour& TaskOutputWindow::TranslateColor( Log::Color color )
{
  const wxColour* result = wxLIGHT_GREY;

  static const wxColor yellow ( 0xff, 0xff, 0x00 );
  static const wxColor* wxYELLOW (&yellow);

  static const wxColor purple ( 0xff, 0x00, 0xff );
  static const wxColor* wxPURPLE (&purple);

  switch ( color )
  {
  case Log::Colors::Red:
    {
      return *wxRED;
    }

  case Log::Colors::Green:
    {
      return *wxGREEN;
    }

  case Log::Colors::Blue:
    {
      return *wxBLUE;
    }

  case Log::Colors::Yellow:
    {
      return *wxYELLOW;
    }

  case Log::Colors::Aqua:
    {
      return *wxCYAN;
    }

  case Log::Colors::Purple:
    {
      return *wxPURPLE;
    }

  case Log::Colors::White:
    {
      return *wxWHITE;
    }
  };

  return *wxLIGHT_GREY;
}
void TaskOutputWindow::TaskStarted( const TaskStartedArgs& args )
{
  m_TextControl->Clear();
  m_CloseButton->Enable( args.m_Cancelable );

  if (args.m_Cancelable)
  {
    m_CloseButton->SetLabel( TXT( "Cancel" ) );
  }

  m_MessageTimer->Start( 100 );
}

void TaskOutputWindow::TaskFinished( const TaskFinishedArgs& args )
{
  ::EnterCriticalSection( &m_CriticalSection );

  m_MessageTimer->Stop();
  m_MessageTimer->Notify();

  switch (args.m_Result)
  {
  case TaskResults::Success:
    m_TextControl->SetDefaultStyle(wxTextAttr(*wxGREEN, *wxBLACK));
    m_TextControl->AppendText( TXT( "\n\nSucceeded\n" ) );
    break;

  case TaskResults::Failure:
    m_TextControl->SetDefaultStyle(wxTextAttr(*wxRED, *wxBLACK));
    m_TextControl->AppendText( TXT( "\n\nFailed\n" ) );
    break;

  case TaskResults::Cancel:
    m_TextControl->SetDefaultStyle(wxTextAttr(*wxRED, *wxBLACK));
    m_TextControl->AppendText( TXT( "\n\nCancelled\n" ) );
    break;
  }

  if (m_CloseButton->IsEnabled())
  {
    m_CloseButton->SetLabel( wxT( "Close" ) );
  }
  else
  {
    m_CloseButton->Enable( true );
  }

  ::LeaveCriticalSection( &m_CriticalSection );
}

void TaskOutputWindow::PrintListener( const Log::PrintedArgs& args )
{
  ::EnterCriticalSection( &m_CriticalSection );

  m_Statements.push_back( args.m_Statement );

  ::LeaveCriticalSection( &m_CriticalSection );
}

void TaskOutputWindow::OnClose( wxCommandEvent& event )
{
  m_Signal.Raise( Nocturnal::Void () );
}