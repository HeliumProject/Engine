#include "Precompile.h"
#include "RemoteView.h"

#include "../../libraries/ipc/tcp.h"
#include "../../libraries/ipc/pipe.h"
#include "File/Manager.h"

#include "Controller.h"
#include "Entity.h"
#include "EntityAssetSet.h"
#include "Light.h"
#include "SpotLight.h"
#include "PointLight.h"
#include "Scene.h"
#include "LightingVolume.h"
#include "Curve.h"
#include "Volume.h"

#include "SceneEditor.h"
#include "SceneManager.h"
#include "Transform.h"
#include "Editor/SessionManager.h"
#include "Live/RuntimeConnection.h"
#include "View.h"
#include "Asset/AssetClass.h"

#include "igcore/igheaders/ps3structs.h"

using namespace Luna;
using namespace Luna::RemoteView;
using namespace RPC;

RPC::ILunaViewHost* RemoteView::g_LunaViewHost = NULL;
RPC::ILunaViewTool* RemoteView::g_LunaViewTool = NULL;

static bool g_Initialized = false;
static bool g_Enabled = false;
static bool g_Camera = false;
SceneEditor* g_Editor = NULL;

namespace Luna
{
  struct LunaViewTool : ILunaViewTool
  {
    LOCAL_IMPL(LunaViewTool, ILunaViewTool);

    virtual void TransformCamera(TransformCameraParam* param);
    virtual void RequestScene();

    virtual void UpdatePhysicsJoint(RPC::PhysicsJointParam* param);
    virtual void SelectJoint(RPC::SelectJointParam* param);
  };
}

void RemoteView::Initialize()
{
  if( g_Initialized )
  {
    return;
  }

  g_Initialized = true;

  RPC::Host* host = RuntimeConnection::GetHost();

  // create a local implementation of our functionality
  g_LunaViewHost = RuntimeConnection::GetRemoteLevelView();
  g_LunaViewTool = new LunaViewTool( host );
}

void RemoteView::Cleanup()
{
  if (g_Initialized)
  {
    delete g_LunaViewTool;

    g_LunaViewHost = NULL;
    g_LunaViewTool = NULL;
  }
}

void RemoteView::Enable( bool enable )
{
  g_Enabled = enable;
}

void RemoteView::EnableCamera( bool enable )
{
  g_Camera = enable;
}

void RemoteView::TransformInstance( Luna::Transform* transform )
{
  if ( RuntimeConnection::IsConnected() && g_LunaViewHost && g_Enabled )
  {
    // find the engine type of this instance
    if ( Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( transform ) )
    {
      Asset::EntityAsset* entityClass = entity->GetClassSet()->GetEntityAsset();

      TransformInstanceParam param;
      param.m_EngineType = -1;

      // set the engine type based on the types we currently support
      switch ( entityClass->GetEngineType() )
      {
      case Asset::EngineTypes::Tie:
        param.m_EngineType = RPC::EngineTypes::Tie;
        break;

      case Asset::EngineTypes::Moby:
        param.m_EngineType = RPC::EngineTypes::Moby;
        break;
      }

      // only send the command if this instance is a type we support being transformed
      if ( param.m_EngineType != -1 )
      {
        memcpy( &param.m_ID, &transform->GetID(), sizeof( param.m_ID ) );

        //ConvertGUID( param.m_ID );

        memcpy( &param.m_Transform, &transform->GetGlobalTransform(), sizeof( param.m_Transform ) );

        g_LunaViewHost->TransformInstance( &param );
      }
    }

    // gameplay controller
    if ( Luna::Controller* controller = Reflect::ObjectCast< Luna::Controller >( transform ) )
    {
      TransformInstanceParam param;
      param.m_EngineType = RPC::EngineTypes::Controller;

      memcpy( &param.m_ID, &transform->GetID(), sizeof( param.m_ID ) );

      //ConvertGUID( param.m_ID );

      memcpy( &param.m_Transform, &transform->GetGlobalTransform(), sizeof( param.m_Transform ) );

      g_LunaViewHost->TransformInstance( &param );
    }

    // gameplay volume
    else if ( Luna::Volume* volume = Reflect::ObjectCast< Luna::Volume >( transform ) )
    {
      TransformVolumeParam param;

      memcpy( &param.m_ID, &transform->GetID(), sizeof( param.m_ID ) );

      //ConvertGUID( param.m_ID );

      memcpy( &param.m_Transform, &transform->GetGlobalTransform(), sizeof( param.m_Transform ) );

      const Content::Volume* volumeData = volume->GetPackage<Content::Volume>();

      param.m_Type = (u8)volumeData->m_Shape;

      g_LunaViewHost->TransformVolume( &param );
    }

    // gameplay curve
    else if ( Luna::Curve* curve = Reflect::ObjectCast< Luna::Curve >( transform ) )
    {
      const Content::Curve* curveData = curve->GetPackage<Content::Curve>();

      //set the number of points in this path
      const Math::V_Vector3& points = curveData->m_Points;

      f32 length = 0.f;
      size_t num_points = points.size();
      IG::vec3* msg_points = NULL;

      if( num_points > 0 )
      {
        //allocate a buffer for the points
        msg_points = new IG::vec3[ num_points ];

        //copy the points to the buffer
        const Math::Vector3* previousPoint = NULL;
        Math::V_Vector3::const_iterator itr = points.begin();
        Math::V_Vector3::const_iterator end = points.end();
        for ( int i = 0; itr != end; ++itr, ++i )
        {
          if ( previousPoint != NULL )
            length += ((*itr) - *previousPoint).Length();

          previousPoint = &(*itr);
          Math::Vector3 worldPnt = (*itr);
          Math::Matrix4 globalTransform;
          globalTransform = curve->GetGlobalTransform();
          globalTransform.TransformVertex( worldPnt );
          //curveData->m_GlobalTransform.TransformVertex( worldPnt );
          memcpy( &msg_points[ i ], &worldPnt, sizeof( Math::Vector3 ) );
          ConvertEndian( msg_points[i], msg_points[i], true );
        }
      }

      TransformCurveParam param;

      memcpy( &param.m_ID, &transform->GetID(), sizeof( param.m_ID ) );

      //ConvertGUID( param.m_ID );

      param.m_Length = length;

      g_LunaViewHost->TransformCurve( &param, (u8*)msg_points, static_cast< u32 >( num_points * sizeof( IG::vec3 ) ) );

      delete [] msg_points;
    }

    // if it's not an entity, it may be a light
    else if ( Luna::Light* light = Reflect::ObjectCast< Luna::Light >( transform ) )
    {
      TransformLightParam param;

      memcpy( &param.m_ID, &light->GetID(), sizeof( param.m_ID ) );

      //ConvertGUID( param.m_ID );

      param.m_Position.x = light->GetTranslate().x;
      param.m_Position.y = light->GetTranslate().y;
      param.m_Position.z = light->GetTranslate().z;
      param.m_Position.w = 1.0;

      // figure out the light direction
      Math::Vector4 dir = Math::Vector4( 0.0, 0.0, 1.0, 0.0 );
      dir = light->GetGlobalTransform() * dir;

      memcpy( &param.m_Direction, &dir, sizeof( param.m_Direction ) );

      g_LunaViewHost->TransformLight( &param );
    }
    else if( transform->HasType( Reflect::GetType<Luna::LightingVolume>() ) )
    {
      // this is sort of a hack.  we assume that any lighting volume in the world
      // file (root scene) is a global volume
      Luna::Scene* currentScene = transform->GetScene();
      bool globalVolume = currentScene->GetManager()->GetRootScene() == currentScene;

      // we don't transform the global volume
      if ( !globalVolume )
      {
        TransformVolumeParam param;
        param.m_Type        = VolumeTypes::Lighting;
        memcpy( &param.m_ID, &transform->GetID(), sizeof( param.m_ID ) );

        //ConvertGUID( param.m_ID );

        memcpy( &param.m_Transform, &transform->GetGlobalTransform(), sizeof( param.m_Transform ) );

        g_LunaViewHost->TransformVolume( &param );
      }
    }
  }
}

void RemoteView::TransformCamera( const Math::Matrix4& transform )
{
  if ( RuntimeConnection::IsConnected() && g_LunaViewHost && g_Enabled && g_Camera )
  {
    // rotate by 180 for the engine
    Math::Matrix4 rotated = Math::Matrix4::RotateY( Math::Pi );

    rotated = rotated * transform;

    RPC::TransformCameraParam param;
    memcpy( &param.m_Transform, &rotated, sizeof( param.m_Transform ) );

    g_LunaViewHost->TransformCamera( &param );
  }
}

void RemoteView::SetLightParams( Luna::Light* light )
{
  if ( RuntimeConnection::IsConnected() && g_LunaViewHost && g_Enabled )
  {
    LightParametersParam param;

    //Clear
    ZeroMemory(&param, sizeof(LightParametersParam));

    memcpy( &param.m_ID, &light->GetID(), sizeof( param.m_ID ) );

    light->GetPackage< Content::Light >()->m_Color.ToLinearFloat( param.m_Color.x, param.m_Color.y, param.m_Color.z );

    param.m_DrawDist = light->GetDrawDist();
    param.m_RadiusInner = 0.f;
    param.m_Radius = 0.f;

    Luna::PointLight* pointLight = Reflect::ObjectCast<Luna::PointLight>(light);
    if (pointLight)
    {
      param.m_RadiusInner = pointLight->GetInnerRadius();
      param.m_Radius = pointLight->GetOuterRadius();
    }

    SpotLight* spotLight = Reflect::ObjectCast<SpotLight>(light);
    if (spotLight)
    {
      param.m_RadiusInner = spotLight->GetInnerRadius();
      param.m_Radius = spotLight->GetOuterRadius();
    }

    param.m_Flags = 0;
    if ( light->GetPackage< Content::Light >()->m_KillIfInactive )
    {
      param.m_Flags |= IGPS3::RTL_KILL_IF_INACTIVE;
    }

    if ( light->GetPackage< Content::Light >()->m_CastShadows )
    {
      param.m_Flags |= IGPS3::RTL_CASTS_SHADOWS;
    }

    if ( light->GetPackage< Content::Light >()->m_AllowOversized )
    {
      param.m_Flags |= IGPS3::RTL_ALLOW_OVERSIZED;
    }

    //Lens Flare
    {
      i32 flareType = light->GetPackage< Content::Light >()->m_FlareType;

      switch(flareType)
      {
        case Content::LensFlareTypes::Vortex:
        {
          param.m_Flags |= IGPS3::RTL_LENS_FLARE_VORTEX;
        }
        break;

        case Content::LensFlareTypes::VortexNoTrail:
        {
          param.m_Flags |= IGPS3::RTL_LENS_FLARE_VORTEX_NO_TRAIL;
        }
        break;
      }
    }

    SpotLight* spot = Reflect::ObjectCast< SpotLight >( light );
    if ( spot )
    {
      param.m_InnerConeAngle = spot->GetInnerConeAngle();
      param.m_OuterConeAngle = spot->GetOuterConeAngle();

      if ( spot->GetShadowMapHiRes() )
      {
        param.m_Flags |= IGPS3::RTL_SHADOW_MAP_HI_RES;
      }

      Content::SpotLight* spotLight = Reflect::ObjectCast<Content::SpotLight>(spot);

      if(spotLight && spotLight->m_GodRayEnabled )
      {
        Math::Vector4 clipPlane = spotLight->GetGodRayClipPlane();

        param.m_Flags   |= IGPS3::RTL_GODRAY;
        param.m_Color.w  = spotLight->m_GodRayOpacity;

        param.m_GodRayDensity       = spotLight->m_GodRayDensity;
        param.m_GodRayQuality       = spotLight->m_GodRayQuality;
        param.m_GodRayFadeNear      = spotLight->m_GodRayFadeNear;
        param.m_GodRayFadeFar       = spotLight->m_GodRayFadeFar;

        param.m_GodRayClipPlaneX    = clipPlane.x;
        param.m_GodRayClipPlaneY    = clipPlane.y;
        param.m_GodRayClipPlaneZ    = clipPlane.z;
        param.m_GodRayClipPlaneD    = clipPlane.w;
      }
    }

    g_LunaViewHost->SetLightParameters( &param );
  }
}

class ColorKeyFrames
{
public:

  Content::V_ParametricColorKeyPtr m_KeyFrames;

  ColorKeyFrames( Content::V_ParametricColorKeyPtr& keyFrames )
  {
    m_KeyFrames = keyFrames;
  }

  Math::Color3 GetColor( f32 time )
  {

    if( m_KeyFrames.empty() )
      return Math::Color3( 0, 0, 0 );

    Content::V_ParametricColorKeyPtr::iterator itor = m_KeyFrames.begin();
    Content::V_ParametricColorKeyPtr::iterator end  = m_KeyFrames.end();

    // should never have fewer than 2 keyframes
    NOC_ASSERT( m_KeyFrames.size() >= 2 );

    Content::ParametricColorKeyPtr lastKey = *itor++;

    if( lastKey->m_Param == time )
      return lastKey->m_Color;

    for( ; itor != end; ++itor )
    {
      if( (*itor)->m_Param >= time )
      {
        if( (*itor)->m_Param == time )
          return (*itor)->m_Color;

        if( lastKey->m_Param == (*itor)->m_Param )
          return lastKey->m_Color;

        // squash to 0 - 1 range
        f32 beginTime = lastKey->m_Param;
        f32 endTime   = (*itor)->m_Param;
        f32 len = endTime - beginTime;
        f32 diff = time - beginTime;
        f32 param = diff/len;

        i32 r = (i32)(*itor)->m_Color.r - (i32)lastKey->m_Color.r;
        i32 g = (i32)(*itor)->m_Color.g - (i32)lastKey->m_Color.g;
        i32 b = (i32)(*itor)->m_Color.b - (i32)lastKey->m_Color.b;

        r = (i32)(param * (f32)r);
        g = (i32)(param * (f32)g);
        b = (i32)(param * (f32)b);

        r = std::max( r + lastKey->m_Color.r, 0 );
        g = std::max( g + lastKey->m_Color.g, 0 );
        b = std::max( b + lastKey->m_Color.b, 0 );

        return Math::Color3( (u8)r, (u8)g, (u8)b );
      }
      lastKey = *itor;
    }
    return Math::Color3( 0, 0, 0 );
  }

};
void RemoteView::SetLightAnimation( Luna::Light* light )
{
  if ( RuntimeConnection::IsConnected() && g_LunaViewHost && g_Enabled )
  {
    LightAnimationParam param;

    //Clear
    ZeroMemory(&param, sizeof(LightAnimationParam));

    memcpy( &param.m_ID, &light->GetID(), sizeof( param.m_ID ) );

    Content::Light* contentLight = light->GetPackage< Content::Light >();

    if( contentLight->m_AnimationColor.size() >= 3 || contentLight->m_AnimationIntensity.size() >= 3 )
    {
      if( contentLight->m_RandomAnimOffset )
      {
        param.m_Flags |= IGPS3::RTL_ANIM_RAND_OFFSET;
      }

      u32 numColorKeys = (u32)contentLight->m_AnimationColor.size();
      u32 numIntensityKeys = (u32)contentLight->m_AnimationIntensity.size();

      ColorKeyFrames mostKeys( ( numColorKeys >= numIntensityKeys ) ? contentLight->m_AnimationColor : contentLight->m_AnimationIntensity );
      ColorKeyFrames fewestKeys( ( numColorKeys >= numIntensityKeys ) ? contentLight->m_AnimationIntensity : contentLight->m_AnimationColor );

      Vector4 lightColor;
      contentLight->m_Color.ToLinearFloat( lightColor.x, lightColor.y, lightColor.z );

      f32 max = std::max( lightColor.x, lightColor.y );
      max = std::max( max, lightColor.z );

      Content::V_ParametricColorKeyPtr::iterator itor = mostKeys.m_KeyFrames.begin();
      Content::V_ParametricColorKeyPtr::iterator end  = mostKeys.m_KeyFrames.end();
      for( u32 i = 0; itor != end; ++itor, ++i )
      {
        LightAnimationKeyframeData& keyFrame = param.m_Keyframes[i];
        ZeroMemory(&keyFrame, sizeof(LightAnimationKeyframeData));

        keyFrame.m_Time = (*itor)->m_Param * contentLight->m_AnimationDuration;

        if( numColorKeys >= numIntensityKeys )
        {
          Math::HDRColor3 color( (*itor)->m_Color );
          color.s = contentLight->m_Color.s;

          color.ToLinearFloat( keyFrame.m_Color.x, keyFrame.m_Color.y, keyFrame.m_Color.z );

          Math::Color3 ampFreqColor( 0, 0, 0 );
          if( numIntensityKeys >= 2 )
            ampFreqColor = fewestKeys.GetColor( (*itor)->m_Param );

          keyFrame.m_Freq = ampFreqColor.g / 255.0f * 10.0f;
          keyFrame.m_Amp  = ampFreqColor.r / 255.0f * max;
        }
        else
        {
          Math::HDRColor3 color( contentLight->m_Color );

          if( numColorKeys >= 2 )
          {
            color =  fewestKeys.GetColor( (*itor)->m_Param );
            color.s = contentLight->m_Color.s;
          }

          color.ToLinearFloat( keyFrame.m_Color.x, keyFrame.m_Color.y, keyFrame.m_Color.z );

          keyFrame.m_Freq = (*itor)->m_Color.g / 255.0f * 10.0f;
          keyFrame.m_Amp  = (*itor)->m_Color.r / 255.0f * max;
        }
      }
      g_LunaViewHost->SetLightAnimation( &param );
    }
  }
}

void RemoteView::AddLight( Luna::Light* light )
{
  if ( RuntimeConnection::IsConnected() && g_LunaViewHost && g_Enabled )
  {
    AddLightParam param;

    memcpy( &param.m_ID, &light->GetID(), sizeof( param.m_ID ) );

    strncpy( param.m_Name, light->GetName().c_str(), sizeof( param.m_Name ) );

    param.m_Type = RPC::LightTypes::Point;

    SpotLight* spot = Reflect::ObjectCast< SpotLight >( light );
    if ( spot )
    {
      param.m_Type = RPC::LightTypes::Spot;
    }

    g_LunaViewHost->AddLight( &param );

    TransformInstance( light );
    SetLightParams( light );
  }
}

void RemoteView::RemoveLight( Luna::Light* light )
{
  if ( RuntimeConnection::IsConnected() && g_LunaViewHost && g_Enabled )
  {
    RemoveLightParam param;

    memcpy( &param.m_ID, &light->GetID(), sizeof( param.m_ID ) );

    g_LunaViewHost->RemoveLight( &param );
  }
}

void RemoteView::TweakLightmapSettings( Luna::Entity* entity, u32 lm_set_index )
{
  if ( RuntimeConnection::IsConnected() && g_LunaViewHost && g_Enabled )
  {
    if ( entity )
    {
      Asset::EntityAsset* entityClass = entity->GetClassSet()->GetEntityAsset();

      LightmapParam param;
      param.m_EngineType = -1;

      switch ( entityClass->GetEngineType() )
      {
      case Asset::EngineTypes::Tie:
        param.m_EngineType = RPC::EngineTypes::Tie;
        break;

      case Asset::EngineTypes::Ufrag:
        param.m_EngineType = RPC::EngineTypes::Ufrag;
        break;
      }

      // only send the command if this instance is a type we support being transformed
      if ( param.m_EngineType != -1 )
      {
        memcpy( &param.m_ID, &entity->GetID(), sizeof( param.m_ID ) );

        param.m_ModifiedLightMapSet = lm_set_index;

        if(lm_set_index == 0)
        {
          param.m_NewFormat = entity->GetRuntimeLM1Format();
          param.m_NewSize = 1 << entity->GetRuntimeLM1Size();
        }
        else
        {
          param.m_NewFormat = entity->GetRuntimeLM2Format();
          param.m_NewSize = 1 << entity->GetRuntimeLM2Size();
        }

        g_LunaViewHost->TweakLightmapSettings( &param );
      }
    }
  }
}

void RemoteView::TweakCubemapSettings( Luna::Entity* entity )
{
  if ( RuntimeConnection::IsConnected() && g_LunaViewHost && g_Enabled )
  {
    if ( entity )
    {
      Asset::EntityAsset* entityClass = entity->GetClassSet()->GetEntityAsset();

      CubemapParam param;
      param.m_EngineType = -1;

      switch ( entityClass->GetEngineType() )
      {
      case Asset::EngineTypes::Tie:
        param.m_EngineType = RPC::EngineTypes::Tie;
        break;

      case Asset::EngineTypes::Ufrag:
        param.m_EngineType = RPC::EngineTypes::Ufrag;
        break;
      }

      // only send the command if this instance is a type we support being transformed
      if ( param.m_EngineType != -1 )
      {
        memcpy( &param.m_ID, &entity->GetID(), sizeof( param.m_ID ) );

        param.m_Intensity = entity->GetCubeMapIntensity();

        g_LunaViewHost->TweakCubemapSettings( &param );
      }
    }
  }
}

void RemoteView::SceneNodeDeleted( const NodeChangeArgs& args )
{
  if ( RuntimeConnection::IsConnected() && g_LunaViewHost && g_Enabled )
  {
    Luna::Light* light = Reflect::ObjectCast< Luna::Light >( args.m_Node );
    if ( light && light->GetRenderType() != Content::LightRenderTypes::Baked )
    {
      RemoveLight( light );
    }
    else if( args.m_Node->HasType( Reflect::GetType<Luna::LightingVolume>() ) )
    {
      // this is sort of a hack.  we assume that any lighting volume in the world
      // file (root scene) is a global volume
      Luna::Scene* currentScene = args.m_Node->GetScene();
      bool globalVolume = currentScene->GetManager()->GetRootScene() == currentScene;

      if ( !globalVolume )
        RemoveLightingVolume( Reflect::DangerousCast< Luna::LightingVolume >( args.m_Node ) );
    }
  }
}

void RemoteView::SceneNodeCreated( const NodeChangeArgs& args )
{
  if ( RuntimeConnection::IsConnected() && g_LunaViewHost && g_Enabled )
  {
    Luna::Light* light = Reflect::ObjectCast< Luna::Light >( args.m_Node );
    if ( light && light->GetRenderType() != Content::LightRenderTypes::Baked )
    {
      AddLight( light );
    }
    else if( args.m_Node->HasType( Reflect::GetType<Luna::LightingVolume>() ) )
    {
      // this is sort of a hack.  we assume that any lighting volume in the world
      // file (root scene) is a global volume
      Luna::Scene* currentScene = args.m_Node->GetScene();
      bool globalVolume = currentScene->GetManager()->GetRootScene() == currentScene;

      if ( !globalVolume )
        AddLightingVolume( Reflect::DangerousCast< Luna::LightingVolume >( args.m_Node ) );
    }
  }
}

void RemoteView::SetLightingVolumeParams( Luna::LightingVolume* light )
{
  //What the hell is this crap anyway?
}

void RemoteView::AddLightingVolume( Luna::LightingVolume* volume )
{
  if ( RuntimeConnection::IsConnected() && g_LunaViewHost && g_Enabled )
  {
    AddVolumeParam param;
    param.m_Type  = VolumeTypes::Lighting;

    memcpy( &param.m_ID, &volume->GetID(), sizeof( param.m_ID ) );

    strncpy( param.m_Name, volume->GetName().c_str(), sizeof( param.m_Name ) );

    g_LunaViewHost->AddVolume( &param );

    TransformInstance( volume );
    SetLightingVolumeParams( volume );
  }
}

void RemoteView::RemoveLightingVolume( Luna::LightingVolume* light )
{
  if ( RuntimeConnection::IsConnected() && g_LunaViewHost && g_Enabled )
  {
    RemoveVolumeParam param;
    param.m_Type  = VolumeTypes::Lighting;

    memcpy( &param.m_ID, &light->GetID(), sizeof( param.m_ID ) );

    g_LunaViewHost->RemoveVolume( &param );
  }
}
