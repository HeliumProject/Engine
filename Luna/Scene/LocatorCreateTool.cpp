#include "Precompile.h"
#include "LocatorCreateTool.h"

#include "Mesh.h"
#include "Scene.h"
#include "Locator.h"
#include "LocatorType.h"

#include "Pick.h"

using namespace Luna;

Content::LocatorShape LocatorCreateTool::s_Shape = Content::LocatorShapes::Cross;

LUNA_DEFINE_TYPE(Luna::LocatorCreateTool);

void LocatorCreateTool::InitializeType()
{
  Reflect::RegisterClass< Luna::LocatorCreateTool >( TXT( "Luna::LocatorCreateTool" ) );
}

void LocatorCreateTool::CleanupType()
{
  Reflect::UnregisterClass< Luna::LocatorCreateTool >();
}

LocatorCreateTool::LocatorCreateTool(Luna::Scene* scene, PropertiesGenerator* generator)
: Luna::CreateTool (scene, generator)
{

}

LocatorCreateTool::~LocatorCreateTool()
{

}

Luna::TransformPtr LocatorCreateTool::CreateNode()
{
#ifdef LUNA_DEBUG_RUNTIME_DATA_SELECTION

  Content::LocatorPtr v = new Content::Locator( s_Shape );

  v->RectifyRuntimeData();

  LLocatorPtr locator = new Luna::Locator( m_Scene, v );

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

  return new Luna::Locator ( m_Scene, new Content::Locator ( s_Shape ) );

#endif
}

void LocatorCreateTool::CreateProperties()
{
  m_Generator->PushPanel( TXT( "Locator" ), true);
  {
    m_Generator->PushContainer();
    {
      m_Generator->AddLabel( TXT( "Shape" ) );

      Inspect::Choice* choice = m_Generator->AddChoice<int>( new Nocturnal::MemberProperty<Luna::LocatorCreateTool, int>(this, &LocatorCreateTool::GetLocatorShape, &LocatorCreateTool::SetLocatorShape) );
      choice->SetDropDown( true );
      Inspect::V_Item items;

      {
        tostringstream str;
        str << Content::LocatorShapes::Cross;
        items.push_back( Inspect::Item( TXT( "Cross" ), str.str() ) );
      }

      {
        tostringstream str;
        str << Content::LocatorShapes::Cube;
        items.push_back( Inspect::Item( TXT( "Cube" ), str.str() ) );
      }

      choice->SetItems( items );
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
