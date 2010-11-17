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

static int32_t g_InitCount = 0;
static Helium::InitializerStack g_InitializerStack;

void SceneGraph::Initialize()
{
    if ( ++g_InitCount == 1 )
    {
        // core library initiailization
        g_InitializerStack.Push( Reflect::Initialize, Reflect::Cleanup );
        g_InitializerStack.Push( Component::Initialize, Component::Cleanup );
        g_InitializerStack.Push( Asset::Initialize, Asset::Cleanup );

        g_InitializerStack.Push( PropertiesGenerator::Initialize, PropertiesGenerator::Cleanup );
        g_InitializerStack.Push( Reflect::RegisterClassType<MRUData>( TXT("SceneGraph::MRUData") ) );

        g_InitializerStack.Push( Reflect::RegisterEnumType<CameraMode>( &CameraModes::CameraModeEnumerateEnum, TXT( "SceneGraph::CameraMode" ) ) ); 
        g_InitializerStack.Push( Reflect::RegisterEnumType<GeometryMode>( &GeometryModes::GeometryModeEnumerateEnum, TXT( "SceneGraph::GeometryMode" ) ) ); 
        g_InitializerStack.Push( Reflect::RegisterEnumType<ViewColorMode>( &ViewColorModes::ViewColorModeEnumerateEnum, TXT( "SceneGraph::ViewColorMode" ) ) ); 
        g_InitializerStack.Push( Reflect::RegisterEnumType<GridUnit>( &GridUnits::GridUnitEnumerateEnum, TXT( "SceneGraph::GridUnit" ) ) ); 
        g_InitializerStack.Push( Reflect::RegisterEnumType<ManipulatorSpace>( &ManipulatorSpaces::ManipulatorSpaceEnumerateEnum, TXT( "SceneGraph::ManipulatorSpace" ) ) ); 
        g_InitializerStack.Push( Reflect::RegisterEnumType<TranslateSnappingMode>( &TranslateSnappingModes::TranslateSnappingModeEnumerateEnum, TXT( "SceneGraph::TranslateSnappingMode" ) ) ); 
        g_InitializerStack.Push( Reflect::RegisterEnumType<ShadingMode>( &ShadingModes::EnumerateEnum, TXT( "SceneGraph::ShadingMode" ) ) );
        g_InitializerStack.Push( Reflect::RegisterEnumType<VolumeShape>( &VolumeShapes::VolumeShapeEnumerateEnum, TXT( "SceneGraph::VolumeShape" ) ) );
        g_InitializerStack.Push( Reflect::RegisterEnumType<LocatorShape>( &LocatorShapes::LocatorShapeEnumerateEnum, TXT( "SceneGraph::LocatorShape" ) ) );
        g_InitializerStack.Push( Reflect::RegisterEnumType<CurveType>( &CurveTypes::CurveTypeEnumerateEnum, TXT( "SceneGraph::CurveType" ) ) );
        g_InitializerStack.Push( Reflect::RegisterEnumType<ControlPointLabel>( &ControlPointLabels::ControlPointLabelEnumerateEnum, TXT( "SceneGraph::ControlPointLabel" ) ) );

        g_InitializerStack.Push( Viewport::InitializeType, Viewport::CleanupType );
        g_InitializerStack.Push( Primitive::InitializeType, Primitive::CleanupType );

        g_InitializerStack.Push( Tool::InitializeType, Tool::CleanupType );
        g_InitializerStack.Push( CreateTool::InitializeType, CreateTool::CleanupType );
        g_InitializerStack.Push( DuplicateTool::InitializeType, DuplicateTool::CleanupType );

        g_InitializerStack.Push( TransformManipulator::InitializeType, TransformManipulator::CleanupType );
        g_InitializerStack.Push( TranslateManipulator::InitializeType, TranslateManipulator::CleanupType );
        g_InitializerStack.Push( RotateManipulator::InitializeType, RotateManipulator::CleanupType );
        g_InitializerStack.Push( ScaleManipulator::InitializeType, ScaleManipulator::CleanupType );

        g_InitializerStack.Push( Graph::InitializeType, Graph::CleanupType );
        g_InitializerStack.Push( SceneNode::InitializeType, SceneNode::CleanupType );
        g_InitializerStack.Push( SceneNodeType::InitializeType, SceneNodeType::CleanupType );
        g_InitializerStack.Push( HierarchyNode::InitializeType, HierarchyNode::CleanupType );
        g_InitializerStack.Push( HierarchyNodeType::InitializeType, HierarchyNodeType::CleanupType );
        g_InitializerStack.Push( Transform::InitializeType, Transform::CleanupType );
        g_InitializerStack.Push( JointTransform::InitializeType, JointTransform::CleanupType );
        g_InitializerStack.Push( PivotTransform::InitializeType, PivotTransform::CleanupType );
        g_InitializerStack.Push( Layer::InitializeType, Layer::CleanupType );
        g_InitializerStack.Push( Shader::InitializeType, Shader::CleanupType );
        g_InitializerStack.Push( Mesh::InitializeType, Mesh::CleanupType );
        g_InitializerStack.Push( Skin::InitializeType, Skin::CleanupType );
        g_InitializerStack.Push( CurveControlPoint::InitializeType, CurveControlPoint::CleanupType );
        g_InitializerStack.Push( Curve::InitializeType, Curve::CleanupType );
        g_InitializerStack.Push( CurveCreateTool::InitializeType, CurveCreateTool::CleanupType );
        g_InitializerStack.Push( CurveEditTool::InitializeType, CurveEditTool::CleanupType );
        g_InitializerStack.Push( Instance::InitializeType, Instance::CleanupType );
        g_InitializerStack.Push( InstanceSet::InitializeType, InstanceSet::CleanupType );
        g_InitializerStack.Push( InstanceType::InitializeType, InstanceType::CleanupType );
        g_InitializerStack.Push( Volume::InitializeType, Volume::CleanupType );
        g_InitializerStack.Push( VolumeType::InitializeType, VolumeType::CleanupType );
        g_InitializerStack.Push( VolumeCreateTool::InitializeType, VolumeCreateTool::CleanupType );
        g_InitializerStack.Push( Locator::InitializeType, Locator::CleanupType );
        g_InitializerStack.Push( LocatorType::InitializeType, LocatorType::CleanupType );
        g_InitializerStack.Push( LocatorCreateTool::InitializeType, LocatorCreateTool::CleanupType );
        g_InitializerStack.Push( EntityInstance::InitializeType, EntityInstance::CleanupType );
        g_InitializerStack.Push( EntitySet::InitializeType, EntitySet::CleanupType );
        g_InitializerStack.Push( EntityInstanceType::InitializeType, EntityInstanceType::CleanupType );
        g_InitializerStack.Push( EntityInstanceCreateTool::InitializeType, EntityInstanceCreateTool::CleanupType );
        g_InitializerStack.Push( Light::InitializeType, Light::CleanupType );

        g_InitializerStack.Push( Reflect::RegisterClassType< SceneGraph::CameraSettings >( TXT("SceneGraph::CameraSettings") ) ); 
        g_InitializerStack.Push( Reflect::RegisterClassType< SceneGraph::ViewportSettings >( TXT("SceneGraph::ViewportSettings") ) ); 
        g_InitializerStack.Push( Reflect::RegisterClassType< SceneGraph::GridSettings >( TXT("SceneGraph::GridSettings") ) );
        g_InitializerStack.Push( Reflect::RegisterClassType< SceneGraph::SceneSettings >( TXT("SceneGraph::SceneSettings") ) );
    }
}

void SceneGraph::Cleanup()
{
    if ( --g_InitCount == 0 )
    {
        g_InitializerStack.Cleanup();
    }
}
