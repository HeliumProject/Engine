/*#include "Precompile.h"*/
#include "LocatorType.h"

#include "Locator.h"
#include "Core/Scene/Scene.h"

#include "Core/Scene/Viewport.h"
#include "Color.h"

#include "PrimitiveLocator.h"
#include "PrimitiveCube.h"

using namespace Helium;
using namespace Helium::Core;

SCENE_DEFINE_TYPE(Core::LocatorType);

void LocatorType::InitializeType()
{
  Reflect::RegisterClassType< Core::LocatorType >( TXT( "Core::LocatorType" ) );
}

void LocatorType::CleanupType()
{
  Reflect::UnregisterClassType< Core::LocatorType >();
}

LocatorType::LocatorType( Core::Scene* scene, i32 instanceType )
: Core::InstanceType( scene, instanceType )
{
  m_Locator = new Core::PrimitiveLocator( scene->GetViewport()->GetResources() );
  m_Locator->Update();

  m_Cube = new Core::PrimitiveCube( scene->GetViewport()->GetResources() );
  m_Cube->Update();
}

LocatorType::~LocatorType()
{
  delete m_Locator;
  delete m_Cube;
}

void LocatorType::Create()
{
  m_Locator->Create();
  m_Cube->Create();
}

void LocatorType::Delete()
{
  m_Locator->Delete();
  m_Cube->Delete();
}

const Core::Primitive* LocatorType::GetShape( Content::LocatorShape shape ) const
{
  switch (shape)
  {
  case Content::LocatorShapes::Cross:
    {
      return m_Locator;
    }

  case Content::LocatorShapes::Cube:
    {
      return m_Cube;
    }
  }

  return NULL;
}