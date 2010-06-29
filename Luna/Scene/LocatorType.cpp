#include "Precompile.h"
#include "LocatorType.h"

#include "Locator.h"
#include "Scene.h"

#include "View.h"
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
  m_Locator = new Luna::PrimitiveLocator( scene->GetView()->GetResources() );
  m_Locator->Update();

  m_Cube = new Luna::PrimitiveCube( scene->GetView()->GetResources() );
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

void LocatorType::SetConfiguration(const TypeConfigPtr& t)
{
  __super::SetConfiguration(t);

  for ( int i=0; i<2; i++ )
  {
    Luna::Primitive* shape = NULL;

    switch (i)
    {
    case 0:
      shape = m_Locator;
      break;

    case 1:
      shape = m_Cube;
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