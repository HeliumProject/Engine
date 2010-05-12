#include "Precompile.h"
#include "LightingVolumeType.h"

#include "Volume.h"
#include "Scene.h"

#include "View.h"
#include "Color.h"

#include "PrimitiveCube.h"


using namespace Luna;

LUNA_DEFINE_TYPE(Luna::LightingVolumeType);

void LightingVolumeType::InitializeType()
{
  Reflect::RegisterClass< Luna::LightingVolumeType >( "Luna::LightingVolumeType" );
}

void LightingVolumeType::CleanupType()
{
  Reflect::UnregisterClass< Luna::LightingVolumeType >();
}

LightingVolumeType::LightingVolumeType( Luna::Scene* scene, i32 instanceType )
: Luna::InstanceType( scene, instanceType )
{
  m_Cube = new Luna::PrimitiveCube( scene->GetView()->GetResources() );
  m_Cube->Update();
}

LightingVolumeType::~LightingVolumeType()
{
  delete m_Cube;
}

void LightingVolumeType::Create()
{
  m_Cube->Create();
}

void LightingVolumeType::Delete()
{
  m_Cube->Delete();
}