#include "Precompile.h"

#include "RemoteEditor.h"
#include "RuntimeConnection.h"
#include "LiveManager.h"
#include "Task/Build.h"

#include "AssetEditor/LevelAsset.h"
#include "Asset/AssetClass.h"
#include "AssetBuilder/AssetBuilder.h"
#include "Attribute/AttributeHandle.h"
#include "Asset/FoliageAttribute.h"
#include "Asset/Entity.h"
#include "Asset/InstanceCollisionAttribute.h"
#include "Asset/ShaderGroupAttribute.h"
#include "Asset/ShaderGroupOverrideAttribute.h"
#include "Asset/WeatherAttribute.h"
#include "BuilderUtil/ColorPalette.h"
#include "BuilderUtil/BuilderUtil.h"
#include "Content/JointTransformsAttribute.h"
#include "Content/Light.h"
#include "Content/PointLight.h"
#include "Content/SpotLight.h"
#include "Content/LightingVolume.h"
#include "ContentProcess/ShaderUtils.h"
#include "Content/PostProcessingVolume.h"
#include "Content/PostEffectsFogAttribute.h"
#include "Content/ShadowDirection.h"
#include "Content/DirectionalLight.h"
#include "Content/AmbientLight.h"
#include "Asset/WorldFileAttribute.h"

#include "FileSystem/FileSystem.h"
#include "Finder/OcclusionSpecs.h"
#include "Symbol/Inheritance.h"

#include "math/Matrix4.h"

#include "rpc/interfaces/rpc_lunaview_host.h"
#include "rpc/interfaces/rpc_physics.h"
#include "rpc/interfaces/rpc_common.h"

#include "UIToolKit/DialogWorkerThread.h"
#include "UIToolKit/WorkerThreadDialog.h"

#include "TextureProcess/TextureWriter.h"
#include "Texture/Texture.h"

#include "igCore/igHeaders/FileChunkIDs.h"
#include "Editor/Editor.h"

using namespace Luna;
using namespace Math;
using namespace RPC;

Attachment::Attachment()
: RefCountBase()
, m_Mode(RPC::AttachmentModes::JointToJoint)
, m_MainJointID(-1)
, m_AttachedJointID(-1)
{

}

RemoteEditor::RemoteEditor( Editor* editor )
: m_Enabled( false )
, m_Editor( editor )
, m_Mode( RPC::LunaViewModes::Normal )
{
  RuntimeConnection::AddRuntimeConnectionStatusListener( RuntimeConnectionStatusSignature::Delegate( this, &RemoteEditor::OnConnectionStatusChanged ) );
  AddTaskFinishedListener( TaskFinishedSignature::Delegate ( this, &RemoteEditor::BuildFinished ) );
  AddAssetBuiltListener( AssetBuilder::AssetBuiltSignature::Delegate ( this, &RemoteEditor::AssetBuilt ) );
}

RemoteEditor::~RemoteEditor()
{
  RuntimeConnection::RemoveRuntimeConnectionStatusListener( RuntimeConnectionStatusSignature::Delegate( this, &RemoteEditor::OnConnectionStatusChanged ) );
  RemoveTaskFinishedListener( TaskFinishedSignature::Delegate ( this, &RemoteEditor::BuildFinished ) );
  RemoveAssetBuiltListener( AssetBuilder::AssetBuiltSignature::Delegate ( this, &RemoteEditor::AssetBuilt ) );
}

void RemoteEditor::Enable( bool enable )
{
  m_Enabled = enable;
}

void RemoteEditor::OnConnectionStatusChanged( const RuntimeConnectionStatusArgs& args )
{
  if ( !m_Enabled )
  {
    return;
  }

  if ( args.m_Connected )
  {
    RPC::ILunaViewHostRemote* lunaView = RuntimeConnection::GetRemoteLevelView();
    lunaView->Reset();
    SendScene();
  }
}

void RemoteEditor::AssetBuilt( const AssetBuilder::AssetBuiltArgsPtr& args )
{
  if ( !m_Enabled )
  {
    return;
  }

  if ( args->m_JobResult != AssetBuilder::JobResults::Skip )
  {
    m_AssetsToReload.insert( args->m_AssetId );
  }
}

void RemoteEditor::ReloadLastBuiltAssets()
{
  S_tuid::iterator it = m_AssetsToReload.begin();
  S_tuid::iterator end = m_AssetsToReload.end();
  for ( ; it != end; ++it )
  {
    ReloadAsset( *it );
  }

  m_AssetsToReload.clear();
}

void RemoteEditor::BuildFinished( const TaskFinishedArgs& taskArgs )
{
  const BuildFinishedArgs& args = static_cast<const BuildFinishedArgs&>(taskArgs);

  if ( !m_Enabled )
  {
    return;
  }

  ReloadLastBuiltAssets();

  if ( !args.m_View || args.m_Result != TaskResults::Success || args.m_Assets.size() != 1 )
  {
    return;
  }

  Asset::AssetClassPtr assetClass = Asset::AssetClass::FindAssetClass( *args.m_Assets.begin() );

  // use uberview if it supports this asset type, otherwise go through AssetBuilder::View
  Asset::EngineType engineType = assetClass->GetEngineType();
  bool useUberview = false;
  switch ( engineType )
  {
  case Asset::EngineTypes::Ufrag:
  case Asset::EngineTypes::Foliage:
  case Asset::EngineTypes::Shrub:
  case Asset::EngineTypes::Tie:
  case Asset::EngineTypes::Moby:
    useUberview = true;
    break;
  }

  if ( useUberview )
  {
    TargetManager::TargetPtr target = Manager::GetInstance()->GetCurrentTarget();

    if ( !target.ReferencesObject() )
    {
      wxMessageBox( "No PS3 Targets!", "Error", wxOK|wxCENTRE );
      return;
    }

    ViewAsset( assetClass->m_AssetClassID );
    if ( !RuntimeConnection::IsViewerConnected( "uberview" ) || target->IsCrashed() )
    {
      Manager::GetInstance()->StartViewer();
    }
    else
    {
      RPC::ILunaViewHostRemote* lunaView = RuntimeConnection::GetRemoteLevelView();
      lunaView->Reset();
      SendScene();
    }
  }
  else
  {
    std::string region;

    AssetBuilder::LevelBuilderOptionsPtr levelOptions = Reflect::ObjectCast< AssetBuilder::LevelBuilderOptions >( args.m_BuilderOptions );

    if ( levelOptions.ReferencesObject() )
    {
      if ( levelOptions->m_Regions.size() == 1 )
      {
        region = levelOptions->m_Regions[ 0 ];
      }
    }

    AssetBuilder::View( assetClass, region );
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////
inline u32 ConvertSrgbToSquared(u8 r, u8 g, u8 b)
{
  float red   = SRGBToLinear(f32(r)/255.f);
  float green = SRGBToLinear(f32(g)/255.f);
  float blue  = SRGBToLinear(f32(b)/255.f);

  int sqrr = int(sqrtf(red) * 255.f + 0.5f);
  int sqrg = int(sqrtf(green) * 255.f + 0.5f);
  int sqrb = int(sqrtf(blue) * 255.f + 0.5f);

  return ( sqrr << 24 ) | ( sqrg << 16 ) | ( sqrb << 8 ) | 0x00;
}

void RemoteEditor::SetLightingEnvironment( std::string& filePath )
{
  RPC::ILunaViewHostRemote* lunaView = RuntimeConnection::GetRemoteLevelView();

  if(!m_Enabled || !lunaView || !RuntimeConnection::IsConnected())
  {
    return;
  }

  if( !FileSystem::Exists( filePath ) )
    return;

  std::string filePathToLoad = filePath;
  if( FileSystem::HasExtension( filePath, FinderSpecs::Asset::LEVEL_DECORATION.GetDecoration() ) )
  {
    std::vector< Asset::LevelAssetPtr > levels;
    Reflect::Archive::FromFile< Asset::LevelAsset >( filePath, levels, NULL );
    if( !levels.empty()  )
    {
      Asset::LevelAsset* levelAsset = Reflect::ObjectCast< Asset::LevelAsset >( levels[0] );
      Attribute::AttributeViewer< Asset::WorldFileAttribute > worldFile( levelAsset );
      if( worldFile.Valid() )
      {
        filePathToLoad = worldFile->GetFilePath();
      }
    }
  }

  RPC::LinkedBakedLightsParam     param;
  ZeroMemory(&param, sizeof(LinkedBakedLightsParam));

  Content::Scene lightingEnvironment( filePathToLoad );
  
  Content::V_LightingVolume volumes;
  Content::V_Light lights;
  
  lightingEnvironment.GetAll< Content::LightingVolume >( volumes );
  
  if( volumes.size() == 1 )
  {
    volumes[0]->GetLinkedLights( lightingEnvironment, lights );
  }
  else
  {
    lightingEnvironment.GetAll< Content::Light >( lights );
  }

  Content::V_Light::iterator itor = lights.begin();
  Content::V_Light::iterator end  = lights.end();

  for ( ; itor != end && (param.m_Count < MAX_LINKED_LIGHTS_COUNT); ++itor )
  {
    Content::Light* light  = *itor;

    if(Content::ShadowDirection* shadowDir = Reflect::ObjectCast< Content::ShadowDirection >( light ) )
    {
      param.m_ShadowDirection.x = -light->m_GlobalTransform.z.x;
      param.m_ShadowDirection.y = -light->m_GlobalTransform.z.y;
      param.m_ShadowDirection.z = -light->m_GlobalTransform.z.z;
      continue;
    }

    //Directional Light
    if(light->HasType( Reflect::GetType<Content::DirectionalLight>() ) )
    {
      Content::DirectionalLight*  cDirLight   = Reflect::ObjectCast< Content::DirectionalLight >( light );
      u32                         idx         = param.m_Count++;

      param.m_Directions[idx].x = -cDirLight->m_GlobalTransform.z.x;
      param.m_Directions[idx].y = -cDirLight->m_GlobalTransform.z.y;
      param.m_Directions[idx].z = -cDirLight->m_GlobalTransform.z.z;
      param.m_Colors[idx]       = ConvertSrgbToSquared(cDirLight->m_Color.r, cDirLight->m_Color.g, cDirLight->m_Color.b);
      param.m_HDR[idx]          = cDirLight->m_Color.s;
      param.m_Flags[idx]        = IGPS3::IG_BAKED_DIR;

      if(cDirLight->m_DoVisibilityRayTest)
      {
        param.m_Flags[idx] |= IGPS3::IG_BAKED_LINE_OF_SIGHT;
      }

      if(cDirLight->m_GlobalSun)
      {
        param.m_Flags[idx] |= IGPS3::IG_BAKED_SUNLIGHT;
      }

      continue;
    }

    //Ambient light
    if( light->HasType( Reflect::GetType<Content::AmbientLight>() ) )
    {
      u32 idx                   = param.m_Count++;
      param.m_Directions[idx].x = 1.0f;
      param.m_Colors[idx]       = ConvertSrgbToSquared(light->m_Color.r, light->m_Color.g, light->m_Color.b);
      param.m_HDR[idx]          = light->m_Color.s;
      param.m_Flags[idx]        = IGPS3::IG_BAKED_AMB;
      continue;
    }
  }//Done

  //Send the info
  lunaView->SetLinkedBakedLights( &param );
}
void RemoteEditor::ViewAsset( tuid assetId )
{
  ClearScene();

  Asset::EntityPtr entity = new Asset::Entity;

  entity->SetEntityAssetID( assetId );
  entity->m_DefaultName = "generated";
  entity->m_ID = assetId;

  m_Scene.Add( entity );

  m_SelectedEntity = entity;

  AssetViewArgs args;
  args.m_entity = entity;

  m_AssetViewEvent.Raise(args);
}

void RemoteEditor::FrameEntityInstance( tuid assetId, const UniqueID::TUID& instanceId )
{
  RPC::ILunaViewHostRemote* lunaView = RuntimeConnection::GetRemoteLevelView();

  if(!m_Enabled || !lunaView || !RuntimeConnection::IsConnected())
  {
    return;
  }

  Asset::AssetClassPtr assetClass = Asset::AssetClass::FindAssetClass( assetId );
  if ( !assetClass.ReferencesObject() )
  {
    return;
  }

  RPC::FrameInstanceParam param;
  param.m_Type = GetAssetType( assetClass );

  if ( param.m_Type == (u8)-1 )
  {
    return;
  }

  param.m_ID = instanceId;

  lunaView->FrameInstance( &param );
}

u8 RemoteEditor::GetAssetType( const Asset::AssetClass* assetClass )
{
  if ( !assetClass )
  {
    return (u8)-1;
  }

  // set the engine type based on the types we currently support
  switch ( assetClass->GetEngineType() )
  {
  case Asset::EngineTypes::Tie:
    return RPC::EngineTypes::Tie;

  case Asset::EngineTypes::Moby:
    return RPC::EngineTypes::Moby;

  case Asset::EngineTypes::Ufrag:
    return RPC::EngineTypes::Ufrag;

  case Asset::EngineTypes::Foliage:
    return RPC::EngineTypes::Foliage;

  case Asset::EngineTypes::Shrub:
    return RPC::EngineTypes::Shrub;

  case Asset::EngineTypes::Sky:
    return RPC::EngineTypes::Sky;

  case Asset::EngineTypes::Shader:
    return RPC::EngineTypes::Shader;

  case Asset::EngineTypes::CubeMap:
    return RPC::EngineTypes::CubeMap;
  }

  return (u8)-1;
}

void RemoteEditor::ReloadAsset( tuid assetId )
{
  RPC::ILunaViewHostRemote* lunaView = RuntimeConnection::GetRemoteLevelView();

  if(!m_Enabled || !lunaView || !RuntimeConnection::IsConnected())
  {
    return;
  }

  Asset::AssetClassPtr assetClass;
  try
  {
    assetClass = Asset::AssetClass::FindAssetClass( assetId );
  }
  catch ( const Nocturnal::Exception& e )
  {
    Console::Error( "%s\n", e.what() );
    return;
  }

  if ( !assetClass.ReferencesObject() )
  {
    return;
  }

  RPC::ReloadAssetParam param;
  param.m_Type = GetAssetType( assetClass );

  if ( param.m_Type == -1 )
  {
    return;
  }

  param.m_AssetId = assetId;

  lunaView->ReloadAsset( &param );
}

void RemoteEditor::CreateMobyPayload( Asset::Entity* entity, u8*& payload, u32& payload_size )
{
  Attribute::AttributeViewer< Content::JointTransformsAttribute > jointTransforms( entity );
  if ( jointTransforms.Valid() )
  {
    payload_size = (u32)jointTransforms->m_Transforms.size() * sizeof( RPC::MobyJointInfo );
    payload = new u8[ payload_size ];

    RPC::MobyJointInfo* jointInfo = reinterpret_cast<RPC::MobyJointInfo*>( payload );
    for each ( const Content::M_IDToTransform::value_type& val in jointTransforms->m_Transforms )
    {
      const UniqueID::TUID& id = val.first;
      const Math::Matrix4& transform = val.second;

      jointInfo->m_ID = id;
      memcpy( &jointInfo->m_Transform, &transform, sizeof( jointInfo->m_Transform ) );

      Swizzle( jointInfo );

      jointInfo++;
    }
  }
}

void RemoteEditor::SendMobyAnimJointMats( Asset::Entity* entity )
{
  u8* payload = NULL;
  u32 payload_size = 0;

  CreateMobyPayload( entity, payload, payload_size );

  RPC::ILunaViewHostRemote* lunaView = RuntimeConnection::GetRemoteLevelView();

  RPC::SetMobyAnimJointMatsParam param;
  param.m_ID = entity->m_ID;
  lunaView->SetMobyAnimJointMats( &param, payload, payload_size );
}

void RemoteEditor::SendEntityInstance( Asset::Entity* entity )
{
  tuid assetClassId = entity->GetEntityAssetID();
  Asset::AssetClassPtr assetClass = Asset::AssetClass::FindAssetClass( assetClassId );

  i32 updateClass = -1;
  if ( assetClass->GetEngineType() == Asset::EngineTypes::Moby )
  {
    std::string runtimeClass = entity->GetRuntimeClass();

    Symbol::Inheritance::GetValue( runtimeClass, updateClass );
  }

  RPC::ILunaViewHostRemote* lunaView = RuntimeConnection::GetRemoteLevelView();

  if(!m_Enabled || !lunaView || !RuntimeConnection::IsConnected())
  {
    return;
  }

  RPC::PlaceInstanceParam param;
  param.m_Type = GetAssetType( assetClass );

  if ( param.m_Type != -1 )
  {
    param.m_AssetId = assetClass->m_AssetClassID;
    param.m_ID = entity->m_ID;
    memcpy( &param.m_Transform, &entity->m_GlobalTransform, sizeof( param.m_Transform ) );

    size_t nameLen = entity->GetName().length() + 1;
    if ( nameLen > 256 )
      nameLen = 256;

    memcpy( param.m_Name, entity->GetName().c_str(), nameLen );

    param.m_UpdateClass = updateClass;

    u8* payload = NULL;
    u32 payload_size = 0;

    if ( param.m_Type == RPC::EngineTypes::Moby )
    {
      CreateMobyPayload( entity, payload, payload_size );
    }

    Attribute::AttributeViewer< Asset::InstanceCollisionAttribute > instanceCollision( entity, true );
    param.m_HighResCollisionEnabled = instanceCollision->m_EnableHiResCollision;
    param.m_LowResCollisionEnabled = instanceCollision->m_EnableLowResCollision;

    param.m_ShaderGroupIndex = 0;
    Attribute::AttributeViewer< Asset::ShaderGroupAttribute > shaderGroup( assetClass, false );
    if ( shaderGroup.Valid() )
    {
      Attribute::AttributeViewer< Asset::ShaderGroupOverrideAttribute > shaderGroupOverride( entity, false );
      if ( shaderGroupOverride.Valid() )
      {
        i32 shaderIndex = Asset::ShaderGroupOverrideAttribute::GetShaderGroupIndexForInstance( entity );
        if ( shaderIndex >= 0 )
        {
          param.m_ShaderGroupIndex = shaderIndex;
        }
      }
    }

    lunaView->PlaceInstance( &param, payload, payload_size );

    // if we have a foliage attribute, attempt to transform the foliage as well
    // we assume that the foliage has the same uid as the tie
    Attribute::AttributeViewer< Asset::FoliageAttribute > foliage( assetClass );
    if ( foliage.Valid() )
    {
      param.m_Type = RPC::EngineTypes::Foliage;
      param.m_AssetId = foliage->m_FileID;
      lunaView->PlaceInstance( &param, NULL, 0 );
    }
  }
}

void RemoteEditor::AddEntityInstance( Asset::Entity* entity )
{
  m_Scene.Add( entity );

  SendEntityInstance( entity );
}

void RemoteEditor::RemoveEntityInstance( Asset::Entity* entity )
{
  Asset::EntityAsset* entityClass = entity->GetEntityAsset();

  m_Scene.Remove( entity );

  RemoveInstanceParam param;
  param.m_Type = GetAssetType( entityClass );

  if(param.m_Type == -1)
  {
    return;
  }

  param.m_ID = entity->m_ID;

  RPC::ILunaViewHostRemote* lunaView = RuntimeConnection::GetRemoteLevelView();

  lunaView->RemoveInstance( &param );

  // if we have a foliage attribute, attempt to transform the foliage as well
  // we assume that the foliage has the same uid as the tie
  Attribute::AttributeViewer< Asset::FoliageAttribute > foliage( entityClass );
  if ( foliage.Valid() )
  {
    param.m_Type = RPC::EngineTypes::Foliage;
    lunaView->RemoveInstance( &param );
  }
}

void RemoteEditor::SendLight( Content::Light* light )
{
  RPC::ILunaViewHostRemote* lunaView = RuntimeConnection::GetRemoteLevelView();

  AddLightParam param;

  param.m_ID = light->m_ID;
  strncpy( param.m_Name, light->GetName().c_str(), sizeof( param.m_Name ) );
  param.m_Name[ sizeof(param.m_Name) - 1] = 0;

  param.m_Type = RPC::LightTypes::Point;

  Content::SpotLight* spot = Reflect::ObjectCast< Content::SpotLight >( light );
  if ( spot )
  {
    param.m_Type = RPC::LightTypes::Spot;
  }

  lunaView->AddLight( &param );

  SetLightParams( light );
  TransformLight( light );
}

void RemoteEditor::AddLight( Content::Light* light )
{
  m_Scene.Add( light );

  SendLight( light );
}

void RemoteEditor::RemoveLight( Content::Light* light )
{
  RPC::ILunaViewHostRemote* lunaView = RuntimeConnection::GetRemoteLevelView();

  m_Scene.Remove( light );

  RemoveLightParam param;

  param.m_ID = light->m_ID;

  lunaView->RemoveLight( &param );
}

void RemoteEditor::SetLightParams( Content::Light* light )
{
  LightParametersParam param;
  //Clear
  ZeroMemory(&param, sizeof(LightParametersParam));

  param.m_ID = light->m_ID;

  light->m_Color.ToLinearFloat( param.m_Color.x, param.m_Color.y, param.m_Color.z );

  param.m_DrawDist = light->m_DrawDist;
  param.m_RadiusInner = 0.f;
  param.m_Radius = 0.f;

  Content::PointLight* pointLight = Reflect::ObjectCast<Content::PointLight>(light);
  if (pointLight)
  {
    param.m_RadiusInner = pointLight->GetInnerRadius();
    param.m_Radius = pointLight->GetOuterRadius();
  }

  param.m_Flags = 0;

  if( light->m_FlareType )
  {
    switch(light->m_FlareType)
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

  Content::SpotLight* spotLight = Reflect::ObjectCast<Content::SpotLight>(light);
  if (spotLight)
  {
    param.m_RadiusInner = spotLight->GetInnerRadius();
    param.m_Radius = spotLight->GetOuterRadius();

    param.m_InnerConeAngle = spotLight->m_InnerConeAngle;
    param.m_OuterConeAngle = spotLight->m_OuterConeAngle;

    if ( spotLight->m_ShadowMapHiRes )
    {
      param.m_Flags |= IGPS3::RTL_SHADOW_MAP_HI_RES;
    }

    if( spotLight->m_GodRayEnabled )
    {
      Math::Vector4 clipPlane = spotLight->GetGodRayClipPlane();

      param.m_Flags   |= IGPS3::RTL_GODRAY;
      param.m_Color.w  = spotLight->m_GodRayOpacity;

      param.m_GodRayDensity     = spotLight->m_GodRayDensity;
      param.m_GodRayFadeNear    = spotLight->m_GodRayFadeNear;
      param.m_GodRayFadeFar     = spotLight->m_GodRayFadeFar;
      param.m_GodRayQuality     = spotLight->m_GodRayQuality;

      param.m_GodRayClipPlaneX  = clipPlane.x;
      param.m_GodRayClipPlaneY  = clipPlane.y;
      param.m_GodRayClipPlaneZ  = clipPlane.z;
      param.m_GodRayClipPlaneD  = clipPlane.w;
    }
  }

  if ( light->m_KillIfInactive )
  {
    param.m_Flags |= IGPS3::RTL_KILL_IF_INACTIVE;
  }

  if ( light->m_CastShadows )
  {
    param.m_Flags |= IGPS3::RTL_CASTS_SHADOWS;
  }

  if ( light->m_AllowOversized )
  {
    param.m_Flags |= IGPS3::RTL_ALLOW_OVERSIZED;
  }

  RPC::ILunaViewHostRemote* lunaView = RuntimeConnection::GetRemoteLevelView();
  lunaView->SetLightParameters( &param );
  SetLightAnimation( light );
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
void RemoteEditor::SetLightAnimation( Content::Light* contentLight )
{
    if( contentLight->m_AnimationColor.size() >= 3 || contentLight->m_AnimationIntensity.size() >= 3 )
    {
      LightAnimationParam param;

      //Clear
      ZeroMemory(&param, sizeof(LightAnimationParam));

      memcpy( &param.m_ID, &contentLight->m_ID, sizeof( param.m_ID ) );

      if( contentLight->m_RandomAnimOffset )
      {
        param.m_Flags |= IGPS3::RTL_ANIM_RAND_OFFSET;
      }

      u32 numColorKeys = (u32)contentLight->m_AnimationColor.size();
      u32 numIntensityKeys = (u32)contentLight->m_AnimationIntensity.size();

      ColorKeyFrames mostKeys( ( numColorKeys >= numIntensityKeys ) ? contentLight->m_AnimationColor : contentLight->m_AnimationIntensity );
      ColorKeyFrames fewestKeys( ( numColorKeys >= numIntensityKeys ) ? contentLight->m_AnimationIntensity : contentLight->m_AnimationColor );

      param.m_NumKeyFrames = (u32)mostKeys.m_KeyFrames.size();

      Math::Vector4 lightColor;
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
      RPC::ILunaViewHostRemote* lunaView = RuntimeConnection::GetRemoteLevelView();
      lunaView->SetLightAnimation( &param );
    }
}

void RemoteEditor::TransformLight( Content::Light* light )
{
  TransformLightParam param;

  param.m_ID = light->m_ID;

  Math::Scale scale;
  Math::Matrix3 rotate;
  Math::Vector3 translate;

  light->m_GlobalTransform.Decompose( scale, rotate, translate );

  param.m_Position.x = translate.x;
  param.m_Position.y = translate.y;
  param.m_Position.z = translate.z;
  param.m_Position.w = 1.0;

  // figure out the light direction
  Math::Vector4 dir = Math::Vector4( 0.0, 0.0, 1.0, 0.0 );
  dir = light->m_GlobalTransform * dir;
  dir.Normalize();

  memcpy( &param.m_Direction, &dir, sizeof( param.m_Direction ) );

  RPC::ILunaViewHostRemote* lunaView = RuntimeConnection::GetRemoteLevelView();
  lunaView->TransformLight( &param );

  //This will guarantee the updating of the GodRay clip plane
  Content::SpotLight* spotLight = Reflect::ObjectCast<Content::SpotLight>(light);
  if (spotLight && spotLight->m_GodRayEnabled)
  {
    SetLightParams(light);
  }
}

void RemoteEditor::TweakShaderGroup( const Asset::Entity* entity )
{
  RPC::ShaderGroupParam param;
  memset( &param, 0, sizeof( param ) );
  param.m_ID = entity->m_ID;
  param.m_EngineType = GetAssetType( entity->GetEntityAsset() );

  Attribute::AttributeViewer< Asset::ShaderGroupAttribute > shaderGroup( entity );
  Attribute::AttributeViewer< Asset::ShaderGroupOverrideAttribute > shaderGroupOverride( entity );
  if ( shaderGroupOverride.Valid() && shaderGroup.Valid() )
  {
    param.m_ShaderGroupIndex = Asset::ShaderGroupOverrideAttribute::GetShaderGroupIndexForInstance( entity );
    if ( param.m_ShaderGroupIndex < 0 )
    {
      param.m_ShaderGroupIndex = 0;
    }
  }

  RuntimeConnection::GetRemoteLevelView()->TweakShaderGroup( &param );
}

void RemoteEditor::SendScene()
{
  SendMode();

  Asset::V_Entity entities;
  m_Scene.GetAll< Asset::Entity >( entities );

  Asset::V_Entity::iterator entityIt = entities.begin();
  Asset::V_Entity::iterator entityEnd = entities.end();
  for (; entityIt != entityEnd; ++entityIt )
  {
    Asset::Entity* entity = (*entityIt);

    SendEntityInstance( entity );
  }

  Content::V_Light lights;
  m_Scene.GetAll< Content::Light >( lights );

  Content::V_Light::iterator lightIt = lights.begin();
  Content::V_Light::iterator lightEnd = lights.end();
  for ( ; lightIt != lightEnd; ++lightIt )
  {
    Content::Light* light = (*lightIt);

    SendLight( light );
  }

  if ( m_SelectedEntity )
  {
    FrameEntityInstance( m_SelectedEntity->GetEntityAssetID(), m_SelectedEntity->m_ID );
  }


  for(V_Attachment::iterator attIt = m_Attachments.begin(); attIt != m_Attachments.end(); ++attIt)
  {
    SendCreateAttachment( *attIt );
  }
}

void RemoteEditor::ClearScene()
{
  m_Scene.Reset();
  m_SelectedEntity = NULL;

  m_Attachments.clear();
}

void SetupLevelParams(RPC::LevelAttributesParam& levelSettings, const Asset::LevelAssetPtr& level)
{
  NOC_ASSERT(level->m_AssetClassID != TUID::Null);

  memset( &levelSettings, 0, sizeof( levelSettings ) );

  std::string localProcessedFolder = FinderSpecs::Occlusion::BUILT_FOLDER.GetFolder() + Asset::AssetClass::GetQualifiedName( level ) + "/";
  std::string file_name = FinderSpecs::Occlusion::DATABASE_FILE.GetFile( localProcessedFolder );
  if ( FileSystem::Exists( file_name ) )
  {
    FileSystem::StripPrefix( Finder::ProjectRoot(), file_name );

    if ( file_name.length() < sizeof( levelSettings.m_OcclusionDbFile ) )
    {
      strcpy( levelSettings.m_OcclusionDbFile, file_name.c_str() );
    }
  }

  for ( u32 i = 0; i < RPC_MAX_SKIES; ++i )
  {
    levelSettings.m_SkyTuid[ i ] = TUID::Null;
  }

  levelSettings.m_LevelTuid = level->m_AssetClassID;
  levelSettings.m_DefaultCubemapTuid  = level->m_DefaultCubeMap;
  levelSettings.m_WaterCubemapTuid    = level->m_WaterCubeMap;
  levelSettings.m_NearClip            = level->m_NearClipDist;
  levelSettings.m_FarClip             = level->m_FarClipDist;
  levelSettings.m_NumSkyTuids         = MIN( (u32) level->m_SkyAssets.size(), RPC_MAX_SKIES );

  u32 currentSky = 0;
  S_tuid::iterator skyItr = level->m_SkyAssets.begin();
  S_tuid::iterator skyEnd = level->m_SkyAssets.end();
  for ( ; skyItr != skyEnd; ++skyItr )
  {
    levelSettings.m_SkyTuid[ currentSky ] = *skyItr;
    ++currentSky;
    if ( currentSky >= levelSettings.m_NumSkyTuids )
    {
      break;
    }
  }

  Nocturnal::BasicBufferPtr buffer = new Nocturnal::BasicBuffer;
  BuilderUtil::WriteLevelAttributes( buffer, level, NULL);

  NOC_ASSERT( sizeof( IGPS3::LevelAttributes ) == buffer->GetSize() );

  memcpy( &levelSettings.m_LevelAttributes, buffer->GetData(), sizeof( levelSettings.m_LevelAttributes ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
inline u32 GenerateTextureMipData(const tuid&                   texture_id,
                                  IG::Texture*&                 texture,
                                  IG::MipGenOptions&            mg,
                                  IG::MipSet::RuntimeSettings&  runtime,
                                  u32                           chunk_id,
                                  u32*                          textureHeader,
                                  u8*&                          texData)
{
  //Generate the mips
  IG::MipSet* mips  = texture->GenerateMipSet(mg, runtime);

  //No longer need this around
  delete texture;
  texture = NULL;

  //Validate the mips
  if(mips == NULL)
  {
    Console::Warning("Unable to generate mips!\n");
    return 0;
  }

  std::string dataPath    = Finder::GetBuiltFolder( texture_id ) + "texture.dat";

  {
    IG::IGSerializer  dataWriter(true);

    Nocturnal::BasicBufferPtr data   = dataWriter.AddChunk(chunk_id + 0x0,  IG::CHUNK_TYPE_SINGLE, 0,0,0);
    Nocturnal::BasicBufferPtr header = dataWriter.AddChunk(chunk_id + 0x1,  IG::CHUNK_TYPE_SINGLE, sizeof(IG::IGTexture),0,0);
    Nocturnal::BasicBufferPtr misc   = dataWriter.AddChunk(chunk_id + 0x2,  IG::CHUNK_TYPE_SINGLE, sizeof(TextureProcess::TextureDebugInfo),0,0);
    TextureProcess::WriteMipSet(mips, header, data, misc);

    NOC_ASSERT(data->GetSize() != 0);

    u32 texDataSize  = data->GetSize();
    texData          = new u8[data->GetSize()];
    memcpy(texData, data->GetData(), texDataSize);
    memcpy(textureHeader, header->GetData(), header->GetSize());

    dataWriter.WriteToFile(dataPath.c_str());
    delete mips;

    return texDataSize;
  }

  return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void  CompileAndStreamWeatherTextures(ILunaViewHost*                    host,
                                      RPC::LevelWeatherAttributesParam& params,
                                      const tuid&                       particlesTextureID,
                                      const tuid&                       detailNormalMapID)
{
  u8* texParticlesData = NULL;

  //Particles textures
  if(particlesTextureID != TUID::Null)
  {
    //Generate the built folder as well as the built file path
    File::ManagedFilePtr  textureFile = File::GlobalManager().GetManagedFile( particlesTextureID );

    //Verify the texture file
    if(textureFile)
    {
      IG::Texture* texture = IG::Texture::LoadFile(textureFile->m_Path.c_str(), true, NULL);

      //Validate the texture
      if(texture == NULL)
      {
        Console::Warning("Unable to load texture: %s\n", textureFile->m_Path.c_str());
      }
      else
      {
        //Check the size of the texture
        if((texture->m_Height > Asset::WeatherAttribute::s_MaxTextureEdgeSize) ||
          (texture->m_Width  > Asset::WeatherAttribute::s_MaxTextureEdgeSize))
        {
          Console::Warning("Weather texture size is ridiculously large: <%s>!\n"
                           "Please reduce the size of your texture to something less or equal to <%d>.\n",
                           textureFile->m_Path.c_str(),
                           Asset::WeatherAttribute::s_MaxTextureEdgeSize);
        }

        IG::MipGenOptions mg;
        mg.m_Filter             = IG::MIP_FILTER_POINT_COMPOSITE;
        mg.m_PostFilter         = IG::IMAGE_FILTER_HIGH_PASS;
        mg.m_Levels             = 0;
        mg.m_ConvertToSrgb      = true;
        mg.m_OutputFormat       = IG::OUTPUT_CF_DXT5;

        IG::MipSet::RuntimeSettings runtime;
        runtime.m_wrap_u        = IG::UV_WRAP;
        runtime.m_wrap_v        = IG::UV_WRAP;
        runtime.m_wrap_w        = IG::UV_WRAP;
        runtime.m_filter        = IG::FILTER_LINEAR_SELECT_MIP;
        runtime.m_direct_uvs    = false;
        runtime.m_expand_range  = false;
        runtime.m_srgb_expand_a = false;
        runtime.m_srgb_expand_r = true;
        runtime.m_srgb_expand_g = true;
        runtime.m_srgb_expand_b = true;

        //Write the data
        params.m_ParticlesTextureSize  = GenerateTextureMipData(particlesTextureID, texture, mg, runtime,
                                                                IGG::FILECHUNK_WEATHER_PARTICLE_TEXTURE_DATA,
                                                                params.m_ParticlesTextureHeader,
                                                                texParticlesData);
        //Validate the size
        if(params.m_ParticlesTextureSize != 0)
        {
          params.m_ParticlesTextureKeep = true;
        }
      }
    }
  }

  u8* detailNormalMapData = NULL;

  //Detail Normal map texture
  if(detailNormalMapID != TUID::Null)
  {
    //Generate the built folder as well as the built file path
    File::ManagedFilePtr  textureFile = File::GlobalManager().GetManagedFile( detailNormalMapID );

    //Verify the texture file
    if(textureFile)
    {
      IG::Texture* texture = IG::Texture::LoadFile(textureFile->m_Path.c_str(), false, NULL);

      //Validate the texture
      if(texture == NULL)
      {
        Console::Warning("Unable to load texture: %s\n", textureFile->m_Path.c_str());
      }
      else
      {
        //Generate the partial derivatives
        //r = normal_dd.y;
        //g = normal_dd.y;
        //b = normal_dd.y;
        //a = normal_dd.x;
        texture->PrepareFor2ChannelNormalMap(true, true);

        IG::MipGenOptions mg;
        mg.m_Filter             = IG::MIP_FILTER_POINT_COMPOSITE;
        mg.m_PostFilter         = IG::IMAGE_FILTER_HIGH_PASS;
        mg.m_Levels             = 0;
        mg.m_ConvertToSrgb      = false;
        mg.m_OutputFormat       = IG::OUTPUT_CF_DXT5;

        IG::MipSet::RuntimeSettings runtime;
        runtime.m_wrap_u        = IG::UV_WRAP;
        runtime.m_wrap_v        = IG::UV_WRAP;
        runtime.m_wrap_w        = IG::UV_WRAP;
        runtime.m_filter        = IG::FILTER_ANISO_2_LINEAR_MIP;
        runtime.m_direct_uvs    = false;
        runtime.m_expand_range  = true;
        runtime.m_srgb_expand_a = false;
        runtime.m_srgb_expand_r = false;
        runtime.m_srgb_expand_g = false;
        runtime.m_srgb_expand_b = false;
        runtime.m_red_channel   = IG::COLOR_CHANNEL_GET_FROM_A;
        runtime.m_green_channel = IG::COLOR_CHANNEL_GET_FROM_R;
        runtime.m_blue_channel  = IG::COLOR_CHANNEL_FORCE_ZERO;
        runtime.m_alpha_channel = IG::COLOR_CHANNEL_FORCE_ZERO;

        //Write the data
        params.m_WetnessDetailMapSize = GenerateTextureMipData(particlesTextureID, texture, mg, runtime,
                                                               IGG::FILECHUNK_WEATHER_DETAIL_NORMALMAP_DATA,
                                                               params.m_WetnessDetailMapHeader,
                                                               detailNormalMapData);

        //Validate the size
        if(params.m_WetnessDetailMapSize != 0)
        {
          params.m_WetnessDetailMapKeep = true;
        }
      }
    }
  }

  u32 total_buffer_size = params.m_ParticlesTextureSize  + params.m_WetnessDetailMapSize;
  u8* total_tex_data    = NULL;

  //Generate the combined data
  if(total_buffer_size)
  {
    total_tex_data  = new u8[total_buffer_size];

    if(params.m_ParticlesTextureSize)
    {
      memcpy(total_tex_data, texParticlesData, params.m_ParticlesTextureSize);
      params.m_ParticlesTextureOffset = 0;

      //Clean-up
      delete[] texParticlesData;
    }

    if(params.m_WetnessDetailMapSize)
    {
      memcpy(total_tex_data + params.m_ParticlesTextureSize,
             detailNormalMapData,
             params.m_WetnessDetailMapSize);
      params.m_WetnessDetailMapOffset = params.m_ParticlesTextureSize;
      //Clean-up
      delete[] detailNormalMapData;
    }
  }

  //Send data
  host->SetLevelWeatherAttributes(&params, total_tex_data, total_buffer_size);

  //Clean-up
  delete[] total_tex_data;
}

///////////////////////////////////////////////////////////////////////////
// Thread for updating weather particles texture
//
class WeatherAttributesTextureUpdateThread : public UIToolKit::DialogWorkerThread
{
private:
  RPC::LevelWeatherAttributesParam  m_Params;
  ILunaViewHost*                    m_LunaViewHost;
  tuid                              m_ParticlesTextureID;
  tuid                              m_DetailNormalMapID;

public:
  // Constructor
  WeatherAttributesTextureUpdateThread( HANDLE                                  evtHandle,
                                        wxDialog*                               dlg,
                                        const RPC::LevelWeatherAttributesParam& src_param,
                                        const tuid&                             particlesTextureID,
                                        const tuid&                             detailNormalMapID,
                                        ILunaViewHost*                          lunaViewHost )
    : UIToolKit::DialogWorkerThread( evtHandle, dlg )
    , m_Params(src_param)
    , m_ParticlesTextureID( particlesTextureID )
    , m_DetailNormalMapID(  detailNormalMapID )
    , m_LunaViewHost( lunaViewHost )
  {
  }

  // Do the work
  virtual void DoWork() NOC_OVERRIDE
  {
    if ( RuntimeConnection::IsConnected())
    {
      CompileAndStreamWeatherTextures(m_LunaViewHost, m_Params, m_ParticlesTextureID, m_DetailNormalMapID);
    }
  }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
inline u32 LoadTextureMipData(const tuid&  texture_id,
                              u32          chunk_id,
                              u32*         textureHeader,
                              u8*&         texData)
{
  //File content
  IGSerializer  builtData(true);
  u32           texDataSize = 0;

  try
  {
    std::string builtFile    = Finder::GetBuiltFolder( texture_id ) + "texture.dat";
    builtData.ReadFromFile(builtFile.c_str());

    try
    {
      Nocturnal::BasicBufferPtr data   = builtData.GetChunk(chunk_id + 0x00, 0);
      Nocturnal::BasicBufferPtr header = builtData.GetChunk(chunk_id + 0x01, 0);

      NOC_ASSERT(data->GetSize() != 0);

      texDataSize  = data->GetSize();
      texData      = new u8[data->GetSize()];
      memcpy(texData, data->GetData(), texDataSize);

      memcpy(textureHeader, header->GetData(), header->GetSize());
    }
    catch (const Nocturnal::Exception& ex)
    {
      Console::Error( "%s\n", ex.what() );
    }
  }
  catch (const Nocturnal::Exception& ex)
  {
    //This is an exceptional case where the attribute was assigned during an off-line session
    //and the data was never generated for this texture
    Console::Error( "%s\n", ex.what() );
  }

  return texDataSize;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
void RemoteEditor::SetLevelWeatherAttributes(const Asset::LevelAssetPtr& level, u32 flags)
{
  RPC::LevelWeatherAttributesParam  params;
  RPC::ILunaViewHostRemote*         lunaView    = RuntimeConnection::GetRemoteLevelView();
  u32                               texDataSize = 0;
  u8*                               texData     = NULL;
  bool                              useLevelFog = true;
  memset(&params, 0, sizeof(params));

  Attribute::AttributeViewer< Asset::WeatherAttribute > weatherAttr( level );

  if(weatherAttr.Valid())
  {
    //Reload the cube-map if necessary
    if(flags & Luna::WEATHER_OP_CUBEMAP_RELOAD)
    {
      wxBusyCursor bc;
      ReloadAsset(weatherAttr->m_WetnessCubeMap);
    }

    //Wetness
    params.m_WetnessCubeMapId                 = (weatherAttr->m_WetnessCubeMap != TUID::Null) ? weatherAttr->m_WetnessCubeMap : level->m_DefaultCubeMap;
    params.m_WetnessCubeSpecType              = weatherAttr->m_WetnessCubeSpecType;
    params.m_WetnessCubeBias                  = weatherAttr->m_WetnessCubeBias;
    params.m_WetnessIntensity                 = weatherAttr->m_WetnessIntensity;
    params.m_WetnessAmbient                   = weatherAttr->m_WetnessAmbient;

    params.m_WetnessTintRed                   = SRGBToLinear(f32(weatherAttr->m_WetnessTint.r)/255.f);
    params.m_WetnessTintGreen                 = SRGBToLinear(f32(weatherAttr->m_WetnessTint.g)/255.f);
    params.m_WetnessTintBlue                  = SRGBToLinear(f32(weatherAttr->m_WetnessTint.b)/255.f);

    params.m_WetnessFadeFarDistance           = weatherAttr->m_WetnessFadeFarDistance;
    params.m_WetnessFadeInterval              = weatherAttr->m_WetnessFadeInterval;
    params.m_WetnessScrollSpeed               = weatherAttr->m_WetnessScrollSpeed;
    params.m_WetnessUVScale                   = weatherAttr->m_WetnessUVScale;
    params.m_WetnessRipplesSplashWeight       = weatherAttr->m_WetnessRipplesSplashWeight;
    params.m_WetnessRipplesSplashRate         = weatherAttr->m_WetnessRipplesSplashRate;
    params.m_WetnessRipplesDamping            = weatherAttr->m_WetnessRipplesDamping;

    //Droplets
    params.m_ParticlesTextureKeep             = true;
    params.m_DropletsColorRed                 = SRGBToLinear(f32(weatherAttr->m_DropletsColor.r)/255.f);
    params.m_DropletsColorGreen               = SRGBToLinear(f32(weatherAttr->m_DropletsColor.g)/255.f);
    params.m_DropletsColorBlue                = SRGBToLinear(f32(weatherAttr->m_DropletsColor.b)/255.f);
    params.m_DropletsColorAlpha               = weatherAttr->m_DropletsAlpha;
    params.m_DropletsSize                     = weatherAttr->m_DropletsSize;
    params.m_DropletsBloom                    = weatherAttr->m_DropletsBloom;
    params.m_DropletsFallSpeed                = weatherAttr->m_DropletsFallSpeed;
    params.m_DropletsDistance                 = weatherAttr->m_DropletsDistance;
    params.m_DropletsDensity                  = weatherAttr->m_DropletsDensity;
    params.m_DropletsTurbulence               = weatherAttr->m_DropletsTurbulence;
    params.m_DropletsMotionBlur               = weatherAttr->m_DropletsMotionBlur;

    //Wind
    params.m_WindIntensity                    = weatherAttr->m_WindIntensity;
    params.m_WindAzimuth                      = weatherAttr->m_WindAzimuth;

    //Wind Gusts  
    params.m_WindGustsIntensityMin            = weatherAttr->m_WindGustsIntensityMin;
    params.m_WindGustsIntensityMax            = weatherAttr->m_WindGustsIntensityMax;
    params.m_WindGustsIntervalMin             = weatherAttr->m_WindGustsIntervalMin;
    params.m_WindGustsIntervalMax             = weatherAttr->m_WindGustsIntervalMax;
    params.m_WindGustsDurationMin             = weatherAttr->m_WindGustsDurationMin;
    params.m_WindGustsDurationMax             = weatherAttr->m_WindGustsDurationMax;

    //Lightning
    params.m_LightningIntensityMin            = weatherAttr->m_LightningIntensityMin;
    params.m_LightningIntensityMax            = weatherAttr->m_LightningIntensityMax;
    params.m_LightningIntervalMin             = weatherAttr->m_LightningIntervalMin;
    params.m_LightningIntervalMax             = weatherAttr->m_LightningIntervalMax;
    params.m_LightningDurationMin             = weatherAttr->m_LightningDurationMin;
    params.m_LightningDurationMax             = weatherAttr->m_LightningDurationMax;

    //Fog
    {
      tuid   fogCustomPalette = weatherAttr->m_FogCustomPalette;
      size_t fogWeightsSize   = weatherAttr->m_FogWeights.size();
      size_t fogColorSize     = weatherAttr->m_FogColors.size();

      //Check if we have any keyed values or a custom palette
      if(fogWeightsSize || fogColorSize || (fogCustomPalette != TUID::Null))
      {
        //We have a valid palette
        ColorPalette::Palette::GeneratePalette(weatherAttr->m_FogWeights,
                                               weatherAttr->m_FogColors,
                                               weatherAttr->m_FogCustomPalette,
                                               params.m_FogData);
        useLevelFog = false;
      }
    }
  }

  //If we are not overriding the fog values use the level fog attribute
  if(useLevelFog == true)
  {
    Content::V_PostProcessingVolume postprocvols;
    m_Scene.GetAll< Content::PostProcessingVolume >( postprocvols );

    Content::V_PostProcessingVolume::iterator postprocvolIt = postprocvols.begin();
    Content::V_PostProcessingVolume::iterator postprocvolEnd = postprocvols.end();
    for ( ; postprocvolIt != postprocvolEnd; ++postprocvolIt )
    {
      Content::PostProcessingVolume* postprocvol = (*postprocvolIt);
      Attribute::AttributeViewer< Content::PostEffectsFogAttribute > fogAttr( postprocvol);

      if(fogAttr.Valid())
      {
        tuid   fogCustomPalette = fogAttr->m_CustomPalette;
        size_t fogWeightsSize   = fogAttr->m_Weight.size();
        size_t fogColorSize     = fogAttr->m_Color.size();

        //Check if we have any keyed values or a custom palette
        if(fogWeightsSize || fogColorSize || (fogCustomPalette != TUID::Null))
        {
          //We have a valid palette
          ColorPalette::Palette::GeneratePalette(fogAttr->m_Weight,
                                                 fogAttr->m_Color,
                                                 fogAttr->m_CustomPalette,
                                                 params.m_FogData);
          break;
        }
      }
    }
  }

  //Texture
  if(weatherAttr.Valid())
  {
    tuid particlesTextureIdForRebuild = TUID::Null;
    tuid detailNormalMapIdForRebuild  = TUID::Null;

    //Check if we need to build the texture
    if(flags & Luna::WEATHER_OP_PARTICLE_TEXTURE_REBUILD)
    {
      if(weatherAttr->m_DropletsTexture != TUID::Null)
      {
        particlesTextureIdForRebuild            = weatherAttr->m_DropletsTexture;
      }
      params.m_ParticlesTextureKeep = false;
    }

    //Check if we need to build the texture
    if(flags & Luna::WEATHER_OP_DETAIL_NORMALMAP_REBUILD)
    {
      if(weatherAttr->m_DetailNormalMapTexture != TUID::Null)
      {
        detailNormalMapIdForRebuild             = weatherAttr->m_DetailNormalMapTexture;
      }
      params.m_WetnessDetailMapKeep = false;
    }

    //Rebuild-step!
    if((particlesTextureIdForRebuild != TUID::Null) || (detailNormalMapIdForRebuild != TUID::Null))
    {
      std::string msg( "Updating Weather Texture(s): " );

      ObjectUpdateDialog dlg(m_Editor, "Weather Texture(s) Update", msg );

      dlg.InitThread( new WeatherAttributesTextureUpdateThread( CreateEvent(NULL,
                                                                            TRUE,
                                                                            FALSE,
                                                                            "Weather Texture(s) Update Thread Event" ),
                                                                            &dlg,
                                                                            params,
                                                                            particlesTextureIdForRebuild,
                                                                            detailNormalMapIdForRebuild,
                                                                            lunaView ) );
      dlg.ShowModal();
      return;
    }

    //Reload step
    if(flags & Luna::WEATHER_OP_TEXTURES2D_RELOAD)
    {
      u8* particlesData = NULL;
      if(weatherAttr->m_DropletsTexture != TUID::Null)
      {
        params.m_ParticlesTextureSize = LoadTextureMipData( weatherAttr->m_DropletsTexture,
                                                            IGG::FILECHUNK_WEATHER_PARTICLE_TEXTURE_DATA,
                                                            params.m_ParticlesTextureHeader,
                                                            particlesData);
        if(params.m_ParticlesTextureSize == 0)
        {
          CompileAndStreamWeatherTextures(lunaView, params, weatherAttr->m_DropletsTexture, weatherAttr->m_DetailNormalMapTexture);
          return;
        }

        params.m_ParticlesTextureKeep = true;
      }

      u8* detailData = NULL;
      if(weatherAttr->m_DetailNormalMapTexture != TUID::Null)
      {
        params.m_WetnessDetailMapSize = LoadTextureMipData( weatherAttr->m_DetailNormalMapTexture,
                                                            IGG::FILECHUNK_WEATHER_DETAIL_NORMALMAP_DATA,
                                                            params.m_WetnessDetailMapHeader,
                                                            detailData);
        if(params.m_WetnessDetailMapSize == 0)
        {
          delete[] particlesData;
          CompileAndStreamWeatherTextures(lunaView, params, weatherAttr->m_DropletsTexture, weatherAttr->m_DetailNormalMapTexture);
          return;
        }

        params.m_WetnessDetailMapKeep = true;
      }

      //Total size
      texDataSize = params.m_ParticlesTextureSize  + params.m_WetnessDetailMapSize;

      //Generate the combined data
      if(texDataSize)
      {
        texData  = new u8[texDataSize];

        if(params.m_ParticlesTextureSize)
        {
          memcpy(texData, particlesData, params.m_ParticlesTextureSize);
          params.m_ParticlesTextureOffset = 0;

          //Clean-up
          delete[] particlesData;
        }

        if(params.m_WetnessDetailMapSize)
        {
          memcpy(texData + params.m_ParticlesTextureSize,
                 detailData,
                 params.m_WetnessDetailMapSize);
          params.m_WetnessDetailMapOffset = params.m_ParticlesTextureSize;
          //Clean-up
          delete[] detailData;
        }
      }
    }
  }

  //Send the data
  lunaView->SetLevelWeatherAttributes(&params, texData, texDataSize);

  delete[] texData;
}

void RemoteEditor::SetLevelAttributes(const Asset::LevelAssetPtr& level)
{
  RPC::ILunaViewHostRemote* lunaView = RuntimeConnection::GetRemoteLevelView();

  RPC::LevelAttributesParam levelSettings;
  SetupLevelParams(levelSettings, level);
  lunaView->SetLevelAttributes(&levelSettings);
}

void RemoteEditor::ClearLevelAttributes()
{
  RPC::ILunaViewHostRemote* lunaView = RuntimeConnection::GetRemoteLevelView();

  lunaView->ClearLevelAttributes();
}

void RemoteEditor::SendMode()
{
  RPC::ILunaViewHostRemote* lunaView = RuntimeConnection::GetRemoteLevelView();

  RPC::SetModeParam mode;
  mode.m_Mode = m_Mode;
  lunaView->SetMode(&mode);
}

void RemoteEditor::SetMode( RPC::LunaViewMode mode )
{
  m_Mode = mode;

  SendMode();
}

void RemoteEditor::SetInstanceCollision( const Asset::Entity* entity )
{
  RPC::ILunaViewHostRemote* lunaView = RuntimeConnection::GetRemoteLevelView();

  Attribute::AttributeViewer< Asset::InstanceCollisionAttribute > instanceCollision( entity, true );

  RPC::SetInstanceCollisionParam param;
  param.m_Type = GetAssetType(entity->GetEntityAsset());
  param.m_EnableHighResCollision = instanceCollision->m_EnableHiResCollision;
  param.m_EnableLowResCollision = instanceCollision->m_EnableLowResCollision;
  param.m_ID = entity->m_ID;
  lunaView->SetInstanceCollision( &param );
}


void RemoteEditor::AddAttachment( AttachmentPtr attachment )
{
  m_Attachments.push_back( attachment );

  if(m_Enabled)
  {
    SendCreateAttachment(attachment);
  }
}

void AttachmentToRPC(RPC::AttachmentParam& param, AttachmentPtr attachment)
{
  param.m_MainEntityInstance   = (attachment->m_MainEntity ? attachment->m_MainEntity->m_ID : 0);
  param.m_AttachEntityInstance = (attachment->m_AttachedEntity ? attachment->m_AttachedEntity->m_ID : 0);
  param.m_Mode                 = attachment->m_Mode;
  param.m_MainJointId          = attachment->m_MainJointID;
  param.m_AttachJointId        = attachment->m_AttachedJointID;
}

void RemoteEditor::RemoveAttachment( AttachmentPtr attachment )
{
  V_Attachment::iterator itr = std::find( m_Attachments.begin(), m_Attachments.end(), attachment);
  if(itr != m_Attachments.end())
  {
    SendDeleteAttachment(attachment);
    m_Attachments.erase( itr );
  }
}

void RemoteEditor::SendCreateAttachment( AttachmentPtr attachment )
{
  RPC::AttachmentParam param;
  AttachmentToRPC(param, attachment);

  RuntimeConnection::GetRemoteLevelView()->CreateAttachment(&param);
}

void RemoteEditor::SendTweakAttachment( AttachmentPtr attachment )
{
  RPC::AttachmentParam param;
  AttachmentToRPC(param, attachment);

  RuntimeConnection::GetRemoteLevelView()->TweakAttachment(&param);
}

void RemoteEditor::SendDeleteAttachment( AttachmentPtr attachment )
{
  RPC::AttachmentParam param;
  AttachmentToRPC(param, attachment);

  RuntimeConnection::GetRemoteLevelView()->DeleteAttachment(&param);
}

