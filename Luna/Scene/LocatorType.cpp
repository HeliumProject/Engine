#include "Precompile.h"
#include "LocatorType.h"

#include "Locator.h"
#include "Scene.h"

#include "Viewport.h"
#include "Color.h"

#include "PrimitiveLocator.h"
#include "PrimitiveCube.h"

using namespace Luna;

LUNA_DEFINE_TYPE(Luna::LocatorType);

void LocatorType::InitializeType()
{
  Reflect::RegisterClass< Luna::LocatorType >( TXT( "Luna::LocatorType" ) );
}

void LocatorType::CleanupType()
{
  Reflect::UnregisterClass< Luna::LocatorType >();
}

LocatorType::LocatorType( Luna::Scene* scene, i32 instanceType )
: Luna::InstanceType( scene, instanceType )
{
  m_Locator = new Luna::PrimitiveLocator( scene->GetViewport()->GetResources() );
  m_Locator->Update();

  m_Cube = new Luna::PrimitiveCube( scene->GetViewport()->GetResources() );
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

const Luna::Primitive* LocatorType::GetShape( Content::LocatorShape shape ) const
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