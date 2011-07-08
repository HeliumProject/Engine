#include "SceneGraphPch.h"
#include "SceneGraph/PropertiesGenerator.h"

#include "Foundation/Log.h"
#include "Foundation/Container/Insert.h" 

#include "Foundation/Inspect/Interpreters/Reflect/ReflectInterpreter.h"
#include "Foundation/Inspect/Controls/LabelControl.h"

using namespace Helium;
using namespace Helium::SceneGraph;

//#define SCENE_DEBUG_PROPERTIES_GENERATOR

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
    g_Initialized = false;
}

void PropertiesGenerator::Reset()
{
    SCENE_GRAPH_SCOPE_TIMER( ("") );

    m_Container->Clear();
}
