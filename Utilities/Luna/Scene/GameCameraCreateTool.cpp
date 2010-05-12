#include "Precompile.h"
#include "GameCameraCreateTool.h"

#include "Mesh.h"
#include "Scene.h"
#include "GameCamera.h"
#include "InstanceType.h"

#include "Pick.h"

using namespace Luna;

LUNA_DEFINE_TYPE(Luna::GameCameraCreateTool);

void GameCameraCreateTool::InitializeType()
{
  Reflect::RegisterClass< Luna::GameCameraCreateTool >( "Luna::GameCameraCreateTool" );
}

void GameCameraCreateTool::CleanupType()
{
  Reflect::UnregisterClass< Luna::GameCameraCreateTool >();
}

GameCameraCreateTool::GameCameraCreateTool(Luna::Scene* scene, Enumerator* enumerator)
: Luna::CreateTool (scene, enumerator)
{

}

GameCameraCreateTool::~GameCameraCreateTool()
{

}

Luna::TransformPtr GameCameraCreateTool::CreateNode()
{
  return new Luna::GameCamera ( m_Scene, new Content::GameCamera () );
}
