/*#include "Precompile.h"*/
#include "VolumeType.h"

#include "Volume.h"
#include "Core/Scene/Scene.h"

#include "Core/Scene/Viewport.h"
#include "Color.h"

#include "PrimitiveCube.h"
#include "PrimitiveSphere.h"
#include "PrimitiveCapsule.h"
#include "PrimitiveCylinder.h"

using namespace Helium;
using namespace Helium::Core;

REFLECT_DEFINE_ABSTRACT(Core::VolumeType);

void VolumeType::InitializeType()
{
  Reflect::RegisterClassType< Core::VolumeType >( TXT( "Core::VolumeType" ) );
}

void VolumeType::CleanupType()
{
  Reflect::UnregisterClassType< Core::VolumeType >();
}

VolumeType::VolumeType( Core::Scene* scene, i32 instanceType )
: Core::InstanceType( scene, instanceType )
{
  m_Cube = new Core::PrimitiveCube( scene->GetViewport()->GetResources() );
  m_Cube->Update();

  m_Cylinder = new Core::PrimitiveCylinder( scene->GetViewport()->GetResources() );
  m_Cylinder->Update();
  
  m_Sphere = new Core::PrimitiveSphere( scene->GetViewport()->GetResources() );
  m_Sphere->Update();

  m_Capsule = new Core::PrimitiveCapsule( scene->GetViewport()->GetResources() );
  m_Capsule->Update();
}

VolumeType::~VolumeType()
{
  delete m_Cube;
  delete m_Cylinder;
  delete m_Sphere;
  delete m_Capsule;
}

void VolumeType::Create()
{
  m_Cube->Create();
  m_Cylinder->Create();
  m_Sphere->Create();
  m_Capsule->Create();
}

void VolumeType::Delete()
{
  m_Cube->Delete();
  m_Cylinder->Delete();
  m_Sphere->Delete();
  m_Capsule->Delete();
}

const Core::Primitive* VolumeType::GetShape( Content::VolumeShape shape ) const
{
  switch (shape)
  {
  case Content::VolumeShapes::Cube:
    return m_Cube;

  case Content::VolumeShapes::Cylinder:
    return m_Cylinder;

  case Content::VolumeShapes::Sphere:
    return m_Sphere;

  case Content::VolumeShapes::Capsule:
    return m_Capsule;
  }

  return NULL;
}