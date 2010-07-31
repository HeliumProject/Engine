#include "Precompile.h"
#include "LocatorType.h"

#include "Locator.h"
#include "Scene.h"

#include "Viewport.h"
#include "Color.h"

#include "PrimitiveLocator.h"
#include "PrimitiveCube.h"

using namespace Helium;
using namespace Helium::Editor;

EDITOR_DEFINE_TYPE(Editor::LocatorType);

void LocatorType::InitializeType()
{
  Reflect::RegisterClass< Editor::LocatorType >( TXT( "Editor::LocatorType" ) );
}

void LocatorType::CleanupType()
{
  Reflect::UnregisterClass< Editor::LocatorType >();
}

LocatorType::LocatorType( Editor::Scene* scene, i32 instanceType )
: Editor::InstanceType( scene, instanceType )
{
  m_Locator = new Editor::PrimitiveLocator( scene->GetViewport()->GetResources() );
  m_Locator->Update();

  m_Cube = new Editor::PrimitiveCube( scene->GetViewport()->GetResources() );
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

const Editor::Primitive* LocatorType::GetShape( Content::LocatorShape shape ) const
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