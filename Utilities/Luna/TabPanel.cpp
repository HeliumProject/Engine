#include "Precompile.h"
#include "TabPanel.h"

using namespace Luna;

TabPanel::TabPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: wxPanel( parent, id, pos, size, style, name )
, m_Tabs( NULL )
, m_OnlyChild( NULL )
, m_Sizer( NULL )
{
  m_Sizer = new wxBoxSizer( wxVERTICAL );
  SetSizer( m_Sizer );
  Layout();
}

TabPanel::~TabPanel()
{
}

void TabPanel::AddWindow( wxWindow* child )
{
  if ( m_Tabs )
  {
    m_Tabs->AddPage( child, child->GetName() );
  }
  else if ( m_OnlyChild )
  {
    m_Sizer->Detach( m_OnlyChild );

    const long tabStyle = wxAUI_NB_DEFAULT_STYLE;
    m_Tabs = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, tabStyle );

    m_OnlyChild->SetParent( m_Tabs );
    m_Tabs->AddPage( m_OnlyChild, m_OnlyChild->GetName() );
    m_OnlyChild = NULL;

    m_Tabs->AddPage( child, child->GetName() );

    m_Sizer->Add( m_Tabs, 1, wxALL, 0 );
    m_Sizer->FitInside( this );
    Layout();
  }
  else
  {
    m_OnlyChild = child;
    m_Sizer->Add( child, 1, wxALL | wxEXPAND, 0 );
    m_Sizer->FitInside( this );
    Layout();
  }
}

void TabPanel::RemoveWindow( wxWindow* child )
{
}
