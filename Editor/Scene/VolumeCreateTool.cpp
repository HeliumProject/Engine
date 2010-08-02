#include "Precompile.h"
#include "VolumeCreateTool.h"

#include "Mesh.h"
#include "Scene.h"
#include "Volume.h"
#include "VolumeType.h"

#include "Pick.h"

using namespace Helium;
using namespace Helium::Editor;

Content::VolumeShape VolumeCreateTool::s_Shape = Content::VolumeShapes::Cube;

EDITOR_DEFINE_TYPE(Editor::VolumeCreateTool);

void VolumeCreateTool::InitializeType()
{
  Reflect::RegisterClass< Editor::VolumeCreateTool >( TXT( "Editor::VolumeCreateTool" ) );
}

void VolumeCreateTool::CleanupType()
{
  Reflect::UnregisterClass< Editor::VolumeCreateTool >();
}

VolumeCreateTool::VolumeCreateTool(Editor::Scene* scene, PropertiesGenerator* generator)
: Editor::CreateTool (scene, generator)
{

}

VolumeCreateTool::~VolumeCreateTool()
{

}

Editor::TransformPtr VolumeCreateTool::CreateNode()
{
#ifdef EDITOR_DEBUG_RUNTIME_DATA_SELECTION

  Content::VolumePtr v = new Content::Volume( s_Shape );

  v->RectifyRuntimeData();

  VolumePtr volume = new Editor::Volume( m_Scene, v );

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
  
  return new Editor::Volume ( m_Scene, volume );

#endif
}

void VolumeCreateTool::CreateProperties()
{
  m_Generator->PushPanel( TXT( "Volume" ), true);
  {
    m_Generator->PushContainer();
    {
      m_Generator->AddLabel( TXT( "Shape" ) );

      Inspect::Choice* choice = m_Generator->AddChoice<int>( new Helium::MemberProperty<Editor::VolumeCreateTool, int>(this, &VolumeCreateTool::GetVolumeShape, &VolumeCreateTool::SetVolumeShape) );
      choice->SetDropDown( true );
      Inspect::V_Item items;

      {
        tostringstream str;
        str << Content::VolumeShapes::Cube;
        items.push_back( Inspect::Item( TXT( "Cube" ), str.str() ) );
      }

      {
        tostringstream str;
        str << Content::VolumeShapes::Cylinder;
        items.push_back( Inspect::Item( TXT( "Cylinder" ), str.str() ) );
      }

      {
        tostringstream str;
        str << Content::VolumeShapes::Sphere;
        items.push_back( Inspect::Item( TXT( "Sphere" ), str.str() ) );
      }

      {
        tostringstream str;
        str << Content::VolumeShapes::Capsule;
        items.push_back( Inspect::Item( TXT( "Capsule" ), str.str() ) );
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
