#include "EditorScenePch.h"
#include "EditorScene/Tool.h"
#include "EditorScene/Scene.h"

HELIUM_DEFINE_ABSTRACT( Helium::Editor::Tool );

using namespace Helium;
using namespace Helium::Editor;

Tool::Tool( Editor::Scene* scene, PropertiesGenerator* generator )
	: m_Generator ( generator )
	, m_View ( scene->GetViewport() )
	, m_Scene ( scene )
	, m_AllowSelection( true )
{

}