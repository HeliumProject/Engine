/*#include "Precompile.h"*/
#include "VolumeCreateTool.h"

#include "Pipeline/SceneGraph/Mesh.h"
#include "Pipeline/SceneGraph/Scene.h"
#include "Volume.h"
#include "VolumeType.h"

#include "Pipeline/SceneGraph/Pick.h"

using namespace Helium;
using namespace Helium::SceneGraph;

VolumeShape VolumeCreateTool::s_Shape = VolumeShape::Cube;

REFLECT_DEFINE_ABSTRACT(SceneGraph::VolumeCreateTool);

void VolumeCreateTool::InitializeType()
{
    Reflect::RegisterClassType< SceneGraph::VolumeCreateTool >( TXT( "SceneGraph::VolumeCreateTool" ) );
}

void VolumeCreateTool::CleanupType()
{
    Reflect::UnregisterClassType< SceneGraph::VolumeCreateTool >();
}

VolumeCreateTool::VolumeCreateTool(SceneGraph::Scene* scene, PropertiesGenerator* generator)
: SceneGraph::CreateTool (scene, generator)
{

}

VolumeCreateTool::~VolumeCreateTool()
{

}

SceneGraph::TransformPtr VolumeCreateTool::CreateNode()
{
#ifdef SCENE_DEBUG_RUNTIME_DATA_SELECTION

    VolumePtr v = new Volume( s_Shape );

    v->RectifyRuntimeData();

    VolumePtr volume = new SceneGraph::Volume( m_Scene, v );

    m_Scene->AddObject( volume );

    {
        OS_SceneNodeDumbPtr selection;
        selection.push_back( volume );
        m_Scene->GetSelection().SetItems( selection );

        m_Scene->GetSelection().Clear();
    }

    m_Scene->RemoveObject( volume );

    return volume;

#else

    VolumePtr volume = new Volume();
    volume->SetOwner( m_Scene );
    volume->Initialize();
    volume->SetShape( (int)s_Shape );
    return volume;

#endif
}

void VolumeCreateTool::CreateProperties()
{
    m_Generator->PushContainer( TXT( "Volume" ) );
    {
        m_Generator->PushContainer();
        {
            m_Generator->AddLabel( TXT( "Shape" ) );

            Inspect::Choice* choice = m_Generator->AddChoice<int>( new Helium::MemberProperty<SceneGraph::VolumeCreateTool, int>(this, &VolumeCreateTool::GetVolumeShape, &VolumeCreateTool::SetVolumeShape) );
            choice->a_IsDropDown.Set( true );
            std::vector< Inspect::ChoiceItem > items;

            {
                tostringstream str;
                str << VolumeShape::Cube;
                items.push_back( Inspect::ChoiceItem( TXT( "Cube" ), str.str() ) );
            }

            {
                tostringstream str;
                str << VolumeShape::Cylinder;
                items.push_back( Inspect::ChoiceItem( TXT( "Cylinder" ), str.str() ) );
            }

            {
                tostringstream str;
                str << VolumeShape::Sphere;
                items.push_back( Inspect::ChoiceItem( TXT( "Sphere" ), str.str() ) );
            }

            {
                tostringstream str;
                str << VolumeShape::Capsule;
                items.push_back( Inspect::ChoiceItem( TXT( "Capsule" ), str.str() ) );
            }

            choice->a_Items.Set( items );
        }
        m_Generator->Pop();
    }
    m_Generator->Pop();

    Base::CreateProperties();
}

int VolumeCreateTool::GetVolumeShape() const
{
    return (int)s_Shape;
}

void VolumeCreateTool::SetVolumeShape(int value)
{
    s_Shape = static_cast< VolumeShape > (value);

    Place(Matrix4::Identity);
}
