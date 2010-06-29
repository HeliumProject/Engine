#include "Precompile.h"
#include "BangleWindow.h"
#include "PreviewWindow.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
BangleWindow::BangleWindow( PreviewWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: wxDialog( parent, id, title, pos, size, style, name )
, m_MinSize( 100, 25 )
, m_PreviewWindow( parent )
{
  wxBoxSizer* panelSizer = new wxBoxSizer( wxVERTICAL );

  m_Panel = new wxPanel( this );
  panelSizer->Add( m_Panel, 1, wxEXPAND | wxALL, 5 );

  m_CheckBoxSizer = new wxBoxSizer( wxVERTICAL );
  m_Panel->SetSizer( m_CheckBoxSizer );
  
	UpdateSize();
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
BangleWindow::~BangleWindow()
{
  ClearBangles();
}

///////////////////////////////////////////////////////////////////////////////
// Clears the list of bangles
// 
void BangleWindow::ClearBangles()
{
  m_Panel->SetFocus();

  m_CheckBoxSizer->Clear();

  m_Panel->DestroyChildren();
}

///////////////////////////////////////////////////////////////////////////////
// Refreshes the list of bangles
// 
void BangleWindow::RefreshBangles()
{
  ClearBangles();

  const M_BangleScene& bangleScene = m_PreviewWindow->GetBangleScenes();
  for ( M_BangleScene::const_iterator itr = bangleScene.begin(), end = bangleScene.end(); itr != end; ++itr )
  {
    tchar bangleString[ 128 ] = { 0 };
    _stprintf( bangleString, TXT( "Bangle %d" ), itr->first );

    wxCheckBox* checkBox = new wxCheckBox( m_Panel, wxID_ANY, bangleString );
    checkBox->SetValue( itr->second.m_Draw );
    checkBox->SetClientData( (void*) &itr->first );
    Connect( checkBox->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( BangleWindow::OnCheckBoxClicked ), NULL, this );

    m_CheckBoxSizer->Add( checkBox, 0, wxALL, 5 );
  }
  
  UpdateSize();
}

///////////////////////////////////////////////////////////////////////////////
// Updates the window size to reflect the number of bangle checkboxes
// 
void BangleWindow::UpdateSize()
{
  Layout();
  wxSize minSize = m_CheckBoxSizer->GetMinSize();

  if ( minSize.x < m_MinSize.x )
  {
    minSize.x = m_MinSize.x;
  }
  
  if ( minSize.y < m_MinSize.y )
  {
    minSize.y = m_MinSize.y;
  }

  SetClientSize( minSize );
}

///////////////////////////////////////////////////////////////////////////////
// Turns a bangle on/off.
// 
void BangleWindow::OnCheckBoxClicked( wxCommandEvent& args )
{
  wxCheckBox* checkBox = (wxCheckBox*) args.GetEventObject();
  m_PreviewWindow->SetBangleDraw( *( (int*) checkBox->GetClientData() ), checkBox->GetValue() );
}
