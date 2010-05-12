#include "Precompiled.h"
#include "DocumentPanel.h"

BEGIN_EVENT_TABLE(DocumentPanel, wxPanel)
END_EVENT_TABLE()

DocumentPanel::DocumentPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: wxPanel(parent, id, pos, size, style, name)
, m_Document (NULL)
, m_Outline (NULL)
{
  // outline
  m_Outline = new wxTreeCtrl ( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxNO_BORDER | wxTR_HIDE_ROOT );
  m_Root = m_Outline->AddRoot("INVISIBLE SANDWICH");

  m_PanelManager.SetManagedWindow( this ); 
  m_PanelManager.AddPane( m_Outline, wxAuiPaneInfo().Name( wxT( "outline" ) ).CloseButton( false ).Caption( wxT( "Outline" ) ).CenterPane() );
  m_PanelManager.AddPane( new wxPanel ( this ), wxAuiPaneInfo().Name( wxT( "view" ) ).CloseButton( false ).Caption( wxT( "View" ) ).Right().Layer( 2 ).Position( 1 ).MinSize( wxSize (80, 80) ) );
  m_PanelManager.Update();
}

DocumentPanel::~DocumentPanel()
{
  m_PanelManager.UnInit();

  SetDocument(NULL);
}

Document* DocumentPanel::GetDocument()
{
  return m_Document;
}

void DocumentPanel::SetDocument(Document* document)
{
  if (m_Document == document)
  {
    return;
  }

  if (m_Document)
  {
    m_Document->GetTitleChangedEvent().Remove( TitleSignature::Delegate ( this, &DocumentPanel::OnTitleChanged ) );
  }

  m_Document = document;

  if (m_Document)
  {
    m_Document->GetTitleChangedEvent().Add( TitleSignature::Delegate ( this, &DocumentPanel::OnTitleChanged ) );
  }
}

void DocumentPanel::OnTitleChanged( const TitleArgs& args )
{
  m_TitleChanged.Raise( args );
}
