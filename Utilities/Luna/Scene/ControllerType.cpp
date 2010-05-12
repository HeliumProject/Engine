#include "Precompile.h"
#include "ControllerType.h"

#include "Controller.h"
#include "Scene.h"

#include "View.h"
#include "Color.h"

#include "PrimitiveCube.h"

using namespace Luna;

LUNA_DEFINE_TYPE(Luna::ControllerType);

void ControllerType::InitializeType()
{
  Reflect::RegisterClass< Luna::ControllerType >( "Luna::ControllerType" );
}

void ControllerType::CleanupType()
{
  Reflect::UnregisterClass< Luna::ControllerType >();
}

ControllerType::ControllerType( Luna::Scene* scene, i32 instanceType )
: Luna::InstanceType( scene, instanceType )
{
  m_Bounds = new Luna::PrimitiveCube( scene->GetView()->GetResources() );
  m_Bounds->Update();
}

ControllerType::~ControllerType()
{
  delete m_Bounds;
}

void ControllerType::Create()
{
  m_Bounds->Create();
}

void ControllerType::Delete()
{
  m_Bounds->Delete();
}

void ControllerType::SetConfiguration(const TypeConfigPtr& t)
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