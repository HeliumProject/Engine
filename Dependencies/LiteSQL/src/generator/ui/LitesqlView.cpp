// For compilers that support precompilation, includes  LITESQL_L( "wx/wx.h" ).
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

#include <wx/treebook.h>

#include "VisualLitesqlApp.h"
#include "LitesqlView.h"
#include "GenerateView.h"

#include "LitesqlDocument.h"

#include "LitesqlObjectPanel.h"
#include "LitesqlFieldPanel.h"
#include "LitesqlMethodPanel.h"
#include "LitesqlRelationPanel.h"
#include "LitesqlDatabasePanel.h"

#include "objectmodel.hpp"

IMPLEMENT_DYNAMIC_CLASS(LitesqlView, wxView)

BEGIN_EVENT_TABLE(LitesqlView, wxView)
  EVT_SIZE(LitesqlView::OnSize) 
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
  
  EVT_TREEBOOK_PAGE_CHANGED(wxID_ANY, LitesqlView::OnPageChanged)
  EVT_TREEBOOK_PAGE_CHANGING(wxID_ANY, LitesqlView::OnPageChanging)

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
    m_ctxMenu->Append(VisualLitesqlApp::ID_ADD_OBJECT,_( LITESQL_L( "Add Object ..." )));
    m_ctxMenu->Append(VisualLitesqlApp::ID_REMOVE_OBJECT,_( LITESQL_L( "Remove Object ..." )));
    m_ctxMenu->AppendSeparator();
    m_ctxMenu->Append(VisualLitesqlApp::ID_ADD_FIELD,_( LITESQL_L( "Add Field..." )));
    m_ctxMenu->Append(VisualLitesqlApp::ID_REMOVE_FIELD,_( LITESQL_L( "Remove Field ..." )));
    m_ctxMenu->AppendSeparator();
    m_ctxMenu->Append(VisualLitesqlApp::ID_ADD_METHOD,_( LITESQL_L( "Add Method ..." )));
    m_ctxMenu->Append(VisualLitesqlApp::ID_REMOVE_METHOD,_( LITESQL_L( "Remove Method..." )));
    m_ctxMenu->AppendSeparator();
    m_ctxMenu->Append(VisualLitesqlApp::ID_ADD_RELATED,_( LITESQL_L( "Add Related..." )));
    m_ctxMenu->Append(VisualLitesqlApp::ID_REMOVE_RELATED,_( LITESQL_L( "Remove Related ..." )));
    m_ctxMenu->AppendSeparator();
    m_ctxMenu->Append(VisualLitesqlApp::ID_ADD_RELATION,_( LITESQL_L( "Add Relation..." )));
    m_ctxMenu->Append(VisualLitesqlApp::ID_REMOVE_RELATION,_( LITESQL_L( "Remove Relation ..." )));
  }

  // \TODO change menu on selected item
  return m_ctxMenu;
}

static void FillTree (ObjectModel* pModel,wxTreebook* pTree) 
{
  int pagePos = 0;

  wxString dbName(pModel->db.name.c_str(),wxConvUTF8);
  pTree->DeleteAllPages();

  pTree->AddPage(new ui::LitesqlDatabasePanel(pTree,&pModel->db),_( LITESQL_L( "database                                      " )));
  pagePos++;
  for( std::vector<Object*>::iterator it = pModel->objects.begin(); it != pModel->objects.end();it++)
  {
    wxString name((*it)->name.c_str(),wxConvUTF8);
    pTree->AddSubPage(new LitesqlObjectPanel(pTree,pModel->objects,*it),name +_( LITESQL_L( "(Object)" )));

    int subPagePos=pagePos++;

    for (std::vector<Field*>::iterator field = (*it)->fields.begin();
      field != (*it)->fields.end();
      field++)
    {
      wxString fname((*field)->name.c_str(),wxConvUTF8);
      pTree->InsertSubPage(subPagePos,new LitesqlFieldPanel(pTree, *field),fname +_( LITESQL_L( "(Field)" )));
      pagePos++;
    }

    for (std::vector<Method*>::iterator method = (*it)->methods.begin();
      method != (*it)->methods.end();
      method++)
    {
      wxString fname((*method)->name.c_str(),wxConvUTF8);
      pTree->InsertSubPage(subPagePos,new LitesqlMethodPanel(pTree, *method) ,fname +_( LITESQL_L( "(Method)" )));
      pagePos++;
    }
  }

  for (std::vector<Relation*>::iterator relation = pModel->relations.begin(); relation!= pModel->relations.end();relation++)
  {
    wxString name((*relation)->name.c_str(),wxConvUTF8);
    pTree->AddSubPage(new LitesqlRelationPanel(pTree,*relation),name +_( LITESQL_L( "(relation)" )));
    pagePos++;
  }

  pTree->Layout();
}

// What to do when a view is created. Creates actual
// windows for displaying the view.
bool LitesqlView::OnCreate(wxDocument *doc, long WXUNUSED(flags) )
{
  frame = wxGetApp().CreateChildFrame(doc, this);
  frame->SetTitle(_T( LITESQL_L( "LitesqlView" )));

#ifdef __X__
  // X seems to require a forced resize
  int x, y;
  frame->GetSize(&x, &y);
  frame->SetSize(wxDefaultCoord, wxDefaultCoord, x, y);
#endif
  
  m_treebook = new wxTreebook(frame,-1);
  
  FillTree(((LitesqlDocument*) doc)->GetModel() ,m_treebook);

  wxSize s = m_treebook->GetTreeCtrl()->GetSize();
  s.SetWidth(s.GetWidth()*2);
  m_treebook->GetTreeCtrl()->SetSize(s);

  m_treebook->Layout();
  
  frame->Show(true);
  Activate(true);

  return true;
}

void LitesqlView::OnSize(wxSizeEvent& WXUNUSED(event))
{
  if (frame) {
    m_treebook->SetSize(frame->GetClientSize());
    
    m_treebook->Layout();
    frame->Layout();
  }

}

void LitesqlView::OnUpdate(wxView *sender, wxObject *hint) 
{
  FillTree(((LitesqlDocument*) GetDocument())->GetModel() ,m_treebook);
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
//  wxMessageBox(_( LITESQL_L( "OnAddObject" )));
  ObjectModel* pModel = ((LitesqlDocument*) GetDocument())->GetModel();
  Object* newObject = new Object( LITESQL_L( "newObject" ), LITESQL_L( "" ));
  pModel->objects.push_back(newObject);
  
  GetDocument()->Modify(true);
  GetDocument()->UpdateAllViews(this,NULL);
}

void LitesqlView::OnRemoveObject(wxCommandEvent& WXUNUSED(event) )
{
  wxMessageBox(_( LITESQL_L( "OnRemoveObject" )));
  GetDocument()->Modify(true);
  GetDocument()->UpdateAllViews(this,NULL);
}

void LitesqlView::OnAddField(wxCommandEvent& WXUNUSED(event) )
{
  wxWindow *pPage = m_treebook->GetCurrentPage();
  if (!pPage) 
  {
  
  }
  else if (pPage->IsKindOf(CLASSINFO(LitesqlObjectPanel)))
  {
    xml::Field* newField = new xml::Field( LITESQL_L( "newField" ),AU_field_type, LITESQL_L( "" ),AU_field_indexed,AU_field_unique);
    ((LitesqlObjectPanel*)pPage)->GetObject()->fields.push_back(newField);
    m_treebook->InsertSubPage(  m_treebook->GetSelection(),
                                new LitesqlFieldPanel(m_treebook, newField),_( LITESQL_L( "newField(Field)" )),
                                true);
      GetDocument()->Modify(true);
//      GetDocument()->UpdateAllViews(this,NULL);
  }
  else
  {
  
  }
}

void LitesqlView::OnRemoveField(wxCommandEvent& WXUNUSED(event) )
{
  wxWindow *pPage = m_treebook->GetCurrentPage();
  if (!pPage) 
  {
  
  }
  else if (pPage->IsKindOf(CLASSINFO(LitesqlFieldPanel)))
  {
      //    int sel = m_treebook->GetSelection();

    ((LitesqlDocument*) GetDocument())->RemoveField(((LitesqlFieldPanel*)pPage)->GetField());
  }
  else
  {
  
  }


}

void LitesqlView::OnAddMethod(wxCommandEvent& WXUNUSED(event) )
{}

void LitesqlView::OnRemoveMethod(wxCommandEvent& WXUNUSED(event) )
{}

void LitesqlView::OnAddRelated(wxCommandEvent& WXUNUSED(event) )
{}

void LitesqlView::OnRemoveRelated(wxCommandEvent& WXUNUSED(event) )
{}

void LitesqlView::OnAddRelation(wxCommandEvent& WXUNUSED(event) )
{}

void LitesqlView::OnRemoveRelation(wxCommandEvent& WXUNUSED(event) )
{}

void LitesqlView::OnGenerate(wxCommandEvent& WXUNUSED(event) )
{
  wxGetApp().m_pGenerateViewTemplate->CreateView(GetDocument());
}

void LitesqlView::OnPageChanged(wxTreebookEvent& WXUNUSED(event))
{
  
  wxWindow *pPage = m_treebook->GetCurrentPage();
  if (pPage)
  {
    pPage->TransferDataToWindow();
  }
}

void LitesqlView::OnPageChanging(wxTreebookEvent& WXUNUSED(event))
{
  wxWindow *pPage = m_treebook->GetCurrentPage();
  if (pPage)
  {
    pPage->TransferDataFromWindow();
  }
}
