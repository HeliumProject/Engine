#include "Precompile.h"
#include "GameRowPanel.h"
#include "UIToolKit/ImageManager.h"

using namespace Luna;

GameRowPanel::GameRowPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: GameRowGenerated( parent, id, pos, size, style )
, m_IsSelected( false )
, m_RevisionStatus( RevisionStats::None )
, m_IsLoading( false )
, m_ContextMenu( NULL )
, m_LoadingIcon( NULL )
, m_TextRectInflation( 2, 2 )
{
  m_LoadingIcon = new wxAnimationCtrl( this, wxID_ANY, UIToolKit::GlobalImageManager().GetAnimation( "throbber_16.gif" ) );
  m_LoadingIcon->Hide();
  m_DefaultTextBackground = m_Label->GetBackgroundColour();
  m_DefaultTextForeground = m_Label->GetForegroundColour();

  Connect( GetId(), wxEVT_PAINT, wxPaintEventHandler( GameRowPanel::OnPaint ), NULL, this );

  m_Icon2->Connect( m_Icon2->GetId(), wxEVT_LEFT_UP, wxMouseEventHandler( GameRowPanel::OnMouseLeftUp ), NULL, this );
  m_Icon2->Connect( m_Icon2->GetId(), wxEVT_RIGHT_DOWN, wxMouseEventHandler( GameRowPanel::OnMouseRightDown ), NULL, this );
  m_Icon2->Connect( m_Icon2->GetId(), wxEVT_RIGHT_UP, wxMouseEventHandler( GameRowPanel::OnMouseRightUp ), NULL, this );

  m_Icon2->Connect( m_Icon2->GetId(), wxEVT_LEFT_UP, wxMouseEventHandler( GameRowPanel::OnMouseLeftUp ), NULL, this );
  m_Icon2->Connect( m_Icon2->GetId(), wxEVT_RIGHT_DOWN, wxMouseEventHandler( GameRowPanel::OnMouseRightDown ), NULL, this );
  m_Icon2->Connect( m_Icon2->GetId(), wxEVT_RIGHT_UP, wxMouseEventHandler( GameRowPanel::OnMouseRightUp ), NULL, this );

  m_Label->Connect( m_Label->GetId(), wxEVT_LEFT_UP, wxMouseEventHandler( GameRowPanel::OnMouseLeftUp ), NULL, this );
  m_Label->Connect( m_Label->GetId(), wxEVT_RIGHT_DOWN, wxMouseEventHandler( GameRowPanel::OnMouseRightDown ), NULL, this );
  m_Label->Connect( m_Label->GetId(), wxEVT_RIGHT_UP, wxMouseEventHandler( GameRowPanel::OnMouseRightUp ), NULL, this );

  // TEMPORARY
  m_CheckBox->Connect( m_CheckBox->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( GameRowPanel::TemporaryCheckBoxCallback ), NULL, this );
}

GameRowPanel::~GameRowPanel()
{
  DeleteContextMenu();

  Disconnect( GetId(), wxEVT_PAINT, wxPaintEventHandler( GameRowPanel::OnPaint ), NULL, this );

  m_Icon2->Disconnect( m_Icon2->GetId(), wxEVT_LEFT_UP, wxMouseEventHandler( GameRowPanel::OnMouseLeftUp ), NULL, this );
  m_Icon2->Disconnect( m_Icon2->GetId(), wxEVT_RIGHT_DOWN, wxMouseEventHandler( GameRowPanel::OnMouseRightDown ), NULL, this );
  m_Icon2->Disconnect( m_Icon2->GetId(), wxEVT_RIGHT_UP, wxMouseEventHandler( GameRowPanel::OnMouseRightUp ), NULL, this );

  m_Icon2->Disconnect( m_Icon2->GetId(), wxEVT_LEFT_UP, wxMouseEventHandler( GameRowPanel::OnMouseLeftUp ), NULL, this );
  m_Icon2->Disconnect( m_Icon2->GetId(), wxEVT_RIGHT_DOWN, wxMouseEventHandler( GameRowPanel::OnMouseRightDown ), NULL, this );
  m_Icon2->Disconnect( m_Icon2->GetId(), wxEVT_RIGHT_UP, wxMouseEventHandler( GameRowPanel::OnMouseRightUp ), NULL, this );

  m_Label->Disconnect( m_Label->GetId(), wxEVT_LEFT_UP, wxMouseEventHandler( GameRowPanel::OnMouseLeftUp ), NULL, this );
  m_Label->Disconnect( m_Label->GetId(), wxEVT_RIGHT_DOWN, wxMouseEventHandler( GameRowPanel::OnMouseRightDown ), NULL, this );
  m_Label->Disconnect( m_Label->GetId(), wxEVT_RIGHT_UP, wxMouseEventHandler( GameRowPanel::OnMouseRightUp ), NULL, this );
}

void GameRowPanel::SetLabel( const wxString& label )
{
  __super::SetLabel( label );
  m_Label->SetLabel( label );
}

wxBitmap GameRowPanel::GetIcon() const
{
  return m_Icon2->GetBitmap();
}

void GameRowPanel::SetIcon( const wxBitmap& icon )
{
  m_Icon2->SetBitmap( icon );
}

void GameRowPanel::SetRevisionStatus( RevisionStat status )
{
  if ( m_RevisionStatus != status )
  {
    m_RevisionStatus = status;

    switch ( m_RevisionStatus )
    {
    case RevisionStats::CheckedOut:
      m_Icon1->SetBitmap( UIToolKit::GlobalImageManager().GetBitmap( "check_blue_16.png" ) );
      break;

    case RevisionStats::CheckedOutByMe:
      m_Icon1->SetBitmap( UIToolKit::GlobalImageManager().GetBitmap( "check_red_16.png" ) );
      break;

    case RevisionStats::OutOfDate:
      m_Icon1->SetBitmap( UIToolKit::GlobalImageManager().GetBitmap( "warning_16.png" ) );
      break;

    default:
      m_Icon1->SetBitmap( wxNullBitmap );
      break;
    }

    m_Icon1->Refresh();
  }
}

void GameRowPanel::SetLoading( bool loading )
{
  if ( m_IsLoading != loading )
  {
    m_IsLoading = loading;

    if ( m_IsLoading )
    {
      m_Icon2->Hide();
      m_InnerSizer->Replace( m_Icon2, m_LoadingIcon );
      m_LoadingIcon->Play( true );
      m_LoadingIcon->Show();
    }
    else
    {
      m_LoadingIcon->Stop();
      m_LoadingIcon->Hide();
      m_InnerSizer->Replace( m_LoadingIcon, m_Icon2 );
      m_Icon2->Show();
    }
    m_InnerSizer->Layout();
  }
}

bool GameRowPanel::IsSelected() const
{
  return m_IsSelected;
}

void GameRowPanel::Select( bool select )
{
  if ( select != m_IsSelected )
  {
    m_IsSelected = select;
    if ( m_IsSelected )
    {
      m_Label->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
      m_Label->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );
    }
    else
    {
      m_Label->SetBackgroundColour( m_DefaultTextBackground );
      m_Label->SetForegroundColour( m_DefaultTextForeground );
    }
    
    RefreshRect( m_Label->GetRect().Inflate( m_TextRectInflation + m_TextRectInflation ) );
  }
}

void GameRowPanel::SetContextMenu( wxMenu* menu )
{
  if ( m_ContextMenu != menu )
  {
    DeleteContextMenu();
    m_ContextMenu = menu;
  }
}

void GameRowPanel::DeleteContextMenu()
{
  delete m_ContextMenu;
  m_ContextMenu = NULL;
}

wxMenu* GameRowPanel::DetachContextMenu()
{
  wxMenu* menu = m_ContextMenu;
  m_ContextMenu = NULL;
  return menu;
}

void GameRowPanel::OnMouseLeftUp( wxMouseEvent& args )
{
  if ( args.ControlDown() )
  {
    Select( !IsSelected() );
  }
  else
  {
    Select();
  }
}

void GameRowPanel::OnMouseRightDown( wxMouseEvent& args )
{
  if ( args.ControlDown() )
  {
    Select( !IsSelected() );
  }
  else
  {
    Select();
  }
}

void GameRowPanel::OnMouseRightUp( wxMouseEvent& args )
{
  if ( IsSelected() && m_ContextMenu )
  {
    PopupMenu( m_ContextMenu, args.GetPosition() );
  }
}

void GameRowPanel::OnPaint( wxPaintEvent& args )
{
  wxPaintDC dc( this );

  if ( IsSelected() )
  {
    wxRect rect = m_Label->GetRect();
    wxRect originalRect = rect;
    rect.Inflate( m_TextRectInflation );
    dc.SetPen( wxPen( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) ) );
    dc.SetBrush( wxBrush( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) ) );
    dc.DrawRectangle( rect.GetTopLeft(), rect.GetSize() );
    RECT winRect;
    winRect.bottom = rect.GetBottom() + 1;
    winRect.left = rect.GetLeft();
    winRect.right = rect.GetRight() + 1;
    winRect.top = rect.GetTop();
    DrawFocusRect( ( HDC )( dc.GetHDC() ), &winRect );
  }

  args.Skip();
}

void GameRowPanel::TemporaryCheckBoxCallback( wxCommandEvent& args )
{
  if ( m_CheckBox->IsChecked() )
  {
    SetLoading( true );
  }
  else
  {
    SetLoading( false );
  }
}
