#include "Precompiled.h"
#include "MainFrame.h"
#include "DocumentPanel.h"

#include "UIToolKit/FileDialog.h"

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_MENU(wxID_NEW, MainFrame::OnCommand)
EVT_MENU(wxID_OPEN, MainFrame::OnCommand)
EVT_MENU(wxID_SAVE, MainFrame::OnCommand)
EVT_MENU(wxID_CLOSE, MainFrame::OnCommand)
EVT_MENU(wxID_EXIT, MainFrame::OnCommand)
EVT_CLOSE(MainFrame::OnClose)
END_EVENT_TABLE()

MainFrame::MainFrame(wxWindow *parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: wxFrame (parent, id, title, pos, size, style, name)
, m_Notebook (NULL)
, m_FileMenu (NULL)
, m_EditMenu (NULL)
, m_ToolBar (NULL)
{
  CreateStatusBar();
  GetStatusBar()->SetStatusText(_("Ready"));

  wxMenuBar* menuBar = new wxMenuBar();

  // file menu
  {
    m_FileMenu = new wxMenu();
    m_FileMenu->Append( wxID_NEW, "New" );
    m_FileMenu->Append( wxID_OPEN, "Open..." );
    m_FileMenu->AppendSeparator();
    m_FileMenu->Append( wxID_SAVE, "Save" );
    m_FileMenu->Append( wxID_SAVEAS, "Save As..." );
    m_FileMenu->AppendSeparator();
    m_FileMenu->Append( wxID_CLOSE, "Close" );
    m_FileMenu->AppendSeparator();
    m_FileMenu->Append( wxID_EXIT, "Exit" );
    menuBar->Append( m_FileMenu, "File" );
  }

  // edit menu
  {
    m_EditMenu = new wxMenu();
    m_EditMenu->Append( wxID_UNDO, "Undo" );
    m_EditMenu->Append( wxID_REDO, "Redo" );
    m_EditMenu->AppendSeparator();
    m_EditMenu->Append( wxID_CUT, "Cut" );
    m_EditMenu->Append( wxID_COPY, "Copy" );
    m_EditMenu->Append( wxID_PASTE, "Paste" );
    menuBar->Append( m_EditMenu, "Edit" );
  }

  // menus complete
  SetMenuBar( menuBar );

  // toolbar
  m_ToolBar = new wxToolBar( this, -1, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER | wxTB_HORZ_TEXT );
  m_ToolBar->SetToolBitmapSize( wxSize( 16,16 ) );
  m_ToolBar->AddTool( wxID_NEW, wxT( "New" ), wxArtProvider::GetBitmap( wxART_NEW, wxART_OTHER, wxSize( 16, 16 ) ) );
  m_ToolBar->AddTool( wxID_OPEN, wxT( "Open" ), wxArtProvider::GetBitmap( wxART_FILE_OPEN, wxART_OTHER, wxSize( 16, 16 ) ) );
  m_ToolBar->AddTool( wxID_SAVE, wxT( "Save" ), wxArtProvider::GetBitmap( wxART_FILE_SAVE, wxART_OTHER, wxSize( 16, 16 ) ) );
  m_ToolBar->AddSeparator();
  m_ToolBar->AddTool( wxID_UNDO, wxT( "Undo" ), wxArtProvider::GetBitmap( wxART_UNDO, wxART_OTHER, wxSize(16, 16 ) ) );
  m_ToolBar->AddTool( wxID_REDO, wxT( "Redo" ), wxArtProvider::GetBitmap( wxART_REDO, wxART_OTHER, wxSize(16, 16 ) ) );
  m_ToolBar->AddSeparator();
  m_ToolBar->AddTool( wxID_CUT, wxT( "Cut" ), wxArtProvider::GetBitmap( wxART_CUT, wxART_OTHER, wxSize( 16, 16 ) ) );
  m_ToolBar->AddTool( wxID_COPY, wxT( "Copy" ), wxArtProvider::GetBitmap( wxART_COPY, wxART_OTHER, wxSize(16, 16 ) ) );
  m_ToolBar->AddTool( wxID_PASTE, wxT( "Paste" ), wxArtProvider::GetBitmap( wxART_PASTE, wxART_OTHER, wxSize(16, 16 ) ) );
  m_ToolBar->Realize();

  m_Notebook = new wxAuiNotebook( this );

  m_FrameManager.SetManagedWindow( this ); 
  m_FrameManager.AddPane( m_Notebook, wxAuiPaneInfo().Name( wxT( "notebook" ) ).DestroyOnClose( false ).Caption( wxT( "Document Notebook" ) ).CenterPane() );
  m_FrameManager.AddPane( m_ToolBar, wxAuiPaneInfo().Name( wxT( "standardtoolbar" ) ).DestroyOnClose( false ).Caption( wxT( "Standard Toolbar" ) ).ToolbarPane().Top().LeftDockable( false ).RightDockable( false ) );
  m_FrameManager.Update();

  // connect listeners
  Document::GetRequestOpenFileEvent().Add( FileSignature::Delegate( this, &MainFrame::OnRequestOpenFile ) );
  Document::GetRequestSaveFileEvent().Add( FileSignature::Delegate( this, &MainFrame::OnRequestSaveFile ) );

  m_Manager.GetDocumentAddedEvent().Add( DocumentSignature::Delegate ( this, &MainFrame::OnDocumentAdded ) );
  m_Manager.GetDocumentRemovedEvent().Add( DocumentSignature::Delegate ( this, &MainFrame::OnDocumentRemoved ) );
}

MainFrame::~MainFrame()
{
  // disconnect listeners
  m_Manager.GetDocumentRemovedEvent().Remove( DocumentSignature::Delegate ( this, &MainFrame::OnDocumentRemoved ) );
  m_Manager.GetDocumentAddedEvent().Remove( DocumentSignature::Delegate ( this, &MainFrame::OnDocumentAdded ) );

  Document::GetRequestSaveFileEvent().Remove( FileSignature::Delegate( this, &MainFrame::OnRequestSaveFile ) );
  Document::GetRequestOpenFileEvent().Remove( FileSignature::Delegate( this, &MainFrame::OnRequestOpenFile ) );

  m_FrameManager.UnInit();
}

void MainFrame::OnCommand(wxCommandEvent& event)
{
  switch ( event.GetId() )
  {
  case wxID_NEW:      m_Manager.New(); break;
  case wxID_OPEN:     m_Manager.Open(); break;
  case wxID_SAVE:     m_Manager.Save(); break;
  case wxID_SAVEAS:   m_Manager.SaveAs(); break;
  case wxID_CLOSE:    m_Manager.Close(); break;
  case wxID_EXIT:
    {
      Close();
    };
  }
}

void MainFrame::OnClose(wxCloseEvent& event)
{
  event.Veto( !m_Manager.CloseAll() );

  event.Skip(); // do base class logic to actually do the close handling
}

void MainFrame::OnDocumentAdded(const DocumentArgs& args)
{
  DocumentPanel* panel = new DocumentPanel ( m_Notebook );

  panel->SetDocument( args.m_Document );

  m_Notebook->AddPage( panel, args.m_Document->GetTitle() );
}

void MainFrame::OnDocumentRemoved(const DocumentArgs& args)
{
  for ( size_t index = 0; index < m_Notebook->GetPageCount(); index ++ )
  {
    DocumentPanel* panel = static_cast<DocumentPanel*>( m_Notebook->GetPage(index) );

    if ( panel->GetDocument() == args.m_Document )
    {
      m_Notebook->DeletePage( index );
      break;
    }
  }
}

static const char* FilterString = "Reflect Files (*.rb,*.rx,*.xml)|*.rb;*.rx;*.xml";

void MainFrame::OnRequestOpenFile(FileArgs& args)
{
  UIToolKit::FileDialog dialog ( this, "Open", wxEmptyString, wxEmptyString, FilterString, UIToolKit::FileDialogStyles::DefaultOpen );

  if ( wxID_OK == dialog.ShowModal() )
  {
    args.m_File = dialog.GetFilePath();
  }
}

void MainFrame::OnRequestSaveFile(FileArgs& args)
{
  UIToolKit::FileDialog dialog ( this, "Save", wxEmptyString, wxEmptyString, FilterString, UIToolKit::FileDialogStyles::DefaultSave );

  if ( wxID_OK == dialog.ShowModal() )
  {
    args.m_File = dialog.GetFilePath();
  }
}