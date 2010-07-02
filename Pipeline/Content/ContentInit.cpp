#include "ContentInit.h"
#include "ContentVersion.h"

#include "Foundation/Reflect/Registry.h"

#include "Pipeline/Content/Nodes/SceneNode.h"
#include "Pipeline/Content/Nodes/HierarchyNode.h"

#include "Pipeline/Content/Nodes/Transform.h"
#include "Pipeline/Content/Nodes/PivotTransform.h"
#include "Pipeline/Content/Nodes/JointTransform.h"

#include "Pipeline/Content/Nodes/Instance.h"
#include "Pipeline/Content/Nodes/Locator.h"
#include "Pipeline/Content/Nodes/Volume.h"

#include "Pipeline/Content/Nodes/Camera.h"
#include "Pipeline/Content/Nodes/Layer.h"
#include "Pipeline/Content/Nodes/Curve.h"
#include "Pipeline/Content/Nodes/Point.h"
#include "Pipeline/Content/Nodes/Mesh.h"
#include "Pipeline/Content/Nodes/Skin.h"
#include "Pipeline/Content/Nodes/Shader.h"

#include "Pipeline/Content/ParametricKey/ParametricColorKey.h"
#include "Pipeline/Content/ParametricKey/ParametricIntensityKey.h"
#include "Pipeline/Content/ParametricKey/ParametricKey.h"

#include "Pipeline/Content/Animation/Animation.h"
#include "Pipeline/Content/Animation/JointAnimation.h"
#include "Pipeline/Content/Animation/CompressedJointAnimation.h"
#include "Pipeline/Content/Animation/JointOrdering.h"

#include "Pipeline/Content/Nodes/Light.h"
#include "Pipeline/Content/Nodes/SpotLight.h"
#include "Pipeline/Content/Nodes/PointLight.h"
#include "Pipeline/Content/Nodes/DirectionalLight.h"
#include "Pipeline/Content/Nodes/AmbientLight.h"

#include "Pipeline/Content/NodeVisibility.h"
#include "Pipeline/Content/SceneVisibility.h"

#include "Foundation/InitializerStack.h"
#include "Foundation/Component/ComponentInit.h"

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

      g_InitializerStack.Push( Reflect::RegisterClass<SceneNode>( TXT( "SceneNode" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<HierarchyNode>( TXT( "HierarchyNode" ) ) );

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

      g_InitializerStack.Push( Reflect::RegisterClass<MorphTargetDelta>( TXT( "MorphTargetDelta" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<MorphTarget>( TXT( "MorphTarget" ) ) );

      g_InitializerStack.Push( Reflect::RegisterEnumeration<Mesh::MeshOriginType>( &Mesh::MeshOriginTypeEnumeration, TXT( "MeshOriginType" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Mesh>( TXT( "Mesh" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Shader>( TXT( "Shader" ) ) );

      g_InitializerStack.Push( Reflect::RegisterClass<Influence>( TXT( "Influence" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Skin>( TXT( "Skin" ) ) );

      g_InitializerStack.Push( Reflect::RegisterClass<Point>( TXT( "Point" ) ) );
      g_InitializerStack.Push( Reflect::RegisterEnumeration<CurveTypes::CurveType>( &CurveTypes::CurveTypeEnumerateEnumeration, TXT( "CurveType" ) ) );
      g_InitializerStack.Push( Reflect::RegisterEnumeration<ControlPointLabels::ControlPointLabel>( &ControlPointLabels::ControlPointLabelEnumerateEnumeration, TXT( "ControlPointLabel" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Curve>( TXT( "Curve" ) ) );

      g_InitializerStack.Push( Reflect::RegisterClass<MorphTargetWeight>( TXT( "MorphTargetWeight" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<FrameMorphTargets>( TXT( "FrameMorphTargets" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<FrameWrinkleMap>( TXT( "FrameWrinkleMap" ) ) );

      g_InitializerStack.Push( Reflect::RegisterClass<CompressedJointAnimation>( TXT( "CompressedJointAnimation" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<JointAnimation>( TXT( "JointAnimation" ) ) );
      g_InitializerStack.Push( Reflect::RegisterClass<Animation>( TXT( "Animation" ) ) );

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
      g_InitializerStack.Push( Reflect::RegisterClass<AmbientLight>( TXT( "AmbientLight" ) ) );

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
