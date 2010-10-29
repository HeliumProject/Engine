/*#include "Precompile.h"*/
#include "SceneGraphInit.h"

#include "Foundation/InitializerStack.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/Component/ComponentInit.h"

#include "Core/Asset/AssetInit.h"

#include "Core/SceneGraph/SceneSettings.h"
#include "Core/SceneGraph/Tool.h"
#include "Core/SceneGraph/CreateTool.h"
#include "Core/SceneGraph/DuplicateTool.h"
#include "Core/SceneGraph/ScaleManipulator.h"
#include "Core/SceneGraph/RotateManipulator.h"
#include "Core/SceneGraph/TranslateManipulator.h"
#include "Core/SceneGraph/HierarchyNodeType.h"
#include "Core/SceneGraph/JointTransform.h"
#include "Core/SceneGraph/PivotTransform.h"
#include "Core/SceneGraph/Layer.h"
#include "Core/SceneGraph/Shader.h"
#include "Core/SceneGraph/Mesh.h"
#include "Core/SceneGraph/Skin.h"
#include "Core/SceneGraph/Curve.h"
#include "Core/SceneGraph/CurveControlPoint.h"
#include "Core/SceneGraph/CurveCreateTool.h"
#include "Core/SceneGraph/CurveEditTool.h"
#include "Core/SceneGraph/Instance.h"
#include "Core/SceneGraph/InstanceType.h"
#include "Core/SceneGraph/EntityInstance.h"
#include "Core/SceneGraph/EntityInstanceType.h"
#include "Core/SceneGraph/EntityInstanceCreateTool.h"
#include "Core/SceneGraph/EntitySet.h"
#include "Core/SceneGraph/Locator.h"
#include "Core/SceneGraph/LocatorType.h"
#include "Core/SceneGraph/LocatorCreateTool.h"
#include "Core/SceneGraph/Volume.h"
#include "Core/SceneGraph/VolumeType.h"
#include "Core/SceneGraph/VolumeCreateTool.h"
#include "Core/SceneGraph/Light.h"
#include "Core/SceneGraph/ViewportSettings.h"
#include "Core/SceneGraph/SceneSettings.h"
#include "Core/SceneGraph/GridSettings.h"

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
        g_InitializerStack.Push( Reflect::RegisterClassType<MRUData>() );

        g_InitializerStack.Push( Reflect::RegisterEnumType<CameraMode>( &CameraModes::CameraModeEnumerateEnum, TXT( "CameraMode" ) ) ); 
        g_InitializerStack.Push( Reflect::RegisterEnumType<GeometryMode>( &GeometryModes::GeometryModeEnumerateEnum, TXT( "GeometryMode" ) ) ); 
        g_InitializerStack.Push( Reflect::RegisterEnumType<ViewColorMode>( &ViewColorModes::ViewColorModeEnumerateEnum, TXT( "ViewColorMode" ) ) ); 
        g_InitializerStack.Push( Reflect::RegisterEnumType<GridUnit>( &GridUnits::GridUnitEnumerateEnum, TXT( "GridUnit" ) ) ); 
        g_InitializerStack.Push( Reflect::RegisterEnumType<ManipulatorSpace>( &ManipulatorSpaces::ManipulatorSpaceEnumerateEnum, TXT( "ManipulatorSpace" ) ) ); 
        g_InitializerStack.Push( Reflect::RegisterEnumType<TranslateSnappingMode>( &TranslateSnappingModes::TranslateSnappingModeEnumerateEnum, TXT( "TranslateSnappingMode" ) ) ); 
        g_InitializerStack.Push( Reflect::RegisterEnumType<ShadingMode>( &ShadingModes::EnumerateEnum, TXT( "ShadingMode" ) ) );
        g_InitializerStack.Push( Reflect::RegisterEnumType<VolumeShape>( &VolumeShapes::VolumeShapeEnumerateEnum, TXT( "VolumeShape" ) ) );
        g_InitializerStack.Push( Reflect::RegisterEnumType<LocatorShape>( &LocatorShapes::LocatorShapeEnumerateEnum, TXT( "LocatorShape" ) ) );
        g_InitializerStack.Push( Reflect::RegisterEnumType<CurveType>( &CurveTypes::CurveTypeEnumerateEnum, TXT( "CurveType" ) ) );
        g_InitializerStack.Push( Reflect::RegisterEnumType<ControlPointLabel>( &ControlPointLabels::ControlPointLabelEnumerateEnum, TXT( "ControlPointLabel" ) ) );

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

        g_InitializerStack.Push( Reflect::RegisterClassType< SceneGraph::CameraSettings >() ); 
        g_InitializerStack.Push( Reflect::RegisterClassType< SceneGraph::ViewportSettings >() ); 
        g_InitializerStack.Push( Reflect::RegisterClassType< SceneGraph::GridSettings >() );
        g_InitializerStack.Push( Reflect::RegisterClassType< SceneGraph::SceneSettings >() );
    }
}

void SceneGraph::Cleanup()
{
    if ( --g_InitCount == 0 )
    {
        g_InitializerStack.Cleanup();
    }
}
