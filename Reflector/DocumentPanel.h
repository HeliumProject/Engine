#pragma once

#include "Document.h"

#include "Inspect/Canvas.h"
#include "InspectReflect/ReflectInterpreter.h"

typedef std::map<Reflect::ElementPtr, wxTreeItemId> M_ElementToID;

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
  // UI events
  void OnOutlineSelectionChanged( wxTreeEvent& event );

  // document events
  void OnTitleChanged( const TitleArgs& args );
  void OnElementAdded( const ElementArgs& args );
  void OnElementRemoved( const ElementArgs& args );
  void OnSelectionChanged( const SelectionArgs& args );

private:
  // GUI managment
  wxAuiManager                    m_PanelManager;
  Inspect::ReflectInterpreterPtr  m_Interpreter;

  // GUI elements
  wxTreeCtrl*                     m_Outline;
  wxTreeItemId                    m_Root;
  M_ElementToID                   m_IDs;
  Inspect::Canvas                 m_Canvas;

  // document
  Document*                       m_Document;

  // events
  TitleSignature::Event           m_TitleChanged;

  DECLARE_EVENT_TABLE();
};
