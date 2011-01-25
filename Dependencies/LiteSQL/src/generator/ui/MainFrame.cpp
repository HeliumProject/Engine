#include <wx/cmdproc.h>
#include <wx/msgdlg.h>
#include <wx/menu.h>

#include "objectmodel.hpp"

#include "MainFrame.h"
#include "VisualLitesqlApp.h"
#include "LitesqlDocument.h"
#include "GenerateView.h"

#include "ui.h"

IMPLEMENT_CLASS(MainFrame, wxDocMDIParentFrame)
BEGIN_EVENT_TABLE(MainFrame, wxDocMDIParentFrame)
    EVT_MENU(VisualLitesqlApp::ID_ABOUT, MainFrame::OnAbout)
END_EVENT_TABLE()

MainFrame::MainFrame(wxDocManager *manager, wxFrame *frame, const wxString& title,
    const wxPoint& pos, const wxSize& size, long type):
  wxDocMDIParentFrame(manager, frame, wxID_ANY, title, pos, size, type, _T(LITESQL_L("myFrame")))
{
  editMenu = (wxMenu *) NULL;
  //// Make a menubar
  wxMenu *file_menu = new wxMenu;
  wxMenu *edit_menu = (wxMenu *) NULL;

  file_menu->Append(wxID_NEW, _T(LITESQL_L("&New...\tCtrl-N")));
  file_menu->Append(wxID_OPEN, _T(LITESQL_L("&Open...\tCtrl-X")));

  file_menu->AppendSeparator();
  file_menu->Append(wxID_EXIT, _T(LITESQL_L("E&xit\tAlt-X")));
  
  // A nice touch: a history of files visited. Use this menu.
  m_docManager->FileHistoryUseMenu(file_menu);

  wxMenu *help_menu = new wxMenu;
  help_menu->Append(VisualLitesqlApp::ID_ABOUT, _T(LITESQL_L("&About\tF1")));

  wxMenuBar *menu_bar = new wxMenuBar;

  menu_bar->Append(file_menu, _T(LITESQL_L("&File")));
  if (edit_menu)
    menu_bar->Append(edit_menu, _T(LITESQL_L("&Edit")));
  menu_bar->Append(help_menu, _T(LITESQL_L("&Help")));

  SetMenuBar(menu_bar);
}

MainFrame::~MainFrame()
{}

void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
  ui::AboutDialog dlg(this);
  dlg.ShowModal();
}
