#include "EditorScenePch.h"
#include "EditorScene/PropertiesGenerator.h"

#include "Foundation/Log.h"

#include "Inspect/Controls/LabelControl.h"

using namespace Helium;
using namespace Helium::Editor;

//#define SCENE_DEBUG_PROPERTIES_GENERATOR

static bool g_Initialized = false;

PropertiesGenerator::PropertiesGenerator(Inspect::Container* container)
	: Inspect::ReflectInterpreter (container)
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
	HELIUM_EDITOR_SCENE_SCOPE_TIMER( ("") );

	m_Container->Clear();
}
