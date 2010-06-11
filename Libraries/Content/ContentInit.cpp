#include "ContentInit.h"
#include "ContentVersion.h"
#include "ContentTypes.h"

#include "Reflect/Registry.h"

#include "SceneNode.h"
#include "HierarchyNode.h"
#include "Hierarchy.h"

#include "Transform.h"
#include "PivotTransform.h"
#include "JointTransform.h"

#include "Instance.h"
#include "Locator.h"
#include "Volume.h"

#include "Camera.h"
#include "Layer.h"
#include "Zone.h"
#include "Region.h"

#include "Mesh.h"
#include "Skin.h"
#include "Shader.h"
#include "CollisionPrimitive.h"
#include "Descriptor.h"
#include "Effector.h"

#include "ParametricColorKey.h"
#include "ParametricIntensityKey.h"
#include "ParametricKey.h"

#include "Curve.h"
#include "Point.h"
#include "Animation.h"
#include "AnimationClip.h"
#include "CompressedAnimation.h"
#include "JointOrdering.h"

#include "Light.h"
#include "SpotLight.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "ShadowDirection.h"
#include "SunLight.h"
#include "AreaLight.h"
#include "RadiusLight.h"
#include "SphereLight.h"
#include "RectangleLight.h"
#include "CylinderLight.h"
#include "DiscLight.h"
#include "PortalLight.h"
#include "AmbientLight.h"
#include "AmbientVolumeLight.h"

#include "NodeVisibility.h"
#include "SceneVisibility.h"

#include "Foundation/InitializerStack.h"
#include "Attribute/AttributeInit.h"
#include "Attribute/AttributeCategories.h"

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
      g_InitializerStack.Push( Attribute::Initialize, Attribute::Cleanup );

      g_InitializerStack.Push( Reflect::RegisterClass<ContentVersion>( "ContentVersion" ) );
      g_InitializerStack.Push( Reflect::RegisterEnumeration<ContentTypes::ContentType>( &ContentTypes::ContentTypeEnumerateEnumeration, "ContentType" ) );

      g_InitializerStack.Push( Reflect::RegisterClass<SceneNode>( "SceneNode" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<HierarchyNode>( "HierarchyNode" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Hierarchy>( "Hierarchy" ) );

      g_InitializerStack.Push( Reflect::RegisterClass<Transform>( "Transform" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<PivotTransform>( "PivotTransform" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<JointTransform>( "JointTransform" ) );

      g_InitializerStack.Push( Reflect::RegisterClass<Instance>( "Instance" ) );

      g_InitializerStack.Push( Reflect::RegisterEnumeration<VolumeShapes::VolumeShape>( &VolumeShapes::VolumeShapeEnumerateEnumeration, "VolumeShape" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Volume>( "Volume" ) );

      g_InitializerStack.Push( Reflect::RegisterEnumeration<LocatorShapes::LocatorShape>( &LocatorShapes::LocatorShapeEnumerateEnumeration, "LocatorShape" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Locator>( "Locator" ) );

      g_InitializerStack.Push( Reflect::RegisterClass<Camera>( "Camera" ) );

      g_InitializerStack.Push( Reflect::RegisterClass<Layer>( "Layer" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Zone>( "Zone" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Region>( "Region" ) );

      g_InitializerStack.Push( Reflect::RegisterClass<MorphTargetDelta>( "MorphTargetDelta" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<MorphTarget>( "MorphTarget" ) );

      g_InitializerStack.Push( Reflect::RegisterEnumeration<Mesh::MeshOriginType>( &Mesh::MeshOriginTypeEnumeration, "MeshOriginType" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Mesh>( "Mesh" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Shader>( "Shader" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Effector>( "Effector" ) );

      g_InitializerStack.Push( Reflect::RegisterEnumeration<CollisionShapes::CollisionShape>( &CollisionShapes::CollisionShapeEnumerateEnumeration, "CollisionShape" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<CollisionPrimitive>( "CollisionPrimitive" ) );
      Reflect::Registry::GetInstance()->AliasType( Reflect::GetClass<CollisionPrimitive>(), "CollisionVolume" );

      g_InitializerStack.Push( Reflect::RegisterClass<Influence>( "Influence" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Skin>( "Skin" ) );

      g_InitializerStack.Push( Reflect::RegisterClass<Point>( "Point" ) );
      g_InitializerStack.Push( Reflect::RegisterEnumeration<CurveTypes::CurveType>( &CurveTypes::CurveTypeEnumerateEnumeration, "CurveType" ) );
      g_InitializerStack.Push( Reflect::RegisterEnumeration<ControlPointLabels::ControlPointLabel>( &ControlPointLabels::ControlPointLabelEnumerateEnumeration, "ControlPointLabel" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Curve>( "Curve" ) );

      g_InitializerStack.Push( Reflect::RegisterClass<MorphTargetWeight>( "MorphTargetWeight" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<FrameMorphTargets>( "FrameMorphTargets" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<FrameWrinkleMap>( "FrameWrinkleMap" ) );

      g_InitializerStack.Push( Reflect::RegisterClass<CompressedAnimation>( "CompressedAnimation" ) );

      g_InitializerStack.Push( Reflect::RegisterClass<Animation>( "Animation" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<AnimationClip>( "AnimationClip" ) );

      // descriptor
      g_InitializerStack.Push( Reflect::RegisterEnumeration<GeometrySimulations::GeometrySimulation>( &GeometrySimulations::GeometrySimulationEnumerateEnumeration, "GeometrySimulation" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Descriptor>( "Descriptor" ) );

      // area light sources
      g_InitializerStack.Push( Reflect::RegisterClass<AreaLight>( "AreaLight" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<RadiusLight>( "RadiusLight" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<SphereLight>( "SphereLight" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<RectangleLight>( "RectangleLight" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<CylinderLight>( "CylinderLight" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<DiscLight>( "DiscLight" ) );

      g_InitializerStack.Push( Reflect::RegisterClass<ParametricKey>( "ParametricKey" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<ParametricColorKey>( "ParametricColorKey" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<ParametricIntensityKey>( "ParametricIntensityKey" ) );

      // lights
      g_InitializerStack.Push( Reflect::RegisterEnumeration<LightTypes::LightType>( &LightTypes::LightTypeEnumerateEnumeration, "LightType" ) );
      g_InitializerStack.Push( Reflect::RegisterEnumeration<LightRenderTypes::LightRenderType>( &LightRenderTypes::LightRenderTypeEnumerateEnumeration, "LightRenderType" ) );
      g_InitializerStack.Push( Reflect::RegisterEnumeration<LensFlareTypes::LensFlareType>( &LensFlareTypes::LensFlareTypeEnumerateEnumeration, "LensFlareType" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Light>( "Light" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<SpotLight>( "SpotLight" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<PointLight>( "PointLight" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<DirectionalLight>( "DirectionalLight" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<ShadowDirection>( "ShadowDirection" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<SunLight>( "SunLight" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<PortalLight>( "PortalLight" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<AmbientLight>( "AmbientLight" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<AmbientVolumeLight>( "AmbientVolumeLight" ) );

      g_InitializerStack.Push( Reflect::RegisterClass<JointOrdering>( "JointOrdering" ) );

      // Visibility
      g_InitializerStack.Push( Reflect::RegisterClass<NodeVisibility>( "NodeVisibility" ) );
      g_InitializerStack.Push( Reflect::RegisterClass<SceneVisibility>( "SceneVisibility" ) );
    }
  }

  void Cleanup()
  {
    if ( --g_InitCount == 0 )
    {
      g_InitializerStack.Cleanup();
    }
  }

  REFLECT_DEFINE_ENTRY_POINTS(Initialize, Cleanup);
}
