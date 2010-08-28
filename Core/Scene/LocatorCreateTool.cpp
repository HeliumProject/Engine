/*#include "Precompile.h"*/
#include "LocatorCreateTool.h"

#include "Core/Scene/Mesh.h"
#include "Core/Scene/Scene.h"
#include "Locator.h"
#include "LocatorType.h"

#include "Core/Scene/Pick.h"

using namespace Helium;
using namespace Helium::Core;

Content::LocatorShape LocatorCreateTool::s_Shape = Content::LocatorShapes::Cross;

SCENE_DEFINE_TYPE(Core::LocatorCreateTool);

void LocatorCreateTool::InitializeType()
{
  Reflect::RegisterClassType< Core::LocatorCreateTool >( TXT( "Core::LocatorCreateTool" ) );
}

void LocatorCreateTool::CleanupType()
{
  Reflect::UnregisterClassType< Core::LocatorCreateTool >();
}

LocatorCreateTool::LocatorCreateTool(Core::Scene* scene, PropertiesGenerator* generator)
: Core::CreateTool (scene, generator)
{

}

LocatorCreateTool::~LocatorCreateTool()
{

}

Core::TransformPtr LocatorCreateTool::CreateNode()
{
#ifdef SCENE_DEBUG_RUNTIME_DATA_SELECTION

  Content::LocatorPtr v = new Content::Locator( s_Shape );

  v->RectifyRuntimeData();

  LLocatorPtr locator = new Core::Locator( m_Scene, v );

  m_Scene->AddObject( locator );

  {
    OS_SelectableDumbPtr selection;
    selection.push_back( locator );
    m_Scene->GetSelection().SetItems( selection );

    m_Scene->GetSelection().Clear();
  }

  m_Scene->RemoveObject( locator );

  return locator;

#else

  return new Core::Locator ( m_Scene, new Content::Locator ( s_Shape ) );

#endif
}

void LocatorCreateTool::CreateProperties()
{
  m_Generator->PushContainer( TXT( "Locator" ) );
  {
    m_Generator->PushContainer();
    {
      m_Generator->AddLabel( TXT( "Shape" ) );

      Inspect::Choice* choice = m_Generator->AddChoice<int>( new Helium::MemberProperty<Core::LocatorCreateTool, int>(this, &LocatorCreateTool::GetLocatorShape, &LocatorCreateTool::SetLocatorShape) );
      choice->a_IsDropDown.Set( true );
      std::vector< Inspect::ChoiceItem > items;

      {
        tostringstream str;
        str << Content::LocatorShapes::Cross;
        items.push_back( Inspect::ChoiceItem( TXT( "Cross" ), str.str() ) );
      }

      {
        tostringstream str;
        str << Content::LocatorShapes::Cube;
        items.push_back( Inspect::ChoiceItem( TXT( "Cube" ), str.str() ) );
      }

      choice->a_Items.Set( items );
    }
    m_Generator->Pop();

    __super::CreateProperties();
  }
  m_Generator->Pop();
}

int LocatorCreateTool::GetLocatorShape() const
{
  return s_Shape;
}

void LocatorCreateTool::SetLocatorShape(int value)
{
  s_Shape = static_cast< Content::LocatorShape > (value);

  Place(Math::Matrix4::Identity);
}
