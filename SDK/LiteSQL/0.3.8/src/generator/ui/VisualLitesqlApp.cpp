#include <wx/docview.h>
#include <wx/cmdproc.h>
#include <wx/menu.h>
#include <wx/image.h>



#include "VisualLitesqlApp.h"
#include "LitesqlDocument.h"
#include "LitesqlView.h"
#include "GenerateView.h"

#include "MainFrame.h"

IMPLEMENT_APP(VisualLitesqlApp)

VisualLitesqlApp::VisualLitesqlApp(void) 
{
	
}

VisualLitesqlApp::~VisualLitesqlApp(void)
{
}

bool VisualLitesqlApp::OnInit(void)
{
  ::wxInitAllImageHandlers();

  //// Create a document manager
  m_docManager = new wxDocManager;

  //// Create a template relating drawing documents to their views
  (void) new wxDocTemplate((wxDocManager *) m_docManager, _T("Litesql-Model"), 
                                                          _T("*.xml"), 
                                                          _T(""), 
                                                          _T  ("xml"), 
                                                          _T("Litesql Model"), 
                                                          _T("Litesql Model View"),
          CLASSINFO(LitesqlDocument), CLASSINFO(LitesqlView));
  m_pGenerateViewTemplate = new wxDocTemplate((wxDocManager *) m_docManager, _T("Litesql-Model (generate)"), 
                                                          _T("*.xml"), 
                                                          _T(""), 
                                                          _T  ("xml"), 
                                                          _T("Litesql Model"), 
                                                          _T("Litesql Generate View"),
          CLASSINFO(LitesqlDocument), CLASSINFO(GenerateView));

  //// Create the main frame window
  pMainframe = new MainFrame((wxDocManager *) m_docManager, (wxFrame *) NULL,
                      _T("Visual Litesql"), wxPoint(0, 0), wxSize(800, 600),
                      wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE);

	// start transaction
  //// Give it an icon (this is ignored in MDI mode: uses resources)
#ifdef __WXMSW__
  pMainframe->SetIcon(wxIcon(_T("doc")));
#endif
#ifdef __X__
  pMainframe->SetIcon(wxIcon(_T("doc.xbm")));
#endif

#ifdef __WXMAC__
  wxMenuBar::MacSetCommonMenuBar(pMainframe->GetMenuBar());
#endif //def __WXMAC__
  
  pMainframe->Centre(wxBOTH);

  SetTopWindow(pMainframe);
#ifndef __WXMAC__
  pMainframe->Show(true);
#endif //ndef __WXMAC__
	return true;
}

int VisualLitesqlApp::OnExit(void)
{
    delete m_docManager;
    return 0;
}

/*
 * Centralised code for creating a document frame.
 * Called from view.cpp, when a view is created.
 */
 
wxMDIChildFrame *VisualLitesqlApp::CreateChildFrame(wxDocument *doc, wxView *view)
{
  //// Make a child frame
  wxDocMDIChildFrame *subframe =
      new wxDocMDIChildFrame(doc, view, pMainframe, wxID_ANY, _T("Child Frame"),
                             wxPoint(10, 10), wxSize(500, 500),
                             wxDEFAULT_FRAME_STYLE |
                             wxNO_FULL_REPAINT_ON_RESIZE);

#ifdef __WXMSW__
  subframe->SetIcon(wxString( _T("notepad")));
#endif
#ifdef __X__
  subframe->SetIcon(wxIcon(_T("doc.xbm")));
#endif

  //// Make a menubar
  wxMenu *file_menu = new wxMenu;

  file_menu->Append(wxID_NEW, _T("&New..."));
  file_menu->Append(wxID_OPEN, _T("&Open..."));
  file_menu->Append(wxID_CLOSE, _T("&Close"));
  file_menu->Append(wxID_SAVE, _T("&Save"));
  file_menu->Append(wxID_SAVEAS, _T("Save &As..."));
  file_menu->AppendSeparator();
  file_menu->Append(ID_GENERATE, _T("Generate ..."));

  
  if (false /*isCanvas*/)
  {
    file_menu->AppendSeparator();
    file_menu->Append(wxID_PRINT, _T("&Print..."));
    file_menu->Append(wxID_PRINT_SETUP, _T("Print &Setup..."));
    file_menu->Append(wxID_PREVIEW, _T("Print Pre&view"));
  }

  file_menu->AppendSeparator();
  file_menu->Append(wxID_EXIT, _T("E&xit"));

  wxMenu *edit_menu = (wxMenu *) NULL;

    edit_menu = new wxMenu;
  //  edit_menu->Append(wxID_UNDO, _T("&Undo"));
  //  edit_menu->Append(wxID_REDO, _T("&Redo"));
  //  edit_menu->AppendSeparator();
  //  edit_menu->Append(ID_GENERATE, _T("Generate ..."));

    doc->GetCommandProcessor()->SetEditMenu(edit_menu);

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(ID_ABOUT, _T("&About"));

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, _T("&File"));
  if (false /* isCanvas */ )
    menu_bar->Append(edit_menu, _T("&Edit"));
  menu_bar->Append(help_menu, _T("&Help"));

  //// Associate the menu bar with the frame
  subframe->SetMenuBar(menu_bar);

  return subframe;
}

void VisualLitesqlApp::info(const wxString& message)
{
  //Log entry(*pDatabase);
  //DateTime now;
  //entry.createdAt = now; 
  //entry.severity = Log::Severity::INFO;
  //entry.message= message.ToUTF8().data();
  //entry.update();
}

void VisualLitesqlApp::warn(const wxString& message)
{
  //Log entry(*pDatabase);
  //DateTime now;
  //entry.createdAt = now; 
  //entry.severity = Log::Severity::INFO;
  //entry.message = message.ToUTF8().data();
  //entry.update();
}

void VisualLitesqlApp::error(const wxString& message)
{
  //Log entry(*pDatabase);
  //DateTime now;
  //entry.createdAt = now; 
  //entry.severity = Log::Severity::INFO;
  //entry.message = message.ToUTF8().data();
  //entry.update();
}
