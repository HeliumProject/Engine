#include "Precompile.h"
#include "SceneTool.h"

using namespace Luna;

LUNA_DEFINE_TYPE(Luna::SceneTool);

void SceneTool::InitializeType()
{
  Reflect::RegisterClass< Luna::SceneTool >( "Luna::SceneTool" );
}

void SceneTool::CleanupType()
{
  Reflect::UnregisterClass< Luna::SceneTool >();
}