/*#include "Precompile.h"*/
#include "SceneGraphInit.h"

#include "Foundation/InitializerStack.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/Component/ComponentInit.h"

#include "Pipeline/Asset/AssetInit.h"

#include "Pipeline/SceneGraph/SceneSettings.h"
#include "Pipeline/SceneGraph/Tool.h"
#include "Pipeline/SceneGraph/CreateTool.h"
#include "Pipeline/SceneGraph/DuplicateTool.h"
#include "Pipeline/SceneGraph/ScaleManipulator.h"
#include "Pipeline/SceneGraph/RotateManipulator.h"
#include "Pipeline/SceneGraph/TranslateManipulator.h"
#include "Pipeline/SceneGraph/HierarchyNodeType.h"
#include "Pipeline/SceneGraph/JointTransform.h"
#include "Pipeline/SceneGraph/PivotTransform.h"
#include "Pipeline/SceneGraph/Layer.h"
#include "Pipeline/SceneGraph/Shader.h"
#include "Pipeline/SceneGraph/Mesh.h"
#include "Pipeline/SceneGraph/Skin.h"
#include "Pipeline/SceneGraph/Curve.h"
#include "Pipeline/SceneGraph/CurveControlPoint.h"
#include "Pipeline/SceneGraph/CurveCreateTool.h"
#include "Pipeline/SceneGraph/CurveEditTool.h"
#include "Pipeline/SceneGraph/Instance.h"
#include "Pipeline/SceneGraph/InstanceType.h"
#include "Pipeline/SceneGraph/EntityInstance.h"
#include "Pipeline/SceneGraph/EntityInstanceType.h"
#include "Pipeline/SceneGraph/EntityInstanceCreateTool.h"
#include "Pipeline/SceneGraph/EntitySet.h"
#include "Pipeline/SceneGraph/Locator.h"
#include "Pipeline/SceneGraph/LocatorType.h"
#include "Pipeline/SceneGraph/LocatorCreateTool.h"
#include "Pipeline/SceneGraph/Volume.h"
#include "Pipeline/SceneGraph/VolumeType.h"
#include "Pipeline/SceneGraph/VolumeCreateTool.h"
#include "Pipeline/SceneGraph/Light.h"
#include "Pipeline/SceneGraph/ViewportSettings.h"
#include "Pipeline/SceneGraph/SceneSettings.h"
#include "Pipeline/SceneGraph/GridSettings.h"

using namespace Helium;
using namespace Helium::SceneGraph;

static Helium::InitializerStack g_SceneGraphInitStack;

void SceneGraph::Initialize()
{
    if ( g_SceneGraphInitStack.Increment() == 1 )
    {
        // core library initiailization
        g_SceneGraphInitStack.Push( Reflect::Initialize, Reflect::Cleanup );
        g_SceneGraphInitStack.Push( Component::Initialize, Component::Cleanup );
        g_SceneGraphInitStack.Push( Asset::Initialize, Asset::Cleanup );

        g_SceneGraphInitStack.Push( PropertiesGenerator::Initialize, PropertiesGenerator::Cleanup );
        g_SceneGraphInitStack.Push( Reflect::RegisterClassType<MRUData>( TXT("SceneGraph::MRUData") ) );

        g_SceneGraphInitStack.Push( Viewport::InitializeType, Viewport::CleanupType );
        g_SceneGraphInitStack.Push( Primitive::InitializeType, Primitive::CleanupType );

        g_SceneGraphInitStack.Push( Tool::InitializeType, Tool::CleanupType );
        g_SceneGraphInitStack.Push( CreateTool::InitializeType, CreateTool::CleanupType );
        g_SceneGraphInitStack.Push( DuplicateTool::InitializeType, DuplicateTool::CleanupType );

        g_SceneGraphInitStack.Push( Reflect::RegisterEnumType<ManipulatorSpace>( TXT( "SceneGraph::ManipulatorSpace" ) ) ); 
        g_SceneGraphInitStack.Push( TransformManipulator::InitializeType, TransformManipulator::CleanupType );
        g_SceneGraphInitStack.Push( Reflect::RegisterEnumType<TranslateSnappingMode>( TXT( "SceneGraph::TranslateSnappingMode" ) ) ); 
        g_SceneGraphInitStack.Push( TranslateManipulator::InitializeType, TranslateManipulator::CleanupType );
        g_SceneGraphInitStack.Push( RotateManipulator::InitializeType, RotateManipulator::CleanupType );
        g_SceneGraphInitStack.Push( ScaleManipulator::InitializeType, ScaleManipulator::CleanupType );

        g_SceneGraphInitStack.Push( Graph::InitializeType, Graph::CleanupType );
        g_SceneGraphInitStack.Push( SceneNode::InitializeType, SceneNode::CleanupType );
        g_SceneGraphInitStack.Push( SceneNodeType::InitializeType, SceneNodeType::CleanupType );
        g_SceneGraphInitStack.Push( HierarchyNode::InitializeType, HierarchyNode::CleanupType );
        g_SceneGraphInitStack.Push( HierarchyNodeType::InitializeType, HierarchyNodeType::CleanupType );
        g_SceneGraphInitStack.Push( Transform::InitializeType, Transform::CleanupType );
        g_SceneGraphInitStack.Push( JointTransform::InitializeType, JointTransform::CleanupType );
        g_SceneGraphInitStack.Push( PivotTransform::InitializeType, PivotTransform::CleanupType );
        g_SceneGraphInitStack.Push( Layer::InitializeType, Layer::CleanupType );

        g_SceneGraphInitStack.Push( Shader::InitializeType, Shader::CleanupType );
        g_SceneGraphInitStack.Push( Mesh::InitializeType, Mesh::CleanupType );
        g_SceneGraphInitStack.Push( Skin::InitializeType, Skin::CleanupType );

        g_SceneGraphInitStack.Push( Reflect::RegisterEnumType<CurveType>( TXT( "SceneGraph::CurveType" ) ) );
        g_SceneGraphInitStack.Push( Reflect::RegisterEnumType<ControlPointLabel>( TXT( "SceneGraph::ControlPointLabel" ) ) );
        g_SceneGraphInitStack.Push( CurveControlPoint::InitializeType, CurveControlPoint::CleanupType );
        g_SceneGraphInitStack.Push( Curve::InitializeType, Curve::CleanupType );
        g_SceneGraphInitStack.Push( CurveCreateTool::InitializeType, CurveCreateTool::CleanupType );
        g_SceneGraphInitStack.Push( CurveEditTool::InitializeType, CurveEditTool::CleanupType );

        g_SceneGraphInitStack.Push( Instance::InitializeType, Instance::CleanupType );
        g_SceneGraphInitStack.Push( InstanceSet::InitializeType, InstanceSet::CleanupType );
        g_SceneGraphInitStack.Push( InstanceType::InitializeType, InstanceType::CleanupType );

        g_SceneGraphInitStack.Push( Light::InitializeType, Light::CleanupType );

        g_SceneGraphInitStack.Push( Reflect::RegisterEnumType<VolumeShape>( TXT( "SceneGraph::VolumeShape" ) ) );
        g_SceneGraphInitStack.Push( Volume::InitializeType, Volume::CleanupType );
        g_SceneGraphInitStack.Push( VolumeType::InitializeType, VolumeType::CleanupType );
        g_SceneGraphInitStack.Push( VolumeCreateTool::InitializeType, VolumeCreateTool::CleanupType );

        g_SceneGraphInitStack.Push( Reflect::RegisterEnumType<LocatorShape>( TXT( "SceneGraph::LocatorShape" ) ) );
        g_SceneGraphInitStack.Push( Locator::InitializeType, Locator::CleanupType );
        g_SceneGraphInitStack.Push( LocatorType::InitializeType, LocatorType::CleanupType );
        g_SceneGraphInitStack.Push( LocatorCreateTool::InitializeType, LocatorCreateTool::CleanupType );

        g_SceneGraphInitStack.Push( EntityInstance::InitializeType, EntityInstance::CleanupType );
        g_SceneGraphInitStack.Push( EntitySet::InitializeType, EntitySet::CleanupType );
        g_SceneGraphInitStack.Push( EntityInstanceType::InitializeType, EntityInstanceType::CleanupType );
        g_SceneGraphInitStack.Push( EntityInstanceCreateTool::InitializeType, EntityInstanceCreateTool::CleanupType );

        g_SceneGraphInitStack.Push( Reflect::RegisterEnumType<CameraMode>( TXT( "SceneGraph::CameraMode" ) ) ); 
        g_SceneGraphInitStack.Push( Reflect::RegisterEnumType<ShadingMode>( TXT( "SceneGraph::ShadingMode" ) ) );
        g_SceneGraphInitStack.Push( Reflect::RegisterClassType< SceneGraph::CameraSettings >( TXT("SceneGraph::CameraSettings") ) ); 

        g_SceneGraphInitStack.Push( Reflect::RegisterEnumType<GeometryMode>( TXT( "SceneGraph::GeometryMode" ) ) ); 
        g_SceneGraphInitStack.Push( Reflect::RegisterEnumType<ViewColorMode>( TXT( "SceneGraph::ViewColorMode" ) ) ); 
        g_SceneGraphInitStack.Push( Reflect::RegisterClassType< SceneGraph::ViewportSettings >( TXT("SceneGraph::ViewportSettings") ) ); 

        g_SceneGraphInitStack.Push( Reflect::RegisterEnumType<GridUnit>( TXT( "SceneGraph::GridUnit" ) ) ); 
        g_SceneGraphInitStack.Push( Reflect::RegisterClassType< SceneGraph::GridSettings >( TXT("SceneGraph::GridSettings") ) );

        g_SceneGraphInitStack.Push( Reflect::RegisterClassType< SceneGraph::SceneSettings >( TXT("SceneGraph::SceneSettings") ) );
    }
}

void SceneGraph::Cleanup()
{
    g_SceneGraphInitStack.Decrement();
}
