#include "Precompile.h"
#include "ControllerCreateTool.h"

#include "Mesh.h"
#include "Scene.h"
#include "Controller.h"
#include "InstanceType.h"

#include "Pick.h"

using namespace Luna;

LUNA_DEFINE_TYPE(Luna::ControllerCreateTool);

void ControllerCreateTool::InitializeType()
{
  Reflect::RegisterClass< Luna::ControllerCreateTool >( "Luna::ControllerCreateTool" );
}

void ControllerCreateTool::CleanupType()
{
  Reflect::UnregisterClass< Luna::ControllerCreateTool >();
}

ControllerCreateTool::ControllerCreateTool(Luna::Scene* scene, Enumerator* enumerator)
: Luna::CreateTool (scene, enumerator)
{

}

ControllerCreateTool::~ControllerCreateTool()
{

}

Luna::TransformPtr ControllerCreateTool::CreateNode()
{
  return new Luna::Controller ( m_Scene, new Content::Controller () );
}