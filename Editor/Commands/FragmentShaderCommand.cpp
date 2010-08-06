#include "Precompile.h"
#include "FragmentShaderCommand.h"

#include <wx/msgdlg.h>
#include <wx/clipbrd.h>
#include <wx/debug.h>

#include <wx/wx.h>

#undef RegisterClass

#include "Application/Application.h"

#include "Editor/Graph/ShaderFrame.h"
#include "Editor/Graph/ProjectNotebook.h"
#include "Editor/Graph/Panel.h"
#include "Editor/Graph/Node.h"
#include "Editor/Graph/Shape.h"
#include "Editor/Graph/Group.h"
#include "Editor/Graph/Graph.h"
#include "Editor/Graph/LuaUtilities.h"
#include "Editor/Graph/NodeDefinition.h"
#include "Editor/Graph/Clipboard.h"
#include "Editor/Graph/Debug.h"
#include "Editor/Graph/Serialized.h"

#include "Editor/Graph/XPM/icon.xpm"

using namespace Helium;

static void InitLibPath()
{
    tchar* project_data_path =  _tgetenv(TXT("IG_PROJECT_DATA"));
    wxString common_path =  project_data_path;

    if(project_data_path)
    {
        common_path.Append(wxT("\\FragmentShader\\"));
        g_FragmentShaderLibPath = common_path;
        g_FragmentShaderLibPath.Append(wxT("lib\\"));
    }
}

class FragmentShaderApp : public wxApp
{
public:
    FragmentShaderApp()
    {
    }

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

    DECLARE_NO_COPY_CLASS(FragmentShaderApp)
};
IMPLEMENT_APP_NO_MAIN(FragmentShaderApp)

static void
Abort(std::exception *e)
{
    tstring temp;
    Helium::ConvertString( e->what(), temp );

    wxMessageBox(temp, wxT("Error"), wxOK | wxICON_ERROR);
    if (wxTheClipboard->Open())
    {
        wxTheClipboard->SetData(NEW(wxTextDataObject,(temp)));
        wxTheClipboard->Flush();
        wxTheClipboard->Close();
    }
    lua_close(g_FragmentShaderLuaState);
    throw e;
}

void
FragmentShaderApp::ProcessOptions()
{
    usage = false;
    compile = false;
    mask = wxT("");
    open = wxT("");
    graph = 0;
    for (int index = 1; index < argc; index++)
    {
        const wxChar* argStr = argv[index];

        std::string temp;
        Helium::ConvertString( argStr, temp );
        wxString option = wxString::FromAscii(temp.c_str());

        wxString arg;
        if (index != (argc - 1))
        {
            const wxChar* bareArg = argv[index];
            bareArg++;
            Helium::ConvertString( bareArg, temp );
            arg = wxString::FromAscii(temp.c_str());
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
                    Helium::ConvertString( (const wxChar*)argv[index], temp );

                    wxString name;
                    name.Append(wxString::FromAscii(temp.c_str()));
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
FragmentShaderApp::OnInit()
{
    ProcessOptions();

    if (usage)
    {
        Log::Print(
            TXT( "Usage: igFragmentShader [options] [filename]\n" )
            TXT( "\n" )
            TXT( "Options:\n" )
            TXT( "  -h or --help\n" )
            TXT( "    Show this help.\n" )
            TXT( "\n" )
            TXT( "  -c or --compile\n" )
            TXT( "    Compile the file instead of open it. Must be specified if either -m, -o\n" )
            TXT( "    or -g are specified.\n" )
            TXT( "\n" )
            TXT( "  -g n or --graph n\n" )
            TXT( "    Compile only the n-th graph using \"output_cg_%%d.cg\" as the mask instead\n" )
            TXT( "    of compiling all graphs in the project.\n" )
            TXT( "\n" )
            TXT( "  -m m or --mask m\n" )
            TXT( "    Override the default mask. The mask must have a \"%%d\" somewhere.\n" )
            TXT( "\n" )
            TXT( "  -o f... or --output f...\n" )
            TXT( "    Use these file names instead of generating them with the mask. Must be the\n" )
            TXT( "    last option if specified.\n" )
            TXT( "\n" )
            TXT( "  filename\n" )
            TXT( "    The file to open or compile. It must be informed except if -h is used.\n" )
            );
        return false;
    }

    return true;
}

int
FragmentShaderApp::OnRun()
{
    // Register types for de-serialization.
    InputPort  t1; Serialized::RegisterSerializedClass(t1.GetClassName(), t1.Create);
    OutputPort t2; Serialized::RegisterSerializedClass(t2.GetClassName(), t2.Create);
    Node       t3; Serialized::RegisterSerializedClass(t3.GetClassName(), t3.Create);
    Group      t4; Serialized::RegisterSerializedClass(t4.GetClassName(), t4.Create);
    Shape      t5; Serialized::RegisterSerializedClass(t5.GetClassName(), t5.Create);
    Clipboard  t6; Serialized::RegisterSerializedClass(t6.GetClassName(), t6.Create);
    // HACK must separate Graph into a Canvas and a Shape
    //Graph      t7; Serialized::RegisterClass(t7.GetClassName(), t7.Create);
    Serialized::RegisterSerializedClass(wxT("graph"), Graph::Create);

    g_FragmentShaderLuaState = LuaUtilities::NewState();
    Debug::Init(g_FragmentShaderLuaState);
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
FragmentShaderApp::OnExit()
{
    lua_close(g_FragmentShaderLuaState);
    return wxApp::OnExit();
}

bool
FragmentShaderApp::OnExceptionInMainLoop()
{
    throw;
}
