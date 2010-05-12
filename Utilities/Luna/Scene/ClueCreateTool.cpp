#include "Precompile.h"
#include "ClueCreateTool.h"

#include "Scene.h"
#include "ScenePreferences.h"
#include "Clue.h"
#include "VolumeCreateTool.h"
#include "VolumeType.h"

#include "Pick.h"

using namespace Luna;

LUNA_DEFINE_TYPE(Luna::ClueCreateTool);

void ClueCreateTool::InitializeType()
{
  Reflect::RegisterClass< Luna::ClueCreateTool >( "Luna::ClueCreateTool" );
}

void ClueCreateTool::CleanupType()
{
  Reflect::UnregisterClass< Luna::ClueCreateTool >();
}

ClueCreateTool::ClueCreateTool(Luna::Scene* scene, Enumerator* enumerator)
: Luna::CreateTool (scene, enumerator)
{

}

ClueCreateTool::~ClueCreateTool()
{

}

Luna::TransformPtr ClueCreateTool::CreateNode()
{
  Content::CluePtr clue = new Content::Clue();
  
  clue->m_Shape = Luna::VolumeCreateTool::s_Shape;
 
  Content::NodeVisibilityPtr nodeDefaults = SceneEditorPreferences()->GetDefaultNodeVisibility(); 

  CluePtr cluePtr = new Luna::Clue ( m_Scene, clue );
  cluePtr->SetPointerVisible( nodeDefaults->GetShowPointer() );
  return cluePtr;
}