#include "Group.h"
#include "Application/Inspect/Controls/Canvas.h"
#include "Application/Inspect/Controls/Label.h"

using namespace Helium::Inspect;

PanelOutline::PanelOutline( wxWindow* parent, Group* group, const tchar* name )
: wxPanel( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER, name )
, m_Group( group )
{
}

void PanelOutline::OnPaint( wxPaintEvent& args )
{
  if ( !m_Group->IsBorderEnabled() )
  {
    args.Skip();
    return;
  }

  wxPaintDC dc( this );
  wxColour color = wxSystemSettings::GetColour( m_Group->IsSelected() ? wxSYS_COLOUR_HIGHLIGHT : wxSYS_COLOUR_BTNSHADOW );
  wxPen pen( color );
  dc.SetPen(pen);

  wxSize size = GetSize();
  dc.DrawLine( wxPoint( 0,                   0 ),                      wxPoint( size.GetWidth(),     0 ) );
  dc.DrawLine( wxPoint( size.GetWidth() - 1, 0 ),                      wxPoint( size.GetWidth() - 1, size.GetHeight() - 1 ) );
  dc.DrawLine( wxPoint( size.GetWidth() - 1, size.GetHeight() - 1 ),   wxPoint( 0,                   size.GetHeight() - 1 ) );
  dc.DrawLine( wxPoint( 0,                   size.GetHeight() - 1 ),   wxPoint( 0,                   0 ) );
}

BEGIN_EVENT_TABLE( PanelOutline, wxPanel )
EVT_PAINT( PanelOutline::OnPaint )
END_EVENT_TABLE()


Group::Group()
: m_DrawBorder( true )
{
}

void Group::Realize(Container* parent)
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
  m_Window = new PanelOutline (Control::Cast<wxWindow>(parent), this, TXT( "Group's PanelOutline" ) );
  m_Window->SetSizer( new wxBoxSizer (wxVERTICAL) );
  wxSizer* sizer = m_Window->GetSizer();

  int spacing = GetCanvas()->GetBorder();
  int controlSpacing = spacing * 2;
  V_Control::const_iterator itr = m_Controls.begin();
  V_Control::const_iterator end = m_Controls.end();
  for( ; itr != end; ++itr )
  {
    Control* c = *itr;
    c->Realize(this);

    sizer->Add(0, spacing, 0);
    sizer->Add(c->GetWindow(), 0, wxEXPAND | wxLEFT | wxRIGHT, controlSpacing);
  }
  sizer->Add(0, spacing, 0);

  m_Window->Layout();

  __super::Realize(parent);
}

bool Group::IsBorderEnabled() const
{
  return m_DrawBorder;
}

void Group::EnableBorder( bool enable )
{
  if ( m_DrawBorder != enable )
  {
    m_DrawBorder = enable;
    if ( IsRealized() )
    {
      Invalidate();
    }
  }
}

GroupPtr Group::CreatePanel(Control* control)
{
  if (control == NULL)
    return NULL;

  // create a new control
  Group* group = control->GetCanvas()->Create<Group>();

  group->AddControl(control);

  return group;
}

GroupPtr Group::CreatePanel(const V_Control& controls)
{
  if (controls.size() == 0)
    return NULL;

  // create a new control
  GroupPtr group = controls.front()->GetCanvas()->Create<Group>();

  // spool controls to a fresh control
  V_Control::const_iterator itr = controls.begin();
  V_Control::const_iterator end = controls.end();
  for ( ; itr != end; ++itr )
    group->AddControl((*itr));

  return group;
}
