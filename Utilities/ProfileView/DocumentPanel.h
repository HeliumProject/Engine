#pragma once

#include "Document.h"

class DocumentPanel : public wxPanel
{
public:
  DocumentPanel( wxWindow* parent,
                 wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                 const wxString& name = wxPanelNameStr);

  ~DocumentPanel();

  Document* GetDocument();
  void SetDocument( Document* document );

  TitleSignature::Event& GetTitleChangeEvent()
  {
    return m_TitleChanged;
  }

private:
  // document events
  void OnTitleChanged( const TitleArgs& args );

private:
  // GUI managment
  wxAuiManager                    m_PanelManager;

  // GUI elements
  wxTreeCtrl*                     m_Outline;
  wxTreeItemId                    m_Root;

  // document
  Document*                       m_Document;

  // events
  TitleSignature::Event           m_TitleChanged;

  DECLARE_EVENT_TABLE();
};
