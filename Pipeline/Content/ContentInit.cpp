#include "ContentInit.h"
#include "ContentVersion.h"
#include "ContentTypes.h"

#include "Foundation/Reflect/Registry.h"

#include "Pipeline/Content/Nodes/SceneNode.h"
#include "Pipeline/Content/Nodes/HierarchyNode.h"
#include "Pipeline/Content/Nodes/Hierarchy.h"

#include "Pipeline/Content/Nodes/Transform/Transform.h"
#include "Pipeline/Content/Nodes/Transform/PivotTransform.h"
#include "Pipeline/Content/Nodes/Transform/JointTransform.h"
#include "Pipeline/Content/Nodes/Transform/Descriptor.h"
#include "Pipeline/Content/Nodes/Transform/Effector.h"

#include "Pipeline/Content/Nodes/Instance/Instance.h"
#include "Pipeline/Content/Nodes/Instance/Locator.h"
#include "Pipeline/Content/Nodes/Instance/Volume.h"

#include "Pipeline/Content/Nodes/Camera.h"
#include "Pipeline/Content/Nodes/Layer.h"
#include "Pipeline/Content/Nodes/Zone.h"
#include "Pipeline/Content/Nodes/Region.h"
#include "Pipeline/Content/Nodes/Curve/Curve.h"
#include "Pipeline/Content/Nodes/Curve/Point.h"
#include "Pipeline/Content/Nodes/Geometry/Mesh.h"
#include "Pipeline/Content/Nodes/Geometry/Skin.h"
#include "Pipeline/Content/Nodes/Geometry/Shader.h"
#include "Pipeline/Content/Nodes/Geometry/CollisionPrimitive.h"

#include "Pipeline/Content/ParametricKey/ParametricColorKey.h"
#include "Pipeline/Content/ParametricKey/ParametricIntensityKey.h"
#include "Pipeline/Content/ParametricKey/ParametricKey.h"

#include "Pipeline/Content/Animation/Animation.h"
#include "Pipeline/Content/Animation/AnimationClip.h"
#include "Pipeline/Content/Animation/CompressedAnimation.h"
#include "Pipeline/Content/Animation/JointOrdering.h"

#include "Pipeline/Content/Nodes/Lights/Light.h"
#include "Pipeline/Content/Nodes/Lights/SpotLight.h"
#include "Pipeline/Content/Nodes/Lights/PointLight.h"
#include "Pipeline/Content/Nodes/Lights/DirectionalLight.h"
#include "Pipeline/Content/Nodes/Lights/ShadowDirection.h"
#include "Pipeline/Content/Nodes/Lights/SunLight.h"
#include "Pipeline/Content/Nodes/Lights/AreaLights/AreaLight.h"
#include "Pipeline/Content/Nodes/Lights/AreaLights/RadiusLight.h"
#include "Pipeline/Content/Nodes/Lights/AreaLights/SphereLight.h"
#include "Pipeline/Content/Nodes/Lights/AreaLights/RectangleLight.h"
#include "Pipeline/Content/Nodes/Lights/AreaLights/CylinderLight.h"
#include "Pipeline/Content/Nodes/Lights/AreaLights/DiscLight.h"
#include "Pipeline/Content/Nodes/Lights/PortalLight.h"
#include "Pipeline/Content/Nodes/Lights/AmbientLight.h"
#include "Pipeline/Content/Nodes/Lights/AmbientVolumeLight.h"

#include "Pipeline/Content/NodeVisibility.h"
#include "Pipeline/Content/SceneVisibility.h"

#include "Foundation/InitializerStack.h"
#include "Pipeline/Component/ComponentInit.h"
#include "Pipeline/Component/ComponentCategories.h"

#include "Foundation/InitializerStack.h"

using namespace Reflect;
using namespace Math;

namespace Content
{
  i32 g_InitCount = 0;
  Nocturnal::InitializerStack g_InitializerStack;

  void Initialize()
  {
    if ( ++g_InitCount == 1 )
    {
      g_InitializerStack.Push( Reflect::Initialize, Reflect::Cleanup );
      g_InitializerStack.Push( Component::Initialize, Component::Cleanup );

      g_InitializerStack.Push( Reflect::RegisterClass<ContentVersion>( TXT( "ContentVersion" ) ) );
      g_InitializerStack.Push( Reflect::RegisterEnumeration<ContentTypes::ContentType>( &ContentTypes::ContentTypeEnumerateEnumeration, TXT( "ContentType" ) ) );

      g_InitializerStack.Push( Reflect::RegisterClass<SceneNode>( TXT( "SceneNode" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<HierarchyNode>( TXT( "HierarchyNode" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Hierarchy>( TXT( "Hierarchy" ) ) );

      g_InitializerStack.Push( Reflect::RegisterClass<Transform>( TXT( "Transform" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<PivotTransform>( TXT( "PivotTransform" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<JointTransform>( TXT( "JointTransform" ) ) );

      g_InitializerStack.Push( Reflect::RegisterClass<Instance>( TXT( "Instance" ) ) );

      g_InitializerStack.Push( Reflect::RegisterEnumeration<VolumeShapes::VolumeShape>( &VolumeShapes::VolumeShapeEnumerateEnumeration, TXT( "VolumeShape" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Volume>( TXT( "Volume" ) ) );

      g_InitializerStack.Push( Reflect::RegisterEnumeration<LocatorShapes::LocatorShape>( &LocatorShapes::LocatorShapeEnumerateEnumeration, TXT( "LocatorShape" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Locator>( TXT( "Locator" ) ) );

      g_InitializerStack.Push( Reflect::RegisterClass<Camera>( TXT( "Camera" ) ) );

      g_InitializerStack.Push( Reflect::RegisterClass<Layer>( TXT( "Layer" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Zone>( TXT( "Zone" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Region>( TXT( "Region" ) ) );

      g_InitializerStack.Push( Reflect::RegisterClass<MorphTargetDelta>( TXT( "MorphTargetDelta" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<MorphTarget>( TXT( "MorphTarget" ) ) );

      g_InitializerStack.Push( Reflect::RegisterEnumeration<Mesh::MeshOriginType>( &Mesh::MeshOriginTypeEnumeration, TXT( "MeshOriginType" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Mesh>( TXT( "Mesh" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Shader>( TXT( "Shader" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Effector>( TXT( "Effector" ) ) );

      g_InitializerStack.Push( Reflect::RegisterEnumeration<CollisionShapes::CollisionShape>( &CollisionShapes::CollisionShapeEnumerateEnumeration, TXT( "CollisionShape" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<CollisionPrimitive>( TXT( "CollisionPrimitive" ) ) );
      Reflect::Registry::GetInstance()->AliasType( Reflect::GetClass<CollisionPrimitive>(), TXT( "CollisionVolume" ) );

      g_InitializerStack.Push( Reflect::RegisterClass<Influence>( TXT( "Influence" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Skin>( TXT( "Skin" ) ) );

      g_InitializerStack.Push( Reflect::RegisterClass<Point>( TXT( "Point" ) ) );
      g_InitializerStack.Push( Reflect::RegisterEnumeration<CurveTypes::CurveType>( &CurveTypes::CurveTypeEnumerateEnumeration, TXT( "CurveType" ) ) );
      g_InitializerStack.Push( Reflect::RegisterEnumeration<ControlPointLabels::ControlPointLabel>( &ControlPointLabels::ControlPointLabelEnumerateEnumeration, TXT( "ControlPointLabel" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Curve>( TXT( "Curve" ) ) );

      g_InitializerStack.Push( Reflect::RegisterClass<MorphTargetWeight>( TXT( "MorphTargetWeight" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<FrameMorphTargets>( TXT( "FrameMorphTargets" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<FrameWrinkleMap>( TXT( "FrameWrinkleMap" ) ) );

      g_InitializerStack.Push( Reflect::RegisterClass<CompressedAnimation>( TXT( "CompressedAnimation" ) ) );

      g_InitializerStack.Push( Reflect::RegisterClass<Animation>( TXT( "Animation" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<AnimationClip>( TXT( "AnimationClip" ) ) );

      // descriptor
      g_InitializerStack.Push( Reflect::RegisterEnumeration<GeometrySimulations::GeometrySimulation>( &GeometrySimulations::GeometrySimulationEnumerateEnumeration, TXT( "GeometrySimulation" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Descriptor>( TXT( "Descriptor" ) ) );

      // area light sources
      g_InitializerStack.Push( Reflect::RegisterClass<AreaLight>( TXT( "AreaLight" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<RadiusLight>( TXT( "RadiusLight" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<SphereLight>( TXT( "SphereLight" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<RectangleLight>( TXT( "RectangleLight" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<CylinderLight>( TXT( "CylinderLight" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<DiscLight>( TXT( "DiscLight" ) ) );

      g_InitializerStack.Push( Reflect::RegisterClass<ParametricKey>( TXT( "ParametricKey" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<ParametricColorKey>( TXT( "ParametricColorKey" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<ParametricIntensityKey>( TXT( "ParametricIntensityKey" ) ) );

      // lights
      g_InitializerStack.Push( Reflect::RegisterEnumeration<LightTypes::LightType>( &LightTypes::LightTypeEnumerateEnumeration, TXT( "LightType" ) ) );
      g_InitializerStack.Push( Reflect::RegisterEnumeration<LightRenderTypes::LightRenderType>( &LightRenderTypes::LightRenderTypeEnumerateEnumeration, TXT( "LightRenderType" ) ) );
      g_InitializerStack.Push( Reflect::RegisterEnumeration<LensFlareTypes::LensFlareType>( &LensFlareTypes::LensFlareTypeEnumerateEnumeration, TXT( "LensFlareType" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Light>( TXT( "Light" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<SpotLight>( TXT( "SpotLight" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<PointLight>( TXT( "PointLight" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<DirectionalLight>( TXT( "DirectionalLight" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<ShadowDirection>( TXT( "ShadowDirection" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<SunLight>( TXT( "SunLight" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<PortalLight>( TXT( "PortalLight" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<AmbientLight>( TXT( "AmbientLight" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<AmbientVolumeLight>( TXT( "AmbientVolumeLight" ) ) );

      g_InitializerStack.Push( Reflect::RegisterClass<JointOrdering>( TXT( "JointOrdering" ) ) );

      // Visibility
      g_InitializerStack.Push( Reflect::RegisterClass<NodeVisibility>( TXT( "NodeVisibility" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<SceneVisibility>( TXT( "SceneVisibility" ) ) );
    }
  }

  void Cleanup()
  {
    if ( --g_InitCount == 0 )
    {
      g_InitializerStack.Cleanup();
    }
  }
}
