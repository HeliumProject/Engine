#include "Precompile.h"
#include "VolumeType.h"

#include "Volume.h"
#include "Scene.h"

#include "Viewport.h"
#include "Color.h"

#include "PrimitiveCube.h"
#include "PrimitiveSphere.h"
#include "PrimitiveCapsule.h"
#include "PrimitiveCylinder.h"

using namespace Editor;

LUNA_DEFINE_TYPE(Editor::VolumeType);

void VolumeType::InitializeType()
{
  Reflect::RegisterClass< Editor::VolumeType >( TXT( "Editor::VolumeType" ) );
}

void VolumeType::CleanupType()
{
  Reflect::UnregisterClass< Editor::VolumeType >();
}

VolumeType::VolumeType( Editor::Scene* scene, i32 instanceType )
: Editor::InstanceType( scene, instanceType )
{
  m_Cube = new Editor::PrimitiveCube( scene->GetViewport()->GetResources() );
  m_Cube->Update();

  m_Cylinder = new Editor::PrimitiveCylinder( scene->GetViewport()->GetResources() );
  m_Cylinder->Update();
  
  m_Sphere = new Editor::PrimitiveSphere( scene->GetViewport()->GetResources() );
  m_Sphere->Update();

  m_Capsule = new Editor::PrimitiveCapsule( scene->GetViewport()->GetResources() );
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

const Editor::Primitive* VolumeType::GetShape( Content::VolumeShape shape ) const
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