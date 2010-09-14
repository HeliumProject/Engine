/*#include "Precompile.h"*/
#include "SceneInit.h"

// Libraries
#include "Foundation/InitializerStack.h"
#include "Foundation/Reflect/Registry.h"
#include "Core/Content/ContentInit.h"
#include "Core/Content/ContentVersion.h"
#include "Core/Asset/AssetInit.h"

// Types
#include "SceneSettings.h"

#include "Core/Scene/Tool.h"
#include "CreateTool.h"
#include "DuplicateTool.h"

#include "ScaleManipulator.h"
#include "RotateManipulator.h"
#include "TranslateManipulator.h"

#include "HierarchyNodeType.h"
#include "JointTransform.h"
#include "PivotTransform.h"

#include "Layer.h"
#include "Shader.h"
#include "Core/Scene/Mesh.h"
#include "Skin.h"

#include "Curve.h"
#include "Core/Scene/Point.h"
#include "CurveCreateTool.h"
#include "CurveEditTool.h"

#include "Instance.h"
#include "InstanceType.h"

#include "EntityInstance.h"
#include "EntityInstanceType.h"
#include "EntityInstanceCreateTool.h"
#include "EntitySet.h"

#include "Locator.h"
#include "LocatorType.h"
#include "LocatorCreateTool.h"

#include "Volume.h"
#include "VolumeType.h"
#include "VolumeCreateTool.h"

#include "Light.h"
#include "SpotLight.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "AmbientLight.h"

#include "ViewportSettings.h"
#include "SceneSettings.h"
#include "GridSettings.h"

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
    g_InitializerStack.Push( Content::Initialize, Content::Cleanup );
    g_InitializerStack.Push( Asset::Initialize, Asset::Cleanup );

    g_InitializerStack.Push( PropertiesGenerator::Initialize, PropertiesGenerator::Cleanup );

    g_InitializerStack.Push( Reflect::RegisterClassType<Object>() );
    g_InitializerStack.Push( Reflect::RegisterClassType<Selectable>() );
    g_InitializerStack.Push( Reflect::RegisterClassType<Persistent>() );
    g_InitializerStack.Push( Reflect::RegisterClassType<MRUData>() );

    g_InitializerStack.Push( Reflect::RegisterEnumType<CameraModes::CameraMode>( &CameraModes::CameraModeEnumerateEnum, TXT( "CameraMode" ) ) ); 
    g_InitializerStack.Push( Reflect::RegisterEnumType<GeometryModes::GeometryMode>( &GeometryModes::GeometryModeEnumerateEnum, TXT( "GeometryMode" ) ) ); 
    g_InitializerStack.Push( Reflect::RegisterEnumType<ViewColorModes::ViewColorMode>( &ViewColorModes::ViewColorModeEnumerateEnum, TXT( "ViewColorMode" ) ) ); 
    g_InitializerStack.Push( Reflect::RegisterEnumType<GridUnits::GridUnit>( &GridUnits::GridUnitEnumerateEnum, TXT( "GridUnit" ) ) ); 
    g_InitializerStack.Push( Reflect::RegisterEnumType<ManipulatorSpaces::ManipulatorSpace>( &ManipulatorSpaces::ManipulatorSpaceEnumerateEnum, TXT( "ManipulatorSpace" ) ) ); 
    g_InitializerStack.Push( Reflect::RegisterEnumType<TranslateSnappingModes::TranslateSnappingMode>( &TranslateSnappingModes::TranslateSnappingModeEnumerateEnum, TXT( "TranslateSnappingMode" ) ) ); 
    g_InitializerStack.Push( Reflect::RegisterEnumType<ShadingMode>( &ShadingModes::EnumerateEnum, TXT( "ShadingMode" ) ) );

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
    g_InitializerStack.Push( Point::InitializeType, Point::CleanupType );
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
    g_InitializerStack.Push( SpotLight::InitializeType, SpotLight::CleanupType );
    g_InitializerStack.Push( PointLight::InitializeType, PointLight::CleanupType );
    g_InitializerStack.Push( DirectionalLight::InitializeType, DirectionalLight::CleanupType );
    g_InitializerStack.Push( AmbientLight::InitializeType, AmbientLight::CleanupType );

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
