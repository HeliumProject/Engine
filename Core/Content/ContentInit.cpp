#include "ContentInit.h"
#include "ContentVersion.h"

#include "Foundation/Reflect/Registry.h"

#include "Core/Content/Nodes/ContentSceneNode.h"
#include "Core/Content/Nodes/ContentHierarchyNode.h"

#include "Core/Content/Nodes/ContentTransform.h"
#include "Core/Content/Nodes/ContentPivotTransform.h"
#include "Core/Content/Nodes/ContentJointTransform.h"

#include "Core/Content/Nodes/ContentEntityInstance.h"
#include "Core/Content/Nodes/ContentInstance.h"
#include "Core/Content/Nodes/ContentLocator.h"
#include "Core/Content/Nodes/ContentVolume.h"

#include "Core/Content/Nodes/ContentCamera.h"
#include "Core/Content/Nodes/ContentLayer.h"
#include "Core/Content/Nodes/ContentCurve.h"
#include "Core/Content/Nodes/ContentPoint.h"
#include "Core/Content/Nodes/ContentMesh.h"
#include "Core/Content/Nodes/ContentSkin.h"
#include "Core/Content/Nodes/ContentShader.h"

#include "Core/Content/Animation/Animation.h"
#include "Core/Content/Animation/JointAnimation.h"
#include "Core/Content/Animation/CompressedJointAnimation.h"
#include "Core/Content/Animation/JointOrdering.h"

#include "Core/Content/Nodes/ContentLight.h"
#include "Core/Content/Nodes/ContentSpotLight.h"
#include "Core/Content/Nodes/ContentPointLight.h"
#include "Core/Content/Nodes/ContentDirectionalLight.h"
#include "Core/Content/Nodes/ContentAmbientLight.h"

#include "Core/Content/NodeVisibility.h"
#include "Core/Content/SceneVisibility.h"

#include "Foundation/InitializerStack.h"
#include "Foundation/Component/ComponentInit.h"

#include "Foundation/InitializerStack.h"

using namespace Helium;

i32 g_InitCount = 0;
static Helium::InitializerStack g_InitializerStack;

void Content::Initialize()
{
    if ( ++g_InitCount == 1 )
    {
        g_InitializerStack.Push( Reflect::Initialize, Reflect::Cleanup );
        g_InitializerStack.Push( Component::Initialize, Component::Cleanup );

        g_InitializerStack.Push( Reflect::RegisterClassType<ContentVersion>( TXT( "ContentVersion" ) ) );

        g_InitializerStack.Push( Reflect::RegisterClassType<SceneNode>( TXT( "SceneNode" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<HierarchyNode>( TXT( "HierarchyNode" ) ) );

        g_InitializerStack.Push( Reflect::RegisterClassType<Transform>( TXT( "Transform" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<PivotTransform>( TXT( "PivotTransform" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<JointTransform>( TXT( "JointTransform" ) ) );

        g_InitializerStack.Push( Reflect::RegisterClassType<Instance>( TXT( "Instance" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<EntityInstance>( TXT( "EntityInstance" ) ) );

        g_InitializerStack.Push( Reflect::RegisterEnumType<VolumeShapes::VolumeShape>( &VolumeShapes::VolumeShapeEnumerateEnum, TXT( "VolumeShape" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<Volume>( TXT( "Volume" ) ) );

        g_InitializerStack.Push( Reflect::RegisterEnumType<LocatorShapes::LocatorShape>( &LocatorShapes::LocatorShapeEnumerateEnum, TXT( "LocatorShape" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<Locator>( TXT( "Locator" ) ) );

        g_InitializerStack.Push( Reflect::RegisterClassType<Camera>( TXT( "Camera" ) ) );

        g_InitializerStack.Push( Reflect::RegisterClassType<Layer>( TXT( "Layer" ) ) );

        g_InitializerStack.Push( Reflect::RegisterClassType<MorphTargetDelta>( TXT( "MorphTargetDelta" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<MorphTarget>( TXT( "MorphTarget" ) ) );

        g_InitializerStack.Push( Reflect::RegisterEnumType<Mesh::MeshOriginType>( &Mesh::MeshOriginTypeEnumeration, TXT( "MeshOriginType" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<Mesh>( TXT( "Mesh" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<Shader>( TXT( "Shader" ) ) );

        g_InitializerStack.Push( Reflect::RegisterClassType<Influence>( TXT( "Influence" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<Skin>( TXT( "Skin" ) ) );

        g_InitializerStack.Push( Reflect::RegisterClassType<Point>( TXT( "Point" ) ) );
        g_InitializerStack.Push( Reflect::RegisterEnumType<CurveTypes::CurveType>( &CurveTypes::CurveTypeEnumerateEnum, TXT( "CurveType" ) ) );
        g_InitializerStack.Push( Reflect::RegisterEnumType<ControlPointLabels::ControlPointLabel>( &ControlPointLabels::ControlPointLabelEnumerateEnum, TXT( "ControlPointLabel" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<Curve>( TXT( "Curve" ) ) );

        g_InitializerStack.Push( Reflect::RegisterClassType<MorphTargetWeight>( TXT( "MorphTargetWeight" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<FrameMorphTargets>( TXT( "FrameMorphTargets" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<FrameWrinkleMap>( TXT( "FrameWrinkleMap" ) ) );

        g_InitializerStack.Push( Reflect::RegisterClassType<CompressedJointAnimation>( TXT( "CompressedJointAnimation" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<JointAnimation>( TXT( "JointAnimation" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<Animation>( TXT( "Animation" ) ) );

        // lights
        g_InitializerStack.Push( Reflect::RegisterEnumType<LightTypes::LightType>( &LightTypes::LightTypeEnumerateEnum, TXT( "LightType" ) ) );
        g_InitializerStack.Push( Reflect::RegisterEnumType<LightRenderTypes::LightRenderType>( &LightRenderTypes::LightRenderTypeEnumerateEnum, TXT( "LightRenderType" ) ) );
        g_InitializerStack.Push( Reflect::RegisterEnumType<LensFlareTypes::LensFlareType>( &LensFlareTypes::LensFlareTypeEnumerateEnum, TXT( "LensFlareType" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<Light>( TXT( "Light" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<SpotLight>( TXT( "SpotLight" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<PointLight>( TXT( "PointLight" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<DirectionalLight>( TXT( "DirectionalLight" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<AmbientLight>( TXT( "AmbientLight" ) ) );

        g_InitializerStack.Push( Reflect::RegisterClassType<JointOrdering>( TXT( "JointOrdering" ) ) );

        // Visibility
        g_InitializerStack.Push( Reflect::RegisterClassType<NodeVisibility>( TXT( "NodeVisibility" ) ) );
        g_InitializerStack.Push( Reflect::RegisterClassType<SceneVisibility>( TXT( "SceneVisibility" ) ) );
    }
}

void Content::Cleanup()
{
    if ( --g_InitCount == 0 )
    {
        g_InitializerStack.Cleanup();
    }
}
