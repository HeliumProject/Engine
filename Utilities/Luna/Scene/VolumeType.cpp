#include "Precompile.h"
#include "VolumeType.h"

#include "Volume.h"
#include "Scene.h"

#include "View.h"
#include "Color.h"

#include "PrimitiveCube.h"
#include "PrimitiveSphere.h"
#include "PrimitiveCapsule.h"
#include "PrimitiveCylinder.h"

using namespace Luna;

LUNA_DEFINE_TYPE(Luna::VolumeType);

void VolumeType::InitializeType()
{
  Reflect::RegisterClass< Luna::VolumeType >( TXT( "Luna::VolumeType" ) );
}

void VolumeType::CleanupType()
{
  Reflect::UnregisterClass< Luna::VolumeType >();
}

VolumeType::VolumeType( Luna::Scene* scene, i32 instanceType )
: Luna::InstanceType( scene, instanceType )
{
  m_Cube = new Luna::PrimitiveCube( scene->GetView()->GetResources() );
  m_Cube->Update();

  m_Cylinder = new Luna::PrimitiveCylinder( scene->GetView()->GetResources() );
  m_Cylinder->Update();
  
  m_Sphere = new Luna::PrimitiveSphere( scene->GetView()->GetResources() );
  m_Sphere->Update();

  m_Capsule = new Luna::PrimitiveCapsule( scene->GetView()->GetResources() );
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

void VolumeType::SetConfiguration(const TypeConfigPtr& t)
{
  __super::SetConfiguration(t);

  for ( int i=0; i<4; i++ )
  {
    Luna::Primitive* shape = NULL;

    switch (i)
    {
    case 0:
      shape = m_Cube;
      break;

    case 1:
      shape = m_Cylinder;
      break;

    case 2:
      shape = m_Sphere;
      break;

    case 3:
      shape = m_Capsule;
      break;
    }

    if (shape)
    {
      shape->SetSolid(t->m_Solid);
      shape->SetTransparent(t->m_Transparent);

      if (t->m_Solid)
      {
        shape->SetUsingCameraShadingMode(true);
      }

      shape->Update();
    }
  }
}

const Luna::Primitive* VolumeType::GetShape( Content::VolumeShape shape ) const
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