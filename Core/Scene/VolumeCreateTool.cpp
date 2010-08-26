/*#include "Precompile.h"*/
#include "VolumeCreateTool.h"

#include "Core/Scene/Mesh.h"
#include "Core/Scene/Scene.h"
#include "Volume.h"
#include "VolumeType.h"

#include "Core/Scene/Pick.h"

using namespace Helium;
using namespace Helium::Core;

Content::VolumeShape VolumeCreateTool::s_Shape = Content::VolumeShapes::Cube;

SCENE_DEFINE_TYPE(Core::VolumeCreateTool);

void VolumeCreateTool::InitializeType()
{
  Reflect::RegisterClassType< Core::VolumeCreateTool >( TXT( "Core::VolumeCreateTool" ) );
}

void VolumeCreateTool::CleanupType()
{
  Reflect::UnregisterClassType< Core::VolumeCreateTool >();
}

VolumeCreateTool::VolumeCreateTool(Core::Scene* scene, PropertiesGenerator* generator)
: Core::CreateTool (scene, generator)
{

}

VolumeCreateTool::~VolumeCreateTool()
{

}

Core::TransformPtr VolumeCreateTool::CreateNode()
{
#ifdef SCENE_DEBUG_RUNTIME_DATA_SELECTION

  Content::VolumePtr v = new Content::Volume( s_Shape );

  v->RectifyRuntimeData();

  VolumePtr volume = new Core::Volume( m_Scene, v );

  m_Scene->AddObject( volume );

  {
    OS_SelectableDumbPtr selection;
    selection.push_back( volume );
    m_Scene->GetSelection().SetItems( selection );

    m_Scene->GetSelection().Clear();
  }

  m_Scene->RemoveObject( volume );

  return volume;

#else

  Content::VolumePtr volume = new Content::Volume();
  
  volume->m_Shape = s_Shape;
  
  return new Core::Volume ( m_Scene, volume );

#endif
}

void VolumeCreateTool::CreateProperties()
{
  m_Generator->PushPanel( TXT( "Volume" ), true);
  {
    m_Generator->PushContainer();
    {
      m_Generator->AddLabel( TXT( "Shape" ) );

      Inspect::Choice* choice = m_Generator->AddChoice<int>( new Helium::MemberProperty<Core::VolumeCreateTool, int>(this, &VolumeCreateTool::GetVolumeShape, &VolumeCreateTool::SetVolumeShape) );
      choice->SetDropDown( true );
      std::vector< Inspect::ChoiceItem > items;

      {
        tostringstream str;
        str << Content::VolumeShapes::Cube;
        items.push_back( Inspect::ChoiceItem( TXT( "Cube" ), str.str() ) );
      }

      {
        tostringstream str;
        str << Content::VolumeShapes::Cylinder;
        items.push_back( Inspect::ChoiceItem( TXT( "Cylinder" ), str.str() ) );
      }

      {
        tostringstream str;
        str << Content::VolumeShapes::Sphere;
        items.push_back( Inspect::ChoiceItem( TXT( "Sphere" ), str.str() ) );
      }

      {
        tostringstream str;
        str << Content::VolumeShapes::Capsule;
        items.push_back( Inspect::ChoiceItem( TXT( "Capsule" ), str.str() ) );
      }

      choice->SetItems( items );
    }
    m_Generator->Pop();

    __super::CreateProperties();
  }
  m_Generator->Pop();
}

int VolumeCreateTool::GetVolumeShape() const
{
  return s_Shape;
}

void VolumeCreateTool::SetVolumeShape(int value)
{
  s_Shape = static_cast< Content::VolumeShape > (value);

  Place(Math::Matrix4::Identity);
}
