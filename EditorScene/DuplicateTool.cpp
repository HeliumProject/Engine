#include "EditorScenePch.h"
#include "DuplicateTool.h"

#include "EditorScene/Scene.h"
#include "EditorScene/Pick.h"

HELIUM_DEFINE_ABSTRACT( Helium::Editor::DuplicateTool );

using namespace Helium;
using namespace Helium::Editor;

DuplicateTool::DuplicateTool(Editor::Scene* scene, PropertiesGenerator* generator)
	: Editor::CreateTool (scene, generator)
	, m_Source (NULL)
{
	if (!m_Scene->GetSelection().GetItems().Empty())
	{
		m_Source = Reflect::SafeCast<Editor::Transform>( *m_Scene->GetSelection().GetItems().Begin() );
	}
}

DuplicateTool::~DuplicateTool()
{

}

Editor::TransformPtr DuplicateTool::CreateNode()
{
	if (m_Source)
	{
		return Reflect::SafeCast<Editor::Transform>( m_Source->Duplicate() );
	}
	else
	{
		return NULL;
	}
}