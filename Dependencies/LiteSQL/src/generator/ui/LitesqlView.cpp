// For compilers that support precompilation, includes LITESQL_L("wx/wx.h").
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "ui.h"
#include "VisualLitesqlApp.h"
#include "LitesqlView.h"
#include "GenerateView.h"

#include "LitesqlDocument.h"

#include "LitesqlModelTreePanel.h"

IMPLEMENT_DYNAMIC_CLASS(LitesqlView, wxView)

BEGIN_EVENT_TABLE(LitesqlView, wxView)
  EVT_MENU(VisualLitesqlApp::ID_GENERATE, LitesqlView::OnGenerate)
  
  EVT_MENU(VisualLitesqlApp::ID_ADD_OBJECT,     LitesqlView::OnAddObject)
  EVT_MENU(VisualLitesqlApp::ID_REMOVE_OBJECT,  LitesqlView::OnRemoveObject)

  EVT_MENU(VisualLitesqlApp::ID_ADD_FIELD,     LitesqlView::OnAddField)
  EVT_MENU(VisualLitesqlApp::ID_REMOVE_FIELD,  LitesqlView::OnRemoveField)

  EVT_MENU(VisualLitesqlApp::ID_ADD_METHOD,     LitesqlView::OnAddMethod)
  EVT_MENU(VisualLitesqlApp::ID_REMOVE_METHOD,  LitesqlView::OnRemoveMethod)

  EVT_MENU(VisualLitesqlApp::ID_ADD_RELATED,    LitesqlView::OnAddRelated)
  EVT_MENU(VisualLitesqlApp::ID_REMOVE_RELATED, LitesqlView::OnRemoveRelated)

  EVT_MENU(VisualLitesqlApp::ID_ADD_RELATION,   LitesqlView::OnAddRelation)
  EVT_MENU(VisualLitesqlApp::ID_REMOVE_RELATION,LitesqlView::OnRemoveRelation)

  EVT_CONTEXT_MENU(LitesqlView::OnContextMenu)
  
END_EVENT_TABLE()

using namespace xml;
using namespace litesql;
using namespace ui;

LitesqlView::LitesqlView() 
  : frame((wxMDIChildFrame *) NULL),m_ctxMenu(NULL)
{}

LitesqlView::~LitesqlView()
{
  if (m_ctxMenu)
  {
    delete m_ctxMenu;
  }
}

wxMenu* LitesqlView::GetContextMenu()
{
  if (!m_ctxMenu) 
  {
    m_ctxMenu = new wxMenu;
    m_ctxMenu->Append(VisualLitesqlApp::ID_ADD_OBJECT,_(LITESQL_L("Add Object ...")));
    m_ctxMenu->Append(VisualLitesqlApp::ID_REMOVE_OBJECT,_(LITESQL_L("Remove Object ...")));
    m_ctxMenu->AppendSeparator();
    m_ctxMenu->Append(VisualLitesqlApp::ID_ADD_FIELD,_(LITESQL_L("Add Field...")));
    m_ctxMenu->Append(VisualLitesqlApp::ID_REMOVE_FIELD,_(LITESQL_L("Remove Field ...")));
    m_ctxMenu->AppendSeparator();
    m_ctxMenu->Append(VisualLitesqlApp::ID_ADD_METHOD,_(LITESQL_L("Add Method ...")));
    m_ctxMenu->Append(VisualLitesqlApp::ID_REMOVE_METHOD,_(LITESQL_L("Remove Method...")));
    m_ctxMenu->AppendSeparator();
    m_ctxMenu->Append(VisualLitesqlApp::ID_ADD_RELATED,_(LITESQL_L("Add Related...")));
    m_ctxMenu->Append(VisualLitesqlApp::ID_REMOVE_RELATED,_(LITESQL_L("Remove Related ...")));
    m_ctxMenu->AppendSeparator();
    m_ctxMenu->Append(VisualLitesqlApp::ID_ADD_RELATION,_(LITESQL_L("Add Relation...")));
    m_ctxMenu->Append(VisualLitesqlApp::ID_REMOVE_RELATION,_(LITESQL_L("Remove Relation ...")));
    m_ctxMenu->AppendSeparator();
    m_ctxMenu->Append(VisualLitesqlApp::ID_GENERATE,_(LITESQL_L("Generate...")));
  }

  // \TODO change menu on selected item
  return m_ctxMenu;
}

// What to do when a view is created. Creates actual
// windows for displaying the view.
bool LitesqlView::OnCreate(wxDocument *doc, long WXUNUSED(flags) )
{
	frame = wxGetApp().CreateChildFrame(doc, this);
	frame->SetTitle(_T(LITESQL_L("LitesqlView")));

#ifdef __X__
	// X seems to require a forced resize
	int x, y;
	frame->GetSize(&x, &y);
	frame->SetSize(wxDefaultCoord, wxDefaultCoord, x, y);
#endif

	panel = new LitesqlModelTreePanel(frame);
	panel->Layout();
	frame->Show(true);
	Activate(true);

	return true;
}

void LitesqlView::OnUpdate(wxView *sender, wxObject *hint) 
{
	panel->setObjectModel(((LitesqlDocument*) GetDocument())->GetModel());
  panel->Layout();
}

// Sneakily gets used for default print/preview
// as well as drawing on the screen.
void LitesqlView::OnDraw(wxDC *dc)
{
  dc->SetFont(*wxNORMAL_FONT);
  dc->SetPen(*wxBLACK_PEN);
}

// Clean up windows used for displaying the view.
bool LitesqlView::OnClose(bool deleteWindow)
{
  if (!GetDocument()->Close())
    return false;
  wxApp::GetInstance();

  wxString s(wxTheApp->GetAppName());
  if (frame)
    frame->SetTitle(s);

  SetFrame((wxFrame*)NULL);

  Activate(false);

  if (deleteWindow)
  {
    delete frame;
    return true;
  }
  return true;
}



void LitesqlView::OnContextMenu(wxContextMenuEvent& event)
{
  if (frame)
  {
    frame->PopupMenu(GetContextMenu(),frame->ScreenToClient(event.GetPosition()));
  }
}


void LitesqlView::OnAddObject(wxCommandEvent& WXUNUSED(event) )
{
  ObjectPtr newObject(new Object(LITESQL_L("newObject"),LITESQL_L("")));
  panel->AddObject(newObject);
  GetDocument()->Modify(true);
  GetDocument()->UpdateAllViews(this,NULL);
}

void LitesqlView::OnRemoveObject(wxCommandEvent& WXUNUSED(event) )
{
  if (panel->RemoveObject())
  {
    GetDocument()->Modify(true);
    GetDocument()->UpdateAllViews(this,NULL);
  }
}

void LitesqlView::OnAddField(wxCommandEvent& WXUNUSED(event) )
{
  panel->AddField();
  GetDocument()->Modify(true);
  GetDocument()->UpdateAllViews(this,NULL);
}

void LitesqlView::OnRemoveField(wxCommandEvent& WXUNUSED(event) )
{
  if (panel->RemoveField())
  {
    GetDocument()->Modify(true);
    GetDocument()->UpdateAllViews(this,NULL);
  }
}

void LitesqlView::OnAddMethod(wxCommandEvent& WXUNUSED(event) )
{
  panel->AddMethod();
  GetDocument()->Modify(true);
  GetDocument()->UpdateAllViews(this,NULL);
	
}

void LitesqlView::OnRemoveMethod(wxCommandEvent& WXUNUSED(event) )
{
  if (panel->RemoveMethod())
  {
    GetDocument()->Modify(true);
    GetDocument()->UpdateAllViews(this,NULL);
  }
}

void LitesqlView::OnAddRelated(wxCommandEvent& WXUNUSED(event) )
{}

void LitesqlView::OnRemoveRelated(wxCommandEvent& WXUNUSED(event) )
{}

void LitesqlView::OnAddRelation(wxCommandEvent& WXUNUSED(event) )
{
  panel->AddRelation();
  GetDocument()->Modify(true);
  GetDocument()->UpdateAllViews(this,NULL);
}

void LitesqlView::OnRemoveRelation(wxCommandEvent& WXUNUSED(event) )
{}

void LitesqlView::OnGenerate(wxCommandEvent& WXUNUSED(event) )
{
  wxGetApp().m_pGenerateViewTemplate->CreateView(GetDocument());
}
