#include "Precompile.h"
#include "GameCameraType.h"

#include "GameCamera.h"
#include "Scene.h"

#include "View.h"
#include "Color.h"

#include "PrimitiveCube.h"

using namespace Luna;

LUNA_DEFINE_TYPE(Luna::GameCameraType);

void GameCameraType::InitializeType()
{
  Reflect::RegisterClass< Luna::GameCameraType >( "Luna::GameCameraType" );
}

void GameCameraType::CleanupType()
{
  Reflect::UnregisterClass< Luna::GameCameraType >();
}

GameCameraType::GameCameraType( Luna::Scene* scene, i32 instanceType )
: Luna::InstanceType( scene, instanceType )
{
  m_Bounds = new Luna::PrimitiveCube( scene->GetView()->GetResources() );
  m_Bounds->Update();
}

GameCameraType::~GameCameraType()
{
  delete m_Bounds;
}

void GameCameraType::Create()
{
  m_Bounds->Create();
}

void GameCameraType::Delete()
{
  m_Bounds->Delete();
}

void GameCameraType::SetConfiguration(const TypeConfigPtr& t)
{
  __super::SetConfiguration(t);

  if (m_Bounds)
  {
    m_Bounds->SetSolid(t->m_Solid);
    m_Bounds->SetTransparent(t->m_Transparent);

    if (t->m_Solid)
    {
      m_Bounds->SetUsingCameraShadingMode(true);
    }

    m_Bounds->Update();
  }
}
