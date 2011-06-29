#include "PipelinePch.h"
#include "Pipeline/SceneGraph/PropertiesGenerator.h"

#include "Foundation/Log.h"
#include "Foundation/Container/Insert.h" 

#include "Foundation/Inspect/Interpreters/Reflect/ReflectInterpreter.h"
#include "Foundation/Inspect/Controls/LabelControl.h"

using namespace Helium;
using namespace Helium::SceneGraph;

//#define SCENE_DEBUG_PROPERTIES_GENERATOR

namespace Helium
{
    namespace SceneGraph
    {
        M_PanelCreators s_PanelCreators;
    }
}

static bool g_Initialized = false;

PropertiesGenerator::PropertiesGenerator(Inspect::Container* container)
: Inspect::Interpreter (container)
{
}

PropertiesGenerator::~PropertiesGenerator()
{
}

void PropertiesGenerator::Initialize()
{
    g_Initialized = true;
}

void PropertiesGenerator::Cleanup()
{
    s_PanelCreators.clear();
    g_Initialized = false;
}

bool PropertiesGenerator::InitializePanel(const tstring& name, const CreatePanelSignature::Delegate& creator)
{
    SCENE_GRAPH_SCOPE_TIMER( ("") );

    HELIUM_ASSERT(g_Initialized);

    Helium::StdInsert<M_PanelCreators>::Result inserted = s_PanelCreators.insert(M_PanelCreators::value_type (name, creator));

    if (!inserted.second)
    {
        if (!creator.Equals( inserted.first->second ))
        {
            Log::Error( TXT( "Panel '%s' initialized with a different creator\n" ), name.c_str());     
        }
        else
        {
            Log::Warning( TXT( "Panel '%s' already initialized\n" ), name.c_str());
        }
    }

    return inserted.second;
}

void PropertiesGenerator::CleanupPanel(const tstring& name)
{
    HELIUM_ASSERT(g_Initialized);

    Log::Print( TXT( "Cleaned up Panel '%s'\n" ), name.c_str());

    s_PanelCreators.erase(name);
}

void PropertiesGenerator::Reset()
{
    SCENE_GRAPH_SCOPE_TIMER( ("") );

    m_Container->Clear();
}
