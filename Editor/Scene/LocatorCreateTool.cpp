#include "Precompile.h"
#include "LocatorCreateTool.h"

#include "Mesh.h"
#include "Scene.h"
#include "Locator.h"
#include "LocatorType.h"

#include "Pick.h"

using namespace Helium;
using namespace Helium::Editor;

Content::LocatorShape LocatorCreateTool::s_Shape = Content::LocatorShapes::Cross;

EDITOR_DEFINE_TYPE(Editor::LocatorCreateTool);

void LocatorCreateTool::InitializeType()
{
  Reflect::RegisterClass< Editor::LocatorCreateTool >( TXT( "Editor::LocatorCreateTool" ) );
}

void LocatorCreateTool::CleanupType()
{
  Reflect::UnregisterClass< Editor::LocatorCreateTool >();
}

LocatorCreateTool::LocatorCreateTool(Editor::Scene* scene, PropertiesGenerator* generator)
: Editor::CreateTool (scene, generator)
{

}

LocatorCreateTool::~LocatorCreateTool()
{

}

Editor::TransformPtr LocatorCreateTool::CreateNode()
{
#ifdef LUNA_DEBUG_RUNTIME_DATA_SELECTION

  Content::LocatorPtr v = new Content::Locator( s_Shape );

  v->RectifyRuntimeData();

  LLocatorPtr locator = new Editor::Locator( m_Scene, v );

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

  return new Editor::Locator ( m_Scene, new Content::Locator ( s_Shape ) );

#endif
}

void LocatorCreateTool::CreateProperties()
{
  m_Generator->PushPanel( TXT( "Locator" ), true);
  {
    m_Generator->PushContainer();
    {
      m_Generator->AddLabel( TXT( "Shape" ) );

      Inspect::Choice* choice = m_Generator->AddChoice<int>( new Helium::MemberProperty<Editor::LocatorCreateTool, int>(this, &LocatorCreateTool::GetLocatorShape, &LocatorCreateTool::SetLocatorShape) );
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
