#include "main.h"

#include <wx/msgdlg.h>
#include <wx/clipbrd.h>
#include <wx/debug.h>

#include <wx/wx.h>

#include "AppUtils/AppUtils.h"

#include "shader.h"
#include "project.h"
#include "panel.h"
#include "node.h"
#include "shape.h"
#include "group.h"
#include "graph.h"
#include "luautil.h"
#include "nodelib.h"
#include "clipboard.h"

#include "debug.h"

#include "icon.xpm"

wxString g_LibPath;
lua_State *g_L;

static void
InitLibPath()
{
  char*     project_data_path  =  getenv("IG_PROJECT_DATA");
  wxString  common_path        =  project_data_path;

  if(project_data_path)
  {
    common_path.Append("\\FragmentShader\\");
    g_LibPath = common_path;
    g_LibPath.Append("lib\\");
  }
}

class Application: public wxApp
{
	public:
		Application() { }
		void ProcessOptions();
		virtual bool OnInit();
		virtual int OnRun();
		virtual int OnExit();
		bool OnExceptionInMainLoop();

	private:
		bool usage, compile;
		wxString mask, open;
		int graph;
		wxArrayString output;
		Shader *shader;

	DECLARE_NO_COPY_CLASS(Application)
};

IMPLEMENT_APP(Application)

static void
Abort(std::exception *e)
{
	wxMessageBox(e->what(), wxT("Error"), wxOK | wxICON_ERROR);
	if (wxTheClipboard->Open())
	{
		wxTheClipboard->SetData(NEW(wxTextDataObject, (e->what())));
		wxTheClipboard->Flush();
		wxTheClipboard->Close();
	}
	lua_close(g_L);
	throw e;
}

void
Application::ProcessOptions()
{
	usage = false;
	compile = false;
	mask = wxT("");
	open = wxT("");
	graph = 0;
	for (int index = 1; index < argc; index++)
	{
		wxString option = wxString::FromAscii(argv[index]);
		wxString arg;
		if (index != (argc - 1))
		{
			arg = wxString::FromAscii(argv[index + 1]);
		}
		if (!option.Cmp(wxT("-h")) || !option.Cmp(wxT("--help")))
		{
			usage = true;
			break;
		}
		else if (!option.Cmp(wxT("-c")) || !option.Cmp(wxT("--compile")))
		{
			compile = true;
		}
		else if (!option.Cmp(wxT("-m")) || !option.Cmp(wxT("--mask")))
		{
			if (arg.IsEmpty() || !mask.IsEmpty())
			{
				usage = true;
				break;
			}
			else
			{
				mask.Append(arg);
				index++;
			}
		}
		else if (!option.Cmp(wxT("-g")) || !option.Cmp(wxT("--graph")))
		{
			if (arg.IsEmpty() || graph != 0)
			{
				usage = true;
				break;
			}
			else
			{
				long val;
				if (!arg.ToLong(&val, 10) || val <= 0)
				{
					usage = true;
					break;
				}
				graph = (int)val;
				index++;
			}
		}
		else if (!option.Cmp(wxT("-o")) || !option.Cmp(wxT("--output")))
		{
			if (arg.IsEmpty() || output.Count() != 0)
			{
				usage = true;
				break;
			}
			else
			{
				while (++index != argc)
				{
					wxString name;
					name.Append(wxString::FromAscii(argv[index]));
					output.Add(name);
				}
				if (output.Count() == 0)
				{
					usage = true;
				}
				break;
			}
		}
		else if (option[0] != wxT('-'))
		{
			if (!open.IsEmpty())
			{
				usage = true;
				break;
			}
			else
			{
				open.Append(option);
			}
		}
		else
		{
			usage = true;
			break;
		}
	}
	// Check for the default mask.
	if (mask.IsEmpty())
	{
		mask = wxT("output_cg_%d.cg");
	}
	// Mask provided, it must contain %d and user must have
	// requested for compilation.
	else if (!mask.Contains(wxT("%d")) || !compile)
	{
		usage = true;
	}
	// Graph number or output files provided, user must have
	// requested for compilation.
	if ((graph != 0 || output.Count() != 0) && !compile)
	{
		usage = true;
	}
}

bool
Application::OnInit()
{
  ProcessOptions();

  if (usage)
  {
  	printf(
  		"Usage: igFragmentShader [options] [filename]\n"
  		"\n"
  		"Options:\n"
  		"  -h or --help\n"
  		"    Show this help.\n"
  		"\n"
  		"  -c or --compile\n"
  		"    Compile the file instead of open it. Must be specified if either -m, -o\n"
  		"    or -g are specified.\n"
  		"\n"
  		"  -g n or --graph n\n"
  		"    Compile only the n-th graph using \"output_cg_%%d.cg\" as the mask instead\n"
  		"    of compiling all graphs in the project.\n"
  		"\n"
  		"  -m m or --mask m\n"
  		"    Override the default mask. The mask must have a \"%%d\" somewhere.\n"
  		"\n"
  		"  -o f... or --output f...\n"
  		"    Use these file names instead of generating them with the mask. Must be the\n"
  		"    last option if specified.\n"
  		"\n"
  		"  filename\n"
  		"    The file to open or compile. It must be informed except if -h is used.\n"
  	);
  	return false;
  }

	return true;
}

int
Application::OnRun()
{
  // Register types for de-serialization.
  InputPort  t1; Persistent::RegisterClass(t1.GetClassName(), t1.Create);
  OutputPort t2; Persistent::RegisterClass(t2.GetClassName(), t2.Create);
  Node       t3; Persistent::RegisterClass(t3.GetClassName(), t3.Create);
  Group      t4; Persistent::RegisterClass(t4.GetClassName(), t4.Create);
  Shape      t5; Persistent::RegisterClass(t5.GetClassName(), t5.Create);
  Clipboard  t6; Persistent::RegisterClass(t6.GetClassName(), t6.Create);
  // HACK must separate Graph into a Canvas and a Shape
  //Graph      t7; Persistent::RegisterClass(t7.GetClassName(), t7.Create);
  Persistent::RegisterClass(wxT("graph"), Graph::Create);

  g_L = LuaUtil::NewState();
  Debug::Init(g_L);
  InitLibPath();

  shader = NEW(Shader, ());
  if (open.IsEmpty())
  {
    shader->SetIcon(wxIcon(icon_xpm));
    shader->Show(true);
  }
  else
  {
    shader->Open(open);
    if (compile)
    {
      if (graph != 0)
      {
        wxString out;
        if (output.Count() != 0 && graph <= (int)output.Count())
        {
          out = output[graph - 1];
        }
        else
        {
          out.Printf(mask, graph);
        }
        if (shader->Compile(graph, out))
        {
          delete shader;
          return 0;
        }

        delete shader;
        return -1;
      }

      bool done = false;
      int  oidx = 0;

      for (int index = 1; !done; index++)
      {
        wxString out;
        if (output.Count() != 0 && index <= (int)output.Count())
        {
          out = output[oidx++];
        }
        else
        {
          out.Printf(mask, index);
        }
        switch (shader->Compile(index - 1, out))
        {
          case Project::CompileError:
            break;
          case Project::CompileInvalidPage:
            done    = true;
          case Project::CompileSuccess:
            break;
        }
      }
      
      delete shader;
      return 0;
    }
  }
  shader->SetIcon(wxIcon(icon_xpm));
  shader->Show(true);

  return wxApp::OnRun();
}

int
Application::OnExit()
{
	lua_close(g_L);
	return wxApp::OnExit();
}

bool
Application::OnExceptionInMainLoop()
{
	throw;
}

int main()
{
  return AppUtils::StandardWinMain( &wxEntry );
}
