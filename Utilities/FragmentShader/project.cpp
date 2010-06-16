#include "project.h"

#include <wx/config.h>
#include <wx/dir.h>
#include <wx/tokenzr.h>
#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/xml/xml.h>

#include <map>

#include "shader.h"
#include "panel.h"
#include "luautil.h"
#include "stream.h"
#include "undo.h"
#include "main.h"
#include "xml.h"

#include "debug.h"

#include "Application/RCS/RCS.h"
#include "Foundation/Log.h"

static std::map<wxString, wxString> l_Types;
static Project *l_instance = NULL;

BEGIN_EVENT_TABLE(Project, wxNotebook)
	EVT_NOTEBOOK_PAGE_CHANGING(wxID_ANY, Project::OnPageChanging)
	EVT_NOTEBOOK_PAGE_CHANGED(wxID_ANY, Project::OnPageChanged)
END_EVENT_TABLE()

void
Project::New(wxWindow *parent, MenuState *state, const wxString& type)
{
	if (l_instance != NULL)
	{
		DESTROY(l_instance);
	}
	l_instance = NEW(Project, (parent, type));
	// Load the XML project description.
	Debug::Printf("Parsing \"%s\".\n", l_Types[type].c_str());
	wxXmlDocument xml;
	if (!xml.Load(l_Types[type]))
	{
		THROW("Couldn't parse the XML file.");
	}
	if (xml.GetRoot()->GetName() != wxT("project"))
	{
		THROW("Root node isn't <project>.");
	}

	//wxArrayString groups = NodeLib::AddUserNodes();
	//wxString grpfile(g_LibPath);
	//grpfile.Append(wxT("nodes\\user.xml"));
	//wxArrayString groups = NodeLib::Add(grpfile);

	wxXmlNode *child = xml.GetRoot()->GetChildren();
	while (child != 0)
	{
		if (child->GetType() == wxXML_COMMENT_NODE)
		{
			child = child->GetNext();
			continue;
		}
		if (child->GetName() == wxT("panel"))
		{
			wxString name;
			if (!child->GetPropVal(wxT("name"), &name))
			{
				THROW("Couldn't find attribute 'name' in element <panel>.");
			}
			Panel *panel = NEW(Panel, (l_instance, state->Clone()));
			panel->Freeze();
			l_instance->AddPage(panel, name);
			Debug::Printf("Panel \"%s\" (%p) created.\n", name.c_str(), panel);
			wxXmlNode *child2 = child->GetChildren();
      /*
			for (size_t i = 0; i < groups.GetCount(); i++)
			{
				panel->AddToTree(groups[i]);
				Debug::Printf("Group \"%s\" added to panel %p.\n", groups[i].c_str(), panel);
			}
      */
			while (child2 != 0)
			{
				if (child2->GetType() == wxXML_COMMENT_NODE)
				{
					child2 = child2->GetNext();
					continue;
				}
				if (child2->GetName() == wxT("add-library"))
				{
					wxString path;
					if (!child2->GetPropVal(wxT("path"), &path))
					{
						THROW("Couldn't find attribute 'path' in element <add-library>.");
					}
					panel->LoadLibrary(path);
					Debug::Printf("Library \"%s\" added to panel %p.\n", path, panel);
				}
				else if (child2->GetName() == wxT("add-node"))
				{
					wxString type;
					if (!child2->GetPropVal(wxT("type"), &type))
					{
						THROW("Couldn't find attribute 'type' in element <add-node>.");
					}
					wxString deletable = child2->GetPropVal(wxT("deletable"), wxT("false"));
					panel->AddNode(type, deletable == wxT("true"));
					Debug::Printf("Node \"%s\" added to panel %p.\n", type.c_str(), panel);
				}
				else
				{
					Debug::Printf("\tInvalid element <%s> in <panel>.\n", child2->GetName().c_str());
				}
				child2 = child2->GetNext();
			}
			panel->Thaw();
		}
		else
		{
			Debug::Printf("\tInvalid element <%s> in <project>.\n", child->GetName().c_str());
		}
		child = child->GetNext();
	}
	// Delete the menu state (it has been cloned for each panel.)
	DESTROY(state);
	// Load the panels configuration.
	l_instance->LoadConfig();
	// Clear Undo data of all panels.
	size_t count = l_instance->GetPageCount();
	for (size_t index = 0; index < count; index++)
	{
		Panel *panel = (Panel *)l_instance->GetPage(index);
		panel->GetUndoerCtrl()->Clear();
	}
}

void
Project::Load(wxWindow *parent, MenuState *menu, const wxString& filename)
{
	wxString type = GetProjectType(filename);
	New(parent, menu, type);
	wxFileInputStream fis(filename);
	int first = fis.Peek();
	if (first == 0x1b)
	{
		// Binary format.

		fis.GetC();
		LuaInputStream lis(&fis);
		l_instance->LoadBinary(lis);
	}
	else
	{
		// XML format.
		l_instance->LoadXML(fis);
	}
	// Clear Undo data of all panels.
	size_t count = l_instance->GetPageCount();
	for (size_t index = 0; index < count; index++)
	{
		Panel *panel = (Panel *)l_instance->GetPage(index);
		panel->GetUndoerCtrl()->Clear();
		panel->GetGraphCtrl()->Canvas::Redraw(true);
		panel->GetGraphCtrl()->GenerateCode();
	}
}

void
Project::Destroy()
{
	if (l_instance != NULL)
	{
		DESTROY(l_instance);
		l_instance = NULL;
	}
}

Panel *
Project::GetPanel() const
{
	if (GetPageCount() > 0)
	{
		return (Panel *)GetCurrentPage();
	}
	return NULL;
}

static wxWindow *
Find(wxWindow *parent, wxWindow *child)
{
	wxWindowList list = parent->GetChildren();
	wxWindowList::iterator it;
	for (it = list.begin(); it != list.end(); ++it)
	{
		if (*it == child)
		{
			return parent;
		}
		wxWindow *found = Find(*it, child);
		if (found != NULL)
		{
			return found;
		}
	}
	return NULL;
}

Panel *
Project::GetPanel(wxWindow *child) const
{
	for (size_t i = 0; i < GetPageCount(); i++)
	{
		Panel *panel = (Panel *)GetPage(i);
		if (Find(panel, child) != NULL)
		{
			return panel;
		}
	}
	return NULL;
}

Project::~Project()
{
	SaveConfig();
	NodeLib::Clear();
}

Project *
Project::GetProject()
{
	return l_instance;
}

void
Project::Configure()
{
	wxPGChoices choices;
	choices.Add(wxT("GLSL"));
	choices.Add(wxT("Cg"));
	choices.Add(wxT("HLSL"));
	wxEnumProperty *prop = NEW(wxEnumProperty, (wxT("Target"), wxPG_LABEL, choices));
	prop->SetValue(m_target);

	wxDialog config( GetParent(), wxID_ANY, wxT("Project configuration"), wxDefaultPosition, wxSize( 384,384 ), wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
	config.SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* dialog_sizer;
	dialog_sizer = NEW(wxBoxSizer, ( wxVERTICAL ));
	
	wxBoxSizer* props_sizer;
	props_sizer = NEW( wxBoxSizer, ( wxVERTICAL ));
	
	props_sizer->SetMinSize( wxSize( 256,256 ) ); 
	wxPropertyGrid *props = NEW(wxPropertyGrid, (&config));
	props->Append(prop);
	props_sizer->Add( props, 1, wxEXPAND | wxALL, 5 );
	
	dialog_sizer->Add( props_sizer, 1, wxEXPAND, 5 );
	
	wxBoxSizer* buttons_sizer;
	buttons_sizer = NEW(wxBoxSizer, ( wxHORIZONTAL ));
	
	wxButton *save = NEW(wxButton, ( &config, wxID_OK, wxT("Save"), wxDefaultPosition, wxDefaultSize, 0 ));
	save->SetDefault(); 
	buttons_sizer->Add( save, 0, wxALL, 5 );
	
	wxButton *cancel = NEW(wxButton, ( &config, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 ));
	buttons_sizer->Add( cancel, 0, wxALL, 5 );
	
	dialog_sizer->Add( buttons_sizer, 0, wxALIGN_RIGHT, 5 );
	
	config.SetSizer( dialog_sizer );
	config.Layout();

	if (config.ShowModal() == wxID_OK)
	{
		m_target = prop->GetValue().GetString();
	}
}

void
Project::Save(const wxString& filename)
{
	wxXmlNode *root = NEW(wxXmlNode, (wxXML_ELEMENT_NODE, wxT("project")));
	root->AddProperty(wxT("name"), m_type);
	root->AddProperty(wxT("target"), m_target);
	size_t count = GetPageCount();
	for (size_t index = 0; index < count; index++)
	{
		wxXmlNode *panel = NEW(wxXmlNode, (wxXML_ELEMENT_NODE, wxT("panel")));
		panel->AddProperty(wxT("name"), GetPageText(index));
		wxXmlNode *graph = static_cast<Panel *>(GetPage(index))->GetGraphCtrl()->Serialize();
		panel->AddChild(graph);
		root->AddChild(panel);
	}

  const  std::string file = filename.c_str();

  // Checkout the files from revision control if necessary.
  if ( RCS::PathIsManaged( file ) )
  {
    try 
    {
      RCS::File rcsFile( file );
      rcsFile.Open();
    }
    catch ( const Nocturnal::Exception& e )
    {
      Log::Error("Error Saving Graphshader <%s>: %s", filename.c_str(), e.what());
      return;
    }
  }
	wxXmlDocument doc;
	doc.SetRoot(root);
	doc.Save(filename);
}

int
Project::Compile(int index, const wxString& output)
{
	if (index >= (int)GetPageCount())
	{
		return CompileInvalidPage;
	}
	Panel *panel = (Panel *)GetPage(index);
	return panel->GetGraphCtrl()->Compile(output) ? CompileSuccess : CompileError;
}

wxString
Project::GetProjectType(const wxString& filename)
{
	wxFileInputStream fis(filename);
	int first = fis.Peek();
	if (first == 0x1b)
	{
		// Binary format.
		fis.GetC();
		LuaInputStream lis(&fis);
		lis.Read();
		lis.Read();
		wxString type = lis.Read();
		fis.SeekI(0);
		return type;
	}
	else
	{
		// XML format.
		wxXmlDocument doc(fis);
		wxXmlNode *root = doc.GetRoot();
		return root->GetPropVal(wxT("name"), wxT(""));
	}
}

static int
CompareStrings(const wxString& first, const wxString& second)
{
	return first.CmpNoCase(second);
}

const wxArrayString
Project::GetProjectTypes()
{
	wxArrayString types;
	wxString path(g_LibPath);
	path.Append("projects\\");
	wxDir dir(path);
	wxString entry;
	bool cont = dir.GetFirst(&entry, wxT("*.xml"), wxDIR_FILES);
	while (cont)
	{
		wxString filename(path);
		filename.Append(entry);
		Debug::Printf("Found project: %s\n", filename.c_str());
		wxXmlDocument xml;
		if (xml.Load(filename))
		{
			wxXmlNode *root = xml.GetRoot();
			if (root->GetName() == wxT("project"))
			{
				wxString name;
				if (root->GetPropVal(wxT("name"), &name))
				{
					types.Add(name);
					l_Types.insert(std::pair<wxString, wxString>(name, filename));
					Debug::Printf("\tProject name is \"%s\"\n", name.c_str());
				}
				else
				{
					Debug::Printf("\tAttribute \"name\" not found in <project>.\n");
				}
			}
			else
			{
				Debug::Printf("\tRoot node isn't <project>.\n");
			}
		}
		else
		{
			Debug::Printf("\tError parsing file.\n");
		}
		cont = dir.GetNext(&entry);
	}
	types.Sort(CompareStrings);
	return types;
}

void
Project::LoadConfig()
{
	Panel *panel = (Panel *)GetCurrentPage();
	if (panel != NULL)
	{
		wxConfigBase *config = wxConfigBase::Get();
		Panel::Layout layout;
		long value;
		config->Read(wxT("vsplitter_sash"), &value);
		layout.vsash = value;
		config->Read(wxT("hsplitter1_sash"), &value);
		layout.hsash1 = value;
		config->Read(wxT("hsplitter2_sash"), &value);
		layout.hsash2 = value;
		panel->SetLayout(layout);
	}
}

void
Project::SaveConfig()
{
	Panel *panel = (Panel *)GetCurrentPage();
	if (panel != NULL)
	{
		wxConfigBase *config = wxConfigBase::Get();
		Panel::Layout layout = panel->GetLayout();
		config->Write(wxT("vsplitter_sash"), layout.vsash);
		config->Write(wxT("hsplitter1_sash"), layout.hsash1);
		config->Write(wxT("hsplitter2_sash"), layout.hsash2);
	}
}

void
Project::OnPageChanging(wxNotebookEvent& evt)
{
	size_t old = evt.GetOldSelection();
	size_t pages = GetPageCount();
	if (old >= 0 && old < pages)
	{
		Panel *panel = (Panel *)GetPage(old);
		Panel::Layout layout = panel->GetLayout();
		for (size_t page = 0; page < pages; page++)
		{
			if (page != old)
			{
				panel = (Panel *)GetPage(page);
				panel->SetLayout(layout);
			}
		}
	}
}

void
Project::OnPageChanged(wxNotebookEvent& evt)
{
	if (evt.GetEventObject() == this)
	{
		int old = evt.GetOldSelection();
		int current = evt.GetSelection();
		if (old != -1 && current != -1)
		{
			Panel *panel = (Panel *)GetPage(old);
			panel->GetMenu();
			panel = (Panel *)GetPage(current);
			panel->SetMenu();
		}
	}
	evt.Skip();
}

Project::Project(wxWindow *parent, const wxString& type)
	: wxNotebook(parent, wxID_ANY)
	, m_type(type)
	, m_target(wxT("GLSL"))
{
	SetInitialSize(parent->GetClientSize());
}

void
Project::LoadBase64(wxInputStream& is)
{
}

void
Project::LoadBinary(LuaInputStream& lis)
{
	int major = lis.ReadInteger();
	int minor = lis.ReadInteger();
	if (major == 2 && minor == 0)
	{
		LoadBinary20(lis);
	}
	else if (major == 2 && minor == 1)
	{
		LoadBinary21(lis);
	}
	else
	{
		THROW("Unknown binary file version %d.%d.", major, minor);
	}
}

void
Project::LoadBinary20(LuaInputStream& lis)
{
	lis.Read(); // Skip project type.
	int panel_cnt = lis.ReadInteger();
	if (panel_cnt != GetPageCount())
	{
		THROW("Could not read from file, number of graphs mismatch.");
	}
	for (int i = 0; i < panel_cnt; i++)
	{
		Panel *panel = (Panel *)GetPage(i);
		panel->GetGraphCtrl()->Clear();
		panel->LoadBinary20(lis);
	}
}

void
Project::LoadBinary21(LuaInputStream& lis)
{
	lis.Read(); // Skip project type.
	int panel_cnt = lis.ReadInteger();
	if (panel_cnt != GetPageCount())
	{
		THROW("Could not read from file.");
	}
	for (int i = 0; i < panel_cnt; i++)
	{
		Panel *panel = (Panel *)GetPage(i);
		panel->GetGraphCtrl()->Clear();
		panel->LoadBinary21(lis);
	}
}

void
Project::LoadXML(wxInputStream& is)
{
	wxXmlDocument doc(is);
	wxXmlNode *root = doc.GetRoot();
	wxString m_target = XML::GetStringAttribute(*root, wxT("target"), wxT("GLSL"));
	size_t count = GetPageCount();
	for (size_t index = 0; index < count; index++)
	{
		Panel *panel = static_cast<Panel *>(GetPage(index));
		panel->GetGraphCtrl()->Clear();
		wxString name = GetPageText(index);
		wxXmlNode *child = root->GetChildren();
		while (child != NULL)
		{
			if (child->GetPropVal(wxT("name"), wxT("")) == name)
			{
				break;
			}
			child = child->GetNext();
		}
		if (child != NULL)
		{
			Persistent::DeserializeObject(*child->GetChildren(), panel->GetGraphCtrl());
		}
	}
}
