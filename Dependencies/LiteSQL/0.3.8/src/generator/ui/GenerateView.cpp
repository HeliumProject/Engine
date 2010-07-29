// For compilers that support precompilation, includes  LiteSQL_L( "wx/wx.h" ).
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

#include "VisualLitesqlApp.h"
#include "GenerateView.h"
#include "LitesqlDocument.h"



IMPLEMENT_DYNAMIC_CLASS(GenerateView, wxView)

BEGIN_EVENT_TABLE(GenerateView, wxView)
END_EVENT_TABLE()



GenerateView::GenerateView()
{ frame = (wxMDIChildFrame *) NULL; }

GenerateView::~GenerateView()
{}

// What to do when a view is created. Creates actual
// windows for displaying the view.
bool GenerateView::OnCreate(wxDocument *doc, long WXUNUSED(flags) )
{
    frame = wxGetApp().CreateChildFrame(doc,this);
    frame->SetTitle(_T( LiteSQL_L( "GenerateView" )));

#ifdef __X__
    // X seems to require a forced resize
    int x, y;
    frame->GetSize(&x, &y);
    frame->SetSize(wxDefaultCoord, wxDefaultCoord, x, y);
#endif
    m_pGeneratePanel = new LitesqlGeneratePanel(frame,((LitesqlDocument*)doc)->GetModel());

    frame->Layout();
    frame->Show(true);
    Activate(true);

    return true;
}

void GenerateView::OnUpdate(wxView *sender, wxObject *hint) 
{
//  FillTree(((LitesqlDocument*) GetDocument())->GetModel() ,m_treebook);
}
    
// Sneakily gets used for default print/preview
// as well as drawing on the screen.
void GenerateView::OnDraw(wxDC *dc)
{
  dc->SetFont(*wxNORMAL_FONT);
  dc->SetPen(*wxBLACK_PEN);
}

// Clean up windows used for displaying the view.
bool GenerateView::OnClose(bool deleteWindow)
{
  if (!GetDocument()->Close())
    return false;
    
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