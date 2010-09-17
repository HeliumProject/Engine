/*#include "Precompile.h"*/
#include "SceneInit.h"

#include "Foundation/InitializerStack.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/Component/ComponentInit.h"

#include "Core/Asset/AssetInit.h"

#include "Core/Scene/SceneSettings.h"
#include "Core/Scene/Tool.h"
#include "Core/Scene/CreateTool.h"
#include "Core/Scene/DuplicateTool.h"
#include "Core/Scene/ScaleManipulator.h"
#include "Core/Scene/RotateManipulator.h"
#include "Core/Scene/TranslateManipulator.h"
#include "Core/Scene/HierarchyNodeType.h"
#include "Core/Scene/JointTransform.h"
#include "Core/Scene/PivotTransform.h"
#include "Core/Scene/Layer.h"
#include "Core/Scene/Shader.h"
#include "Core/Scene/Mesh.h"
#include "Core/Scene/Skin.h"
#include "Core/Scene/Curve.h"
#include "Core/Scene/CurveControlPoint.h"
#include "Core/Scene/CurveCreateTool.h"
#include "Core/Scene/CurveEditTool.h"
#include "Core/Scene/Instance.h"
#include "Core/Scene/InstanceType.h"
#include "Core/Scene/EntityInstance.h"
#include "Core/Scene/EntityInstanceType.h"
#include "Core/Scene/EntityInstanceCreateTool.h"
#include "Core/Scene/EntitySet.h"
#include "Core/Scene/Locator.h"
#include "Core/Scene/LocatorType.h"
#include "Core/Scene/LocatorCreateTool.h"
#include "Core/Scene/Volume.h"
#include "Core/Scene/VolumeType.h"
#include "Core/Scene/VolumeCreateTool.h"
#include "Core/Scene/Light.h"
#include "Core/Scene/ViewportSettings.h"
#include "Core/Scene/SceneSettings.h"
#include "Core/Scene/GridSettings.h"

using namespace Helium;
using namespace Helium::Core;

static i32 g_InitCount = 0;
static Helium::InitializerStack g_InitializerStack;

void Core::SceneInitialize()
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

        g_InitializerStack.Push( SceneGraph::InitializeType, SceneGraph::CleanupType );
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
        g_InitializerStack.Push( EntityType::InitializeType, EntityType::CleanupType );
        g_InitializerStack.Push( EntityInstanceCreateTool::InitializeType, EntityInstanceCreateTool::CleanupType );
        g_InitializerStack.Push( Light::InitializeType, Light::CleanupType );

        g_InitializerStack.Push( Reflect::RegisterClassType< Core::SettingsManager >() ); 
        g_InitializerStack.Push( Reflect::RegisterClassType< Core::CameraSettings >() ); 
        g_InitializerStack.Push( Reflect::RegisterClassType< Core::ViewportSettings >() ); 
        g_InitializerStack.Push( Reflect::RegisterClassType< Core::GridSettings >() );
        g_InitializerStack.Push( Reflect::RegisterClassType< Core::SceneSettings >() );
    }
}

void Core::SceneCleanup()
{
    if ( --g_InitCount == 0 )
    {
        g_InitializerStack.Cleanup();
    }
}
