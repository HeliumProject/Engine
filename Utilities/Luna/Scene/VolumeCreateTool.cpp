#include "Precompile.h"
#include "VolumeCreateTool.h"

#include "Mesh.h"
#include "Scene.h"
#include "Volume.h"
#include "VolumeType.h"

#include "Pick.h"

using namespace Luna;

Content::VolumeShape VolumeCreateTool::s_Shape = Content::VolumeShapes::Cube;

LUNA_DEFINE_TYPE(Luna::VolumeCreateTool);

void VolumeCreateTool::InitializeType()
{
  Reflect::RegisterClass< Luna::VolumeCreateTool >( "Luna::VolumeCreateTool" );
}

void VolumeCreateTool::CleanupType()
{
  Reflect::UnregisterClass< Luna::VolumeCreateTool >();
}

VolumeCreateTool::VolumeCreateTool(Luna::Scene* scene, Enumerator* enumerator)
: Luna::CreateTool (scene, enumerator)
{

}

VolumeCreateTool::~VolumeCreateTool()
{

}

Luna::TransformPtr VolumeCreateTool::CreateNode()
{
#ifdef LUNA_DEBUG_RUNTIME_DATA_SELECTION

  Content::VolumePtr v = new Content::Volume( s_Shape );

  v->RectifyRuntimeData();

  VolumePtr volume = new Luna::Volume( m_Scene, v );

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
  
  return new Luna::Volume ( m_Scene, volume );

#endif
}

void VolumeCreateTool::CreateProperties()
{
  m_Enumerator->PushPanel("Volume", true);
  {
    m_Enumerator->PushContainer();
    {
      m_Enumerator->AddLabel("Shape");

      Inspect::Choice* choice = m_Enumerator->AddChoice<int>( new Nocturnal::MemberProperty<Luna::VolumeCreateTool, int>(this, &VolumeCreateTool::GetVolumeShape, &VolumeCreateTool::SetVolumeShape) );
      choice->SetDropDown( true );
      Inspect::V_Item items;

      {
        std::ostringstream str;
        str << Content::VolumeShapes::Cube;
        items.push_back( Inspect::Item( "Cube", str.str() ) );
      }

      {
        std::ostringstream str;
        str << Content::VolumeShapes::Cylinder;
        items.push_back( Inspect::Item( "Cylinder", str.str() ) );
      }

      {
        std::ostringstream str;
        str << Content::VolumeShapes::Sphere;
        items.push_back( Inspect::Item( "Sphere", str.str() ) );
      }

      {
        std::ostringstream str;
        str << Content::VolumeShapes::Capsule;
        items.push_back( Inspect::Item( "Capsule", str.str() ) );
      }

      choice->SetItems( items );
    }
    m_Enumerator->Pop();

    __super::CreateProperties();
  }
  m_Enumerator->Pop();
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
