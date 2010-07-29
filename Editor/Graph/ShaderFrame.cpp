#include "Precompile.h"
#include "Graph/ShaderFrame.h"

#include <wx/config.h>
#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/wfstream.h>
#include <wx/aboutdlg.h>
#include <wx/clipbrd.h>

#include "Graph/LuaUtilities.h"
#include "Graph/ProjectNotebook.h"
#include "Graph/MenuState.h"
#include "Graph/Debug.h"

#include "Graph/XPM/copy.xpm"
#include "Graph/XPM/cut.xpm"
#include "Graph/XPM/new.xpm"
#include "Graph/XPM/open.xpm"
#include "Graph/XPM/paste.xpm"
#include "Graph/XPM/save.xpm"
#include "Graph/XPM/x.xpm"

BEGIN_EVENT_TABLE(Shader, wxFrame)
EVT_CLOSE(Shader::OnClose)
EVT_COMMAND_RANGE(wxID_TYPES, wxID_TYPES + 99, wxEVT_COMMAND_MENU_SELECTED, Shader::OnNew)
EVT_COMMAND_RANGE(wxID_RECENT + 1, wxID_RECENT + 20, wxEVT_COMMAND_MENU_SELECTED, Shader::OnRecent)
EVT_COMMAND_RANGE(wxID_VIEWERS, wxID_VIEWERS + 99, wxEVT_COMMAND_MENU_SELECTED, Shader::OnViewer)
EVT_COMMAND(wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED, Shader::OnAbout)
EVT_MENU(wxID_ANY, Shader::OnCommand)
EVT_TIMER(wxID_ANY, Shader::OnTimer)
END_EVENT_TABLE()

Shader::Shader()
: wxFrame(NULL, wxID_ANY, wxT("FragmentShader"))
{
    BuildMenu();
    AddStatusBar();
    LoadConfig();
    m_Timer.SetOwner(this);
    m_Timer.Start(1000, wxTIMER_CONTINUOUS);
}

Shader::~Shader()
{
    SaveConfig();
}

static void
AppendItem(wxMenu *menu, int id, char **xpm, const wxString& text = wxT(""), wxMenu *submenu = NULL)
{
    wxMenuItem *item = NEW(wxMenuItem, (menu, id, text, wxT(""), wxITEM_NORMAL, submenu));
    item->SetBitmap(wxBitmap(xpm));
    menu->Append(item);
}

void
Shader::BuildMenu()
{
    // --------------------------------------------------
    // The main menu,
    m_MenuBar = NEW(wxMenuBar, (0));

    // --------------------------------------------------
    // The list of available projects.
    m_ProjectsMenu = NEW(wxMenu, ());
    wxArrayString types = Project::GetProjectTypes();
    for (unsigned int index = 0; index < types.GetCount(); index++)
    {
        m_ProjectsMenu->Append(wxID_TYPES + index, types[index]);
    }
    if (m_ProjectsMenu->GetMenuItemCount() == 0)
    {
        m_ProjectsMenu->Append(wxID_TYPES, wxT("Empty"));
        m_ProjectsMenu->Enable(wxID_TYPES, false);
    }

    // --------------------------------------------------
    // The list of recent projects.
    m_RecentMenu = NEW(RecentMenu, (wxID_RECENT + 1, wxID_RECENT + 20));

    // --------------------------------------------------
    // The file menu.
    m_FileMenu = NEW(wxMenu, ());
    AppendItem(m_FileMenu, wxID_NEW, new_xpm, wxT(""), m_ProjectsMenu);
    AppendItem(m_FileMenu, wxID_OPEN, open_xpm);
    m_FileMenu->Append(wxID_RECENT, wxT("Recent projects\tCtrl-R"), m_RecentMenu);
    m_FileMenu->AppendSeparator();
    AppendItem(m_FileMenu, wxID_SAVE, save_xpm);
    m_FileMenu->Append(wxID_SAVEAS, wxT("Save as...\tCtrl-A"));
    m_FileMenu->AppendSeparator();
    AppendItem(m_FileMenu, wxID_EXIT, x_xpm, wxT("Quit\tCtrl-Q"));
    // Disable some options.
    m_FileMenu->Enable(wxID_SAVE, false);
    m_FileMenu->Enable(wxID_SAVEAS, false);
    m_MenuBar->Append(m_FileMenu, wxT("&File"));

    // --------------------------------------------------
    // The edit menu.
    m_EditMenu = NEW(wxMenu, ());
    m_EditMenu->Append(wxID_UNDO, wxT("&Undo\tCtrl-Z"));
    m_EditMenu->Append(wxID_REDO, wxT("&Redo\tCtrl-Y"));
    m_EditMenu->AppendSeparator();
    AppendItem(m_EditMenu, wxID_CUT, cut_xpm);
    AppendItem(m_EditMenu, wxID_COPY, copy_xpm);
    AppendItem(m_EditMenu, wxID_PASTE, paste_xpm);
    m_EditMenu->Append(wxID_DUPLICATE, wxT("&Duplicate\tCtrl-D"));
    m_EditMenu->AppendSeparator();
    m_EditMenu->Append(wxID_GROUP, wxT("Group\tCtrl-G"));
    m_EditMenu->Append(wxID_UNGROUP, wxT("Ungroup\tCtrl-U"));
    m_EditMenu->Append(wxID_SAVEGROUP, wxT("Save group...\tCtrl-H"));
    m_EditMenu->AppendSeparator();
    m_EditMenu->Append(wxID_COPYMETAFILE, wxT("Copy as &metafile\tCtrl-M"));
    m_EditMenu->AppendSeparator();
    m_EditMenu->Append(wxID_CONFIGURE, wxT("Project configuration...\tCtrl-P"));
    // Disable some options.
    m_EditMenu->Enable(wxID_UNDO, false);
    m_EditMenu->Enable(wxID_REDO, false);
    m_EditMenu->Enable(wxID_CUT, false);
    m_EditMenu->Enable(wxID_COPY, false);
    m_EditMenu->Enable(wxID_PASTE, false);
    m_EditMenu->Enable(wxID_DUPLICATE, false);
    m_EditMenu->Enable(wxID_GROUP, false);
    m_EditMenu->Enable(wxID_UNGROUP, false);
    //m_EditMenu->Enable(wxID_COPYMETAFILE, false);
    m_MenuBar->Append(m_EditMenu, wxT("&Edit"));

    // --------------------------------------------------
    // The tools menu.
    m_ToolsMenu = NEW(wxMenu, ());
    m_ToolsMenu->Append(wxID_RELOADLIBS, wxT("Reload libraries\tCtrl-L"));
    // Disable some options.
    m_ToolsMenu->Enable(wxID_RELOADLIBS, false);
    m_MenuBar->Append(m_ToolsMenu, wxT("&Tools"));

    // --------------------------------------------------
    // The viewers menu.
    //m_ViewersMenu = NEW(wxMenu, ());
    //m_MenuBar->Append(m_ViewersMenu, wxT("&Viewers"));

    // --------------------------------------------------
    // The help menu.
    m_HelpMenu = NEW(wxMenu, ());
    m_HelpMenu->Append(wxID_ABOUT, wxT("About...\tF1"));
    m_MenuBar->Append(m_HelpMenu, wxT("&Help"));

    // --------------------------------------------------
    // Set the menu.
    this->SetMenuBar(m_MenuBar);
}

void
Shader::AddStatusBar()
{
    m_StatusBar = NEW(wxStatusBar, (this, wxID_ANY, wxST_SIZEGRIP));
    this->SetStatusBar(m_StatusBar);
}

void
Shader::NewProject(const wxString& type)
{
    MenuState *ms = NEW(MenuState, (m_MenuBar));
    ms->Add(wxID_UNDO);
    ms->Add(wxID_REDO);
    ms->Add(wxID_CUT);
    ms->Add(wxID_COPY);
    //ms->Add(wxID_PASTE);
    ms->Add(wxID_DUPLICATE);
    ms->Add(wxID_GROUP);
    ms->Add(wxID_UNGROUP);
    ms->Add(wxID_SAVEGROUP);
    ms->Get();
    Project::New(this, ms, type);
}

void
Shader::Open(const wxString& file)
{
    MenuState *ms = NEW(MenuState, (m_MenuBar));
    ms->Add(wxID_UNDO);
    ms->Add(wxID_REDO);
    ms->Add(wxID_CUT);
    ms->Add(wxID_COPY);
    //ms->Add(wxID_PASTE);
    ms->Add(wxID_DUPLICATE);
    ms->Add(wxID_GROUP);
    ms->Add(wxID_UNGROUP);
    ms->Add(wxID_SAVEGROUP);
    ms->Get();

    m_FileName = file;
    wxFileName fn(m_FileName);
    m_FilePath = fn.GetPath();	
    Project::Load(this, ms, m_FileName);
    m_RecentMenu->Put(m_FileName);
}

int
Shader::Compile(int index, const wxString& output)
{
    return Project::GetProject()->Compile(index, output);
}

void
Shader::Open()
{
    if (ConfirmSave() != wxID_CANCEL)
    {
        wxFileDialog dialog(this, wxT("Open file"), m_FilePath, wxT(""), wxT("Shader files (*.graphshader)|*.graphshader|All files (*.*)|*.*"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if (dialog.ShowModal() == wxID_OK)
        {
            Open(dialog.GetPath());
        }
    }
}

int
Shader::Save()
{
    if (Project::GetProject() != 0)
    {
        if (m_FileName.IsEmpty())
        {
            return SaveAs();
        }
        Project::GetProject()->Save(m_FileName);
    }
    return wxID_YES;
}

int
Shader::SaveAs()
{
    wxFileDialog dialog(this, wxT("Save as"), m_FilePath, m_FileName, wxT("Shader files (*.graphshader)|*.graphshader"), wxFD_SAVE | wxFD_CHANGE_DIR | wxFD_OVERWRITE_PROMPT);
    int choice = dialog.ShowModal();

    if (choice == wxID_OK)
    {
        m_FileName = dialog.GetPath();
        wxFileName fn(m_FileName);
        m_FilePath = fn.GetPath();
        Project::GetProject()->Save(m_FileName);
        m_RecentMenu->Put(m_FileName);
    }

    return choice;
}

int
Shader::ConfirmSave()
{
    Project* project  = Project::GetProject();
    bool     modified = false;

    if (project != 0)
    {
        int count = (int)project->GetPageCount();
        for (int index = 0; index < count; index++)
        {
            Panel*  panel = (Panel *)project->GetPage(index);
            modified      = modified || panel->GetUndoerCtrl()->IsDirty();
        }
    }

    if (modified)
    {
        wxString message = wxT("Save changes to ");
        if (m_FileName.IsEmpty())
        {
            message.Append(wxT("(untitled)"));
        }
        else
        {
            message.Append(m_FileName);
        }
        message.Append('?');
        wxMessageDialog dialog(this, message, wxT("Confirm"), wxYES_NO | wxCANCEL);
        int choice = dialog.ShowModal();
        if (choice == wxID_YES)
        {
            return Save();
        }
        return choice;
    }
    return wxID_YES;
}

void
Shader::EnableMenu(const wxString& label, bool enable)
{
    int slash = label.Find('/');
    if (slash != wxNOT_FOUND)
    {
        wxString first = label.Left(slash);
        wxString second = label.Mid(slash + 1);
        int id = m_MenuBar->FindMenuItem(first, second);
        if (id != wxNOT_FOUND)
        {
            m_MenuBar->Enable(id, enable);
        }
    }
    else
    {
        int pos = m_MenuBar->FindMenu(label);
        if (pos != wxNOT_FOUND)
        {
            m_MenuBar->EnableTop(pos, enable);
        }
    }
}

void
Shader::OnTimer(wxTimerEvent& evt)
{
    wxString title = wxT("igFragmentShader - ");
    if (m_FileName.IsEmpty())
    {
        title.Append(wxT("(untitled)"));
    }
    else
    {
        title.append(wxFileName::FileName(m_FileName).GetFullName());
    }
    if (Project::GetProject() != 0)
    {
        m_MenuBar->Enable(wxID_SAVEAS, true);
        m_MenuBar->Enable(wxID_SAVE, true);
    }
    else
    {
        m_MenuBar->Enable(wxID_SAVEAS, false);
        m_MenuBar->Enable(wxID_SAVE, false);
    }
    SetTitle(title);
    m_MenuBar->Enable(wxID_PASTE, wxTheClipboard->IsSupported(wxDataFormat(wxDF_TEXT)));
}

void
Shader::OnClose(wxCloseEvent& evt)
{
    SaveConfig();
    if (evt.CanVeto())
    {
        if (ConfirmSave() != wxID_CANCEL)
        {
            goto destroy;
        }
        else
        {
            evt.Veto();
        }
    }
    else
    {
        Save();
destroy:
        Project::Destroy();
        Destroy();
    }
}

void
Shader::OnNew(wxCommandEvent& evt)
{
    if (ConfirmSave() != wxID_CANCEL)
    {
        NewProject(m_ProjectsMenu->GetLabelText(evt.GetId()));
        m_FileName.Empty();
    }
}

void
Shader::OnRecent(wxCommandEvent& evt)
{
    wxString file;
    if (m_RecentMenu->Get(evt.GetId(), file) && ConfirmSave() != wxID_CANCEL)
    {
        Open(file);
    }
}

void
Shader::OnViewer(wxCommandEvent& evt)
{
    //project->OnViewer(evt.GetId() - wxID_VIEWERS);
}

void
Shader::OnAbout(wxCommandEvent& evt)
{
    wxAboutDialogInfo info;
    info.SetName(wxT("igFragmentShader"));
    //info.SetDescription(wxT("Built on " __TDATE__ " " __TTIME__));
    wxAboutBox(info);
}

void
Shader::OnCommand(wxCommandEvent& evt)
{
    Project *project = Project::GetProject();
    Panel *panel;
    Undoer *undoer;
    Graph *graph;
    if (project != NULL)
    {
        panel = project->GetPanel();
        if (panel != NULL)
        {
            undoer = panel->GetUndoerCtrl();
            graph = panel->GetGraphCtrl();
        }
    }
    switch (evt.GetId())
    {
    case wxID_OPEN:
        Open();
        break;
    case wxID_SAVE:
        Save();
        break;
    case wxID_SAVEAS:
        SaveAs();
        break;
    case wxID_EXIT:
        Close(false);
        break;
    case wxID_UNDO:
        undoer->Undo();
        break;
    case wxID_REDO:
        undoer->Redo();
        break;
    case wxID_CUT:
        graph->Cut();
        break;
    case wxID_COPY:
        graph->Copy();
        break;
    case wxID_PASTE:
        graph->Paste();
        break;
    case wxID_DUPLICATE:
        graph->Duplicate();
        break;
    case wxID_GROUP:
        graph->GroupNodes();
        break;
    case wxID_UNGROUP:
        graph->UngroupNodes();
        break;
    case wxID_SAVEGROUP:
        graph->SaveGroup();
        break;
    case wxID_COPYMETAFILE:
        graph->CopyAsMetafile();
        break;
    case wxID_CONFIGURE:
        project->Configure();
        break;
    case wxID_RELOADLIBS:
        //project->ReloadLibs();
        break;
    }
}

void
Shader::LoadConfig()
{
    wxConfigBase *config = wxConfigBase::Get();
    //config->DeleteAll();
    // Load the window position on the screen.
    wxRect pos = GetScreenRect();
    long value;
    bool ok = config->Read(wxT("main_x"), &value);
    pos.SetX(value);
    ok = ok && config->Read(wxT("main_y"), &value);
    pos.SetY(value);
    ok = ok && config->Read(wxT("main_width"), &value);
    pos.SetWidth(value);
    ok = ok && config->Read(wxT("main_height"), &value);
    pos.SetHeight(value);
    if (ok)
    {
        SetSize(pos);
    }
    // Check if the window was maximized.
    if (config->Read(wxT("main_maximized"), &value) && value != 0)
    {
        Maximize();
    }
    // Load the list of recent projects.
    m_RecentMenu->Load(*config, wxT("main_recent"));
}

void
Shader::SaveConfig()
{
    wxConfigBase *config = wxConfigBase::Get();
    wxRect pos = GetRect();
    // Save the window position on the screen.
    config->Write(wxT("main_x"), pos.GetX());
    config->Write(wxT("main_y"), pos.GetY());
    config->Write(wxT("main_width"), pos.GetWidth());
    config->Write(wxT("main_height"), pos.GetHeight());
    // Save 1 if the window is maximized.
    config->Write(wxT("main_maximized"), IsMaximized() ? 1 : 0);
    // Write the list of recent projects.
    m_RecentMenu->Save(*config, wxT("main_recent"));
}
