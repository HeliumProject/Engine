#include "Precompile.h"

#include "RemoteScene.h"

#include "BuilderUtil/ColorPalette.h"

#include "Live/LiveManager.h"

#include "IPC/TCP.h"
#include "IPC/Pipe.h"
#include "File/Manager.h"

#include "Core/Selection.h"

#include "Controller.h"
#include "Entity.h"
#include "EntityAssetSet.h"
#include "Light.h"
#include "DirectionalLight.h"
#include "ShadowDirection.h"
#include "AmbientLight.h"
#include "SunLight.h"
#include "SpotLight.h"
#include "PointLight.h"
#include "LightingVolume.h"
#include "Curve.h"
#include "Volume.h"
#include "Layer.h"

#include "LightScattering.h"

#include "SceneEditor.h"
#include "SceneManager.h"
#include "Transform.h"
#include "Editor/SessionManager.h"
#include "Live/RuntimeConnection.h"
#include "Task/Build.h"
#include "View.h"
#include "AssetEditor/LevelAsset.h"
#include "Asset/AssetClass.h"
#include "Asset/LevelAsset.h"
#include "Asset/FoliageAttribute.h"
#include "Asset/CollisionControlAttribute.h"
#include "Asset/Entity.h"
#include "Asset/WeatherAttribute.h"
#include "Attribute/AttributeHandle.h"
#include "Content/JointTransformsAttribute.h"
#include "Content/PostProcessingVolume.h"
#include "Content/LightingVolume.h"
#include "Content/Layer.h"
#include "Finder/OcclusionSpecs.h"
#include "Finder/UfragSpecs.h"
#include "Finder/TieSpecs.h"
#include "Finder/FoliageSpecs.h"
#include "Finder/LevelSpecs.h"
#include "Finder/ZoneSpecs.h"
#include "Finder/ShaderSpecs.h"
#include "NSL/NSL.h"
#include "IGSerializer/IGSerializer.h"

#include "igCore/igHeaders/ps3structs.h"
#include "igCore/igHeaders/FileChunkIDs.h"

#include "LightingJob/LightingJob.h"

#include "Dependencies/Dependencies.h"

#include "rpc/interfaces/rpc_physics.h"

#include "Content/SunShadowMergeAttribute.h"
#include "Content/GlossControlAttribute.h"
#include "Content/GroundLightAttribute.h"

#include "Content/PostEffectsLightScatteringAttribute.h"
#include "Content/PostEffectsColorCorrectionAttribute.h"
#include "Content/PostEffectsCurveControlAttribute.h"
#include "Content/PostEffectsDepthOfFieldAttribute.h"
#include "Content/PostEffectsGraphShaderAttribute.h"
#include "Content/PostEffectsMotionBlurAttribute.h"
#include "Content/PostEffectsFilmGrainAttribute.h"
#include "Content/PostEffectsCausticsAttribute.h"
#include "Content/PostEffectsCausticsAttribute.h"
#include "Content/PostEffectsColorAttribute.h"
#include "Content/PostEffectsBloomAttribute.h"
#include "Content/PostEffectsBlurAttribute.h"
#include "Content/PostEffectsFogAttribute.h"
#include "Content/PostEffectsHDRAttribute.h"

#include "Content/PostProcessingVolume.h"
#include "Asset/CustomMapsAttributes.h"

#include <algorithm>

#include "UIToolKit/DialogWorkerThread.h"
#include "UIToolKit/WorkerThreadDialog.h"

#include "BuilderUtil/GraphShaderBuildPostEffects.h"
#include "BuilderUtil/BakedLightUtil.h"
#include "BuilderUtil/BuilderUtil.h"
#include "BuilderUtil/Collision.h"

#include "TextureProcess/TextureWriter.h"
#include "Texture/Texture.h"

using namespace Luna;
using namespace IG;
using namespace Math;
using namespace RPC;

COMPILE_ASSERT(MAX_LINKED_LIGHTS_COUNT  == Content::LightingVolume::c_NUM_BAKED_LIGHTS);
COMPILE_ASSERT(HISTORGRAM_BIN_COUNT     == Content::PostEffectsHDRAttribute::c_HISTORGRAM_BIN_COUNT);

#define WeatherBlockingStr "Weather Blocker"

namespace Luna
{
  // this is sort of a hack.  we assume that any lighting volume in the world
  // file (root scene) is a global volume
  template <class V>
  inline bool GetIsGlobalVolume(V*  volume)
  {
    Luna::Scene* currentScene = volume->GetScene();
    return  currentScene->GetManager()->GetRootScene() == currentScene;
  }

  struct LunaViewTool : ILunaViewTool
  {
    RemoteScene* m_RemoteScene;
    SceneEditor* m_SceneEditor;

    LOCAL_IMPL(LunaViewTool, ILunaViewTool);

    void Init( RemoteScene* remoteScene )
    {
      m_RemoteScene = remoteScene;
      m_SceneEditor = remoteScene->GetSceneEditor();
    }

    virtual void TransformCamera(TransformCameraParam* param);
    virtual void RequestScene();

    virtual void UpdatePhysicsJoint(RPC::PhysicsJointParam* param);
    virtual void SelectJoint(RPC::SelectJointParam* param);
  };

  void LunaViewTool::TransformCamera( TransformCameraParam* param )
  {
    if ( m_RemoteScene->IsEnabled() && m_RemoteScene->IsCameraEnabled() )
    {
      Luna::View* view = m_SceneEditor->GetView();

      if ( view )
      {
        Math::Matrix4 mat;
        memcpy( &mat, &param->m_Transform, sizeof( param->m_Transform ) );

        // rotate by 180 to account for the engine
        Math::Matrix4 rotated = Math::Matrix4::RotateY( Math::Pi );

        rotated = rotated * mat;

        view->RemoteCameraMoved( rotated );
        view->Refresh();
      }
    }
  }

  void LunaViewTool::RequestScene()
  {
    if ( m_RemoteScene->IsEnabled() )
    {
      m_RemoteScene->SendScene();
    }
  }

  void LunaViewTool::UpdatePhysicsJoint(RPC::PhysicsJointParam* param)
  {
    //  PhysicsJointUpdateArgs args;
    //  args.m_Param = param;

    //g_PhysicsJointUpdateEvent.Raise(args);

  }

  void LunaViewTool::SelectJoint(RPC::SelectJointParam* param)
  {


  }

  struct PhysicsTool : public RPC::IPhysicsTool
  {
    RemoteScene* m_RemoteScene;
    SceneEditor* m_SceneEditor;

    LOCAL_IMPL(PhysicsTool, IPhysicsTool);

    void Init( RemoteScene* remoteScene )
    {
      m_RemoteScene = remoteScene;
      m_SceneEditor = remoteScene->GetSceneEditor();
    }

    void UpdateMobyData( u8* data, u32 size ) NOC_OVERRIDE;
  };

  // data comes in as any # of MobyTransformData, where each struct is followed by MobyTransformData::m_NumJoints
  // worth of MobyJointInfo
  void PhysicsTool::UpdateMobyData( u8* data, u32 size )
  {

    Undo::BatchCommandPtr batch = new Undo::BatchCommand ();

    MobyTransformData* transformData = (MobyTransformData*)data;
    while ( (u8*)transformData < data + size )
    {
      // explicitly swizzle, since this data comes in as payload
      Swizzle( transformData );

      tuid uid = transformData->m_ID;

      Luna::Entity* entity = NULL;

      // see if we can find this entity in one of the open scenes
      const M_SceneSmartPtr& scenes = m_SceneEditor->GetSceneManager()->GetScenes();
      M_SceneSmartPtr::const_iterator it = scenes.begin();
      M_SceneSmartPtr::const_iterator end = scenes.end();
      for ( ; it != end && entity == NULL; ++it )
      {
        const Luna::Scene* scene = it->second;

        entity = scene->Get< Luna::Entity >( uid );
      }

      if ( entity )
      {
        Math::Matrix4 transform;
        memcpy( &transform, &transformData->m_Transform, sizeof( transform ) );

        batch->Push( new Undo::PropertyCommand<Math::Matrix4> ( new Nocturnal::MemberProperty<Luna::Transform, Math::Matrix4> (entity, &Luna::Transform::GetGlobalTransform, &Luna::Transform::SetGlobalTransform), transform ) );

        Asset::EntityPtr entityPackage = entity->GetPackage<Asset::Entity>();

        Reflect::Registry* registry = Reflect::Registry::GetInstance();
        Content::JointTransformsAttributePtr jointTransforms = Reflect::AssertCast< Content::JointTransformsAttribute >( registry->CreateInstance( Reflect::GetClass<Content::JointTransformsAttribute>() ) );

        MobyJointInfo* jointInfo = (MobyJointInfo*)(((u8*)transformData) + sizeof( MobyTransformData ) );
        for ( u32 i = 0; i < transformData->m_NumJoints; ++i )
        {
          // explicitly swizzle, since this data comes in as payload
          Swizzle( jointInfo );

          tuid uid = jointInfo->m_ID;

          Math::Matrix4 transform;
          memcpy( &transform, &jointInfo->m_Transform, sizeof( transform ) );

          jointTransforms->m_Transforms[ uid ] = transform;
          jointInfo++;
        }

        entityPackage->SetJointTransformsAttribute( jointTransforms );
        batch->Push( new Undo::PropertyCommand< Content::JointTransformsAttributePtr >( new Nocturnal::MemberProperty<Asset::Entity, Content::JointTransformsAttributePtr> (entityPackage, &Asset::Entity::GetJointTransformsAttribute, &Asset::Entity::SetJointTransformsAttributeWithRaise) ) );
      }

      transformData = (MobyTransformData*)(((u8*)transformData) + sizeof( MobyTransformData ) + sizeof( MobyJointInfo ) * transformData->m_NumJoints );
    }

    if ( !batch->IsEmpty() )
    {
      m_SceneEditor->GetSceneManager()->GetCurrentScene()->Push( batch );
    }

    m_SceneEditor->GetView()->Refresh();

  }

}


RemoteScene::RemoteScene( Editor *editor )
: RemoteEditor( editor )
, m_SkyOcclId( 0 )
, m_SubsetScene( false )
, m_Camera( false )
, m_CollisionDbValid( false )
, m_Simulate( false )
, m_SyncLighting( true )
{
  m_SceneEditor = dynamic_cast< SceneEditor* >( editor );

  RPC::Host* host = RuntimeConnection::GetHost();

  // create a local implementation of our functionality
  m_LunaViewHost = RuntimeConnection::GetRemoteLevelView();
  m_LunaViewTool = new LunaViewTool( host );
  m_LunaViewTool->Init( this );

  m_PhysicsTool = new PhysicsTool( host );
  m_PhysicsTool->Init( this );

  m_PhysicsHost = new RPC::IPhysicsHostRemote( host );
}

RemoteScene::~RemoteScene()
{
  Enable( false );

  delete m_LunaViewTool;
  delete m_PhysicsHost;
  delete m_PhysicsTool;
}

void RemoteScene::AddSceneListeners( Luna::Scene* scene )
{
  scene->AddNodeAddedListener( NodeChangeSignature::Delegate ( this, &RemoteScene::SceneNodeCreated ) );
  scene->AddNodeRemovingListener( NodeChangeSignature::Delegate ( this, &RemoteScene::SceneNodeDeleting ) );
  scene->GetGraph()->AddEvaluatedListener( SceneGraphEvaluatedSignature::Delegate ( this, &RemoteScene::SceneGraphEvaluated ) );

  V_LightDumbPtr lights;
  scene->GetAll< Luna::Light >( lights );
  V_LightDumbPtr::const_iterator lightItr = lights.begin();
  V_LightDumbPtr::const_iterator lightEnd = lights.end();
  for ( ; lightItr != lightEnd; ++lightItr )
  {
    Luna::Light* light = *lightItr;
    light->AddChangedListener( LightChangeSignature::Delegate ( this, &RemoteScene::SetLightParams ) );
    light->AddRealtimeLightExistenceListener( RealtimeLightExistenceSignature::Delegate ( this, &RemoteScene::RealtimeLightExistenceChanged ) );
  }
}

void RemoteScene::RemoveSceneListeners( Luna::Scene* scene )
{
  scene->RemoveNodeAddedListener( NodeChangeSignature::Delegate ( this, &RemoteScene::SceneNodeCreated ) );
  scene->RemoveNodeRemovingListener( NodeChangeSignature::Delegate ( this, &RemoteScene::SceneNodeDeleting ) );
  scene->GetGraph()->RemoveEvaluatedListener( SceneGraphEvaluatedSignature::Delegate ( this, &RemoteScene::SceneGraphEvaluated ) );

  V_LightDumbPtr lights;
  scene->GetAll< Luna::Light >( lights );
  V_LightDumbPtr::const_iterator lightItr = lights.begin();
  V_LightDumbPtr::const_iterator lightEnd = lights.end();
  for ( ; lightItr != lightEnd; ++lightItr )
  {
    Luna::Light* light = *lightItr;
    light->RemoveChangedListener( LightChangeSignature::Delegate ( this, &RemoteScene::SetLightParams ) );
    light->RemoveRealtimeLightExistenceListener( RealtimeLightExistenceSignature::Delegate ( this, &RemoteScene::RealtimeLightExistenceChanged ) );
  }
}

void RemoteScene::ResetScene()
{
  ClearScene();

  m_SubsetScene = false;

  Luna::SceneManager* manager = m_SceneEditor->GetSceneManager();

  if ( manager->GetRootScene() )
  {
    AddScene( manager->GetRootScene() );

    M_SceneSmartPtr::const_iterator itr = manager->GetScenes().begin();
    M_SceneSmartPtr::const_iterator end = manager->GetScenes().end();
    for ( ; itr != end; ++itr )
    {
      if ( manager->GetRootScene() != itr->second )
      {
        AddScene( itr->second );
      }
    }

    SendLevelData();

    if ( m_CollisionDbValid )
    {
      LoadCollisionDbParam param;
      m_LunaViewHost->LoadCollisionDb( &param );
    }

    SendSimulate();
    SendSimulationData();
  }
}

void RemoteScene::Enable( bool enable )
{
  Enable( enable, false );
}

void RemoteScene::Enable( bool enable, bool subset )
{
  if ( enable == m_Enabled )
  {
    return;
  }

  m_Enabled = enable;
  RPC::Host* host = RuntimeConnection::GetHost();

  if ( enable )
  {
    m_SubsetScene = subset;

    host->SetLocalInterface( kLunaViewTool, m_LunaViewTool );
    host->SetLocalInterface( kPhysicsTool, m_PhysicsTool );
    m_LunaViewHost->Reset();

    Luna::SceneManager* manager = m_SceneEditor->GetSceneManager();
    manager->AddSceneAddedListener( SceneChangeSignature::Delegate ( this, &RemoteScene::SceneAdded ) );
    manager->AddSceneRemovingListener( SceneChangeSignature::Delegate ( this, &RemoteScene::SceneRemoved ) );

    Luna::View* view = m_SceneEditor->GetView();

    if ( view )
    {
      view->AddCameraMovedListener( CameraMovedSignature::Delegate ( this, &RemoteScene::CameraMoved ) );
    }

    if ( manager->GetRootScene() )
    {
      AddSceneListeners( manager->GetRootScene() );
    }

    // iterate over every scene, adding listeners
    const M_SceneSmartPtr& scenes = manager->GetScenes();
    for each ( const M_SceneSmartPtr::value_type& val in scenes )
    {
      ScenePtr scene = val.second;

      if ( scene.Ptr() != manager->GetRootScene() )
      {
        AddSceneListeners( scene );
      }
    }

    SetMode( RPC::LunaViewModes::Normal );
  }
  else
  {
    Luna::SceneManager* manager = m_SceneEditor->GetSceneManager();
    manager->RemoveSceneAddedListener( SceneChangeSignature::Delegate ( this, &RemoteScene::SceneAdded ) );
    manager->RemoveSceneRemovingListener( SceneChangeSignature::Delegate ( this, &RemoteScene::SceneRemoved ) );

    Luna::View* view = m_SceneEditor->GetView();

    if ( view )
    {
      view->RemoveCameraMovedListener( CameraMovedSignature::Delegate ( this, &RemoteScene::CameraMoved ) );
    }

    // iterate over every scene, removing listeners
    const M_SceneSmartPtr& scenes = manager->GetScenes();

    if ( manager->GetRootScene() )
    {
      RemoveSceneListeners( manager->GetRootScene() );
    }

    for each ( const M_SceneSmartPtr::value_type& val in scenes )
    {
      const ScenePtr& scene = val.second;

      if ( scene.ReferencesObject() && (Luna::Scene*)scene != manager->GetRootScene() )
      {
        RemoveSceneListeners( scene );
      }
    }

    ClearScene();
  }

  LoadOcclusion();
}

void RemoteScene::EnableCamera( bool enable )
{
  m_Camera = enable;

  if ( enable )
  {
    TransformCamera();
  }
}

void RemoteScene::EnablePhysics( bool enable )
{
  m_Simulate = enable;

  if ( enable )
  {
    if ( !m_CollisionDbValid )
    {
      BuildCollisionDb();
    }
  }

  SendSimulate();
}

void RemoteScene::SendSimulate()
{
  if ( m_Simulate )
  {
    m_PhysicsHost->BeginSimulation();
  }
  else
  {
    m_PhysicsHost->EndSimulation();
  }
}

void RemoteScene::SendSimulationData()
{
  u32 size = (u32)m_SimulatedMobys.size() * sizeof( tuid );

  if ( size > 0 )
  {
    u8* data = new u8[ size ];
    u8* currentUID = data;
    for each ( const UniqueID::TUID& uid in m_SimulatedMobys )
    {
      memcpy( currentUID, &uid, sizeof( uid ) );

      currentUID += sizeof( uid );
    }

    m_PhysicsHost->AddMobysToSim( data, size );
  }
}

void RemoteScene::CreateUIDArray( const V_EntityDumbPtr& entities, u8*& uids, u32& size )
{
  NOC_ASSERT(entities.size() > 0);

  size = (u32)entities.size() * sizeof( tuid );
  uids = new u8[ size ];
  memset( uids, 0, size );
  tuid* currentUID = (tuid*)uids;
  for each ( const Luna::Entity* entity in entities )
  {
    *currentUID = ConvertEndian( entity->GetID(), true );
    ++currentUID;
  }
}

void RemoteScene::AddMobysToSim( const V_EntityDumbPtr& mobys )
{
  for each ( const Luna::Entity* entity in mobys )
  {
    m_SimulatedMobys.insert( entity->GetID() );
  }

  if(mobys.size() > 0)
  {
    u32 size;
    u8* uids;

    CreateUIDArray( mobys, uids, size );
    m_PhysicsHost->AddMobysToSim( uids, size );
  }
}

void RemoteScene::RemoveMobysFromSim( const V_EntityDumbPtr& mobys )
{
  for each ( const Luna::Entity* entity in mobys )
  {
    m_SimulatedMobys.erase( entity->GetID() );
  }

  if(mobys.size() > 0)
  {
    u32 size;
    u8* uids;

    CreateUIDArray( mobys, uids, size );
    m_PhysicsHost->RemoveMobysFromSim( uids, size );
  }
}

void RemoteScene::SyncMobys( const V_EntityDumbPtr& mobys )
{
  if(mobys.size() > 0)
  {
    u32 size;
    u8* uids;

    CreateUIDArray( mobys, uids, size );
    m_PhysicsHost->RequestMobyData( uids, size );
  }
}

const UniqueID::S_TUID& RemoteScene::GetSimulatedMobys()
{
  return m_SimulatedMobys;
}

void RemoteScene::CameraMoved( const CameraMovedArgs& args )
{
  TransformCamera();
}

void RemoteScene::TransformCamera()
{
  if(!m_Enabled || !m_Camera || !RuntimeConnection::IsConnected())
  {
    return;
  }

  Luna::View* view = m_SceneEditor->GetView();

  if ( view )
  {
    TransformCamera( view->GetCamera()->GetInverseView() );
  }
}

void RemoteScene::ViewAsset( tuid assetId )
{
  m_SubsetScene = true;
  m_Camera = false;

  __super::ViewAsset( assetId );
}

////////////////////////////////////////////////////////////////////////////////////////////////
void      ConvertBakedLightEndianess(IGPS3::BakedLight* bl)
{
  //Position
  bl->m_pos.x       = ConvertEndian(bl->m_pos.x);
  bl->m_pos.y       = ConvertEndian(bl->m_pos.y);
  bl->m_pos.z       = ConvertEndian(bl->m_pos.z);
  bl->m_pos.w       = ConvertEndian(bl->m_pos.w);

  //Direction
  bl->m_dir.x       = ConvertEndian(bl->m_dir.x);
  bl->m_dir.y       = ConvertEndian(bl->m_dir.y);
  bl->m_dir.z       = ConvertEndian(bl->m_dir.z);
  bl->m_dir.w       = ConvertEndian(bl->m_dir.w);

  //Color
  bl->m_color       = ConvertEndian(bl->m_color);
  bl->m_hdr         = ConvertEndian(bl->m_hdr);

  //Flags
  bl->m_flags       = ConvertEndian(bl->m_flags);

  //Cone
  bl->m_cone_phi    = ConvertEndian(bl->m_cone_phi);
  bl->m_cone_theta  = ConvertEndian(bl->m_cone_theta);

  //User stuff
  bl->m_user1       = ConvertEndian(bl->m_user1);
  bl->m_user2       = ConvertEndian(bl->m_user2);
  bl->m_user3       = ConvertEndian(bl->m_user3);
}

////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
inline T* ConvertEndian(void* ptr)
{
  union PtrU32
  {
    void* m_ptr;
    u32   m_u32;
  } value;

  value.m_ptr = ptr;

  {
    _asm mov eax,value.m_u32
    _asm bswap eax
    _asm mov value.m_u32,eax
  }

  return (T*)value.m_ptr;
}

/*
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GenerateBakedLightsDB(Luna::Scene* scene, ILunaViewHost* lHost)
{
  //Right now only process the root zone
  if(scene->GetManager()->GetRootScene() != scene)
  {
    return;
  }

  //Collect the lighting volumes as well as all other lights
  V_LightingVolumeDumbPtr lightingVolumes;
  V_LightDumbPtr          lights;

  scene->GetAll< Luna::LightingVolume >( lightingVolumes );
  scene->GetAll< Luna::Light >( lights );

  {
    Luna::V_LayerDumbPtr  layers;
    IG::IGSerializer      dbSerializer;
    Content::Scene        bakedLightsScene;

    //Fetch the layers
    scene->GetAll<Luna::Layer>(layers);

    //Add the lighting volumes
    for each ( const LightingVolumePtr lightingVolume in lightingVolumes )
    {
      if ( !lightingVolume->IsTransient() )
      {
        Content::LightingVolume* cLightingVolume  = lightingVolume->GetPackage<Content::LightingVolume>();
        bakedLightsScene.Add(cLightingVolume);
      }
    }

    //Add the lights
    for each ( const LightPtr light in lights )
    {
      if ( !light->IsTransient() )
      {
        Content::Light* cLight  = light->GetPackage<Content::Light>();
        bakedLightsScene.Add(cLight);
      }
    }

    //Add the layers
    for each ( const LayerPtr layer in layers )
    {
      Content::Layer* cLayer  = layer->GetPackage<Content::Layer>();
      if(cLayer->m_Type == Content::LayerTypes::LT_Lighting)
      {
        bakedLightsScene.Add(cLayer);
      }
    }

    //Fetch the first global lighting volume
    Content::LightingVolume*  globalVolume =  lightingVolumes.empty() ? NULL :
                                              lightingVolumes[0]->GetPackage<Content::LightingVolume>();

    //Generate the global baked lights db
    BuilderUtil::ProcessGlobalLights(globalVolume, bakedLightsScene, dbSerializer);

    //Stream the data to the devkit
    {
      Nocturnal::BasicBufferPtr dbBufferPtr  = dbSerializer.GetChunk(IGG::FILECHUNK_LEVEL_BAKED_LIGHT_DB, 0);
      Nocturnal::BasicBufferPtr dbContentPtr = dbSerializer.GetChunk(IGG::FILECHUNK_LEVEL_BAKED_LIGHT_DB_CONTENT, 0);
      u32                       dataSize     = dbBufferPtr->GetSize() + dbContentPtr->GetSize();

      //Sanity check
      NOC_ASSERT(dbBufferPtr->GetSize() == sizeof(IGPS3::BakedLightDB));

      RPC::BakedLightDatabaseParam  param;
      u8*                           data     = new u8[dataSize];

      //Global
      param.m_ZoneID  = 0;

      //Copy the data-base
      memcpy(data, dbBufferPtr->GetData(), sizeof(IGPS3::BakedLightDB));
      IGPS3::BakedLightDB*  db           = (IGPS3::BakedLightDB*)data;

      //Sanity check
      NOC_ASSERT(dbContentPtr->GetSize() == db->m_num_lights*sizeof(IGPS3::BakedLight));

      //Copy the data-base content
      memcpy((data + sizeof(IGPS3::BakedLightDB)), dbContentPtr->GetData(), dbContentPtr->GetSize());

      IGPS3::BakedLight*    bakedLights  = (IGPS3::BakedLight*)(data + sizeof(IGPS3::BakedLightDB));

      //Swizzle the baked lights
      for(u32 idx = 0; idx < db->m_num_lights; ++idx)
      {
        ConvertBakedLightEndianess(bakedLights + idx);
      }

      //Swizzle the db
      db->m_root      = ConvertEndian<IGPS3::Node>(db->m_root);
      db->m_indices   = ConvertEndian<u16>(db->m_indices );
      db->m_bspheres  = ConvertEndian<vec4f>(db->m_bspheres);
      db->m_lights    = ConvertEndian<IGPS3::BakedLight>(db->m_lights);
      db->m_mailbox   = ConvertEndian<u16>(db->m_mailbox );
      db->m_num_lights= ConvertEndian(db->m_num_lights);

      lHost->SetBakedLightDatabaseParameters(&param, data, dataSize);

      delete[] data;
    }
  }
}*/

void RemoteScene::AddScene( Luna::Scene* scene )
{
  //Entities
  {
    V_EntityDumbPtr entities;
    scene->GetAll< Luna::Entity >( entities );

    for each ( const EntityPtr entity in entities )
    {
      if ( !entity->IsTransient() )
      {
        AddEntityInstance( entity );
      }
    }
  }

  //Volumes
  {
    V_VolumeDumbPtr volumes;
    scene->GetAll< Luna::Volume >( volumes );

    for each ( const VolumePtr volume in volumes )
    {
      if ( !volume->IsTransient() )
      {
        AddVolume( volume );
      }
    }
  }

  //Lights
  {
    V_LightDumbPtr lights;
    scene->GetAll< Luna::Light >( lights );

    for each ( const LightPtr light in lights )
    {
      if ( !light->IsTransient() )
      {
        AddLight( light );
      }
    }
  }

  //Lighting volumes
  {
    V_LightingVolumeDumbPtr lightingVolumes;
    scene->GetAll< Luna::LightingVolume >( lightingVolumes );

    for each ( const LightingVolumePtr lightingVolume in lightingVolumes )
    {
      if ( !lightingVolume->IsTransient() )
      {
        AddLightingVolume( lightingVolume );
      }
    }
  }

  //Post-processing volumes
  {
    V_PostProcessingVolumeDumbPtr postprocVolumes;
    scene->GetAll< PostProcessingVolume >( postprocVolumes );

    for each ( const PostProcessingVolumePtr postprocVolume in postprocVolumes )
    {
      if ( !postprocVolume->IsTransient() )
      {
        AddPostProcessingVolume( postprocVolume );
      }
    }
  }
}


void RemoteScene::LoadLightingData( RPC::LoadLightingDataParam& param )
{
  if(!m_Enabled || !RuntimeConnection::IsConnected() )
  {
    return;
  }

  RPC::ILunaViewHostRemote* lunaView = RuntimeConnection::GetRemoteLevelView();

  if(!lunaView )
  {
    return;
  }

  {
    lunaView->LoadLightingData( &param );
  }


}

void RemoteScene::RemoveScene( Luna::Scene* scene )
{
  //Entities
  {
    V_EntityDumbPtr entities;
    scene->GetAll< Luna::Entity>( entities );

    for each ( const EntityPtr entity in entities )
    {
      if ( !entity->IsTransient() )
      {
        RemoveEntityInstance( entity );
      }
    }
  }

  //Volumes
  {
    V_VolumeDumbPtr volumes;
    scene->GetAll< Luna::Volume >( volumes );

    for each ( const VolumePtr volume in volumes )
    {
      if ( !volume->IsTransient() )
      {
        RemoveVolume( volume );
      }
    }
  }

  //Lights
  {
    V_LightDumbPtr lights;
    scene->GetAll< Luna::Light >( lights );

    for each ( const LightPtr light in lights )
    {
      if ( !light->IsTransient() )
      {
        RemoveLight( light );
      }
    }
  }

  //Lighting volumes
  {
    V_LightingVolumeDumbPtr lightingVolumes;
    scene->GetAll< Luna::LightingVolume >( lightingVolumes );

    for each ( const LightingVolumePtr lightingVolume in lightingVolumes )
    {
      if ( !lightingVolume->IsTransient() )
      {
        RemoveLightingVolume( lightingVolume );
      }
    }
  }

  //PostProcessing
  {
    V_PostProcessingVolumeDumbPtr postprocessingVolumes;
    scene->GetAll< PostProcessingVolume >( postprocessingVolumes );

    for each ( const PostProcessingVolumePtr postprocessingVolume in postprocessingVolumes )
    {
      if ( !postprocessingVolume->IsTransient() )
      {
        RemovePostProcessingVolume( postprocessingVolume );
      }
    }
  }
}

void RemoteScene::SceneGraphEvaluated( const Luna::SceneGraphEvaluatedArgs& args )
{
  for each ( Luna::SceneNode* node in args.m_Nodes )
  {
    Luna::Transform* transform = Reflect::ObjectCast< Luna::Transform >( node );

    if ( transform )
    {
      Content::Transform* contentTransform = transform->GetPackage< Content::Transform >();
      bool changed = true;
      if ( contentTransform )
      {
        changed = contentTransform->m_GlobalTransform != transform->GetGlobalTransform() ||
          contentTransform->HasType( Reflect::GetType< Content::Curve >() );
      }

      if ( changed )
      {
        TransformInstance( transform );
      }
    }
  }
}

void RemoteScene::SceneLoaded( const Luna::LoadArgs& args )
{
  if( !m_Enabled || m_SubsetScene )
  {
    return;
  }

  // only send the sky if we have opened the root scene
  // ie. not every time we open a zone file..
  //
  if(m_SceneEditor->GetSceneManager()->GetRootScene() == args.m_Scene)
  {
    Asset::LevelAssetPtr level = m_SceneEditor->GetSceneManager()->GetCurrentLevel();
    if(level)
    {
      SetLevelWeatherAttributes(level, Luna::WEATHER_OP_TEXTURES2D_RELOAD);
      SetLevelAttributes(level);
      LoadOcclusion();
    }
    else
    {
      ClearLevelAttributes();
    }
  }

  AddScene( args.m_Scene );
  AddSceneListeners( args.m_Scene );

  args.m_Scene->RemoveLoadFinishedListener( LoadSignature::Delegate ( this, &RemoteScene::SceneLoaded ) );
}

void RemoteScene::SceneAdded( const Luna::SceneChangeArgs& args )
{
  if( !m_Enabled || m_SubsetScene )
  {
    return;
  }

  args.m_Scene->AddLoadFinishedListener( LoadSignature::Delegate ( this, &RemoteScene::SceneLoaded ) );
}

void RemoteScene::SceneRemoved( const Luna::SceneChangeArgs& args )
{
  if( !m_Enabled || m_SubsetScene )
  {
    return;
  }

  RemoveSceneListeners( args.m_Scene );
  RemoveScene( args.m_Scene );

  // clear out the sky if we have unloaded the root scene
  if(m_SceneEditor->GetSceneManager()->GetRootScene() == args.m_Scene)
  {
    ClearLevelAttributes();
  }
}

///////////////////////////////////////////////////////////////////////////////////////////
void RemoteScene::TransformInstance( Luna::Transform* transform )
{
  if( !m_Enabled )
  {
    return;
  }

  Content::TransformPtr& contentTransform = m_Scene.Get< Content::Transform >( transform->GetID() );
  if ( contentTransform )
  {
    contentTransform->m_GlobalTransform = transform->GetGlobalTransform();
  }

  // find the engine type of this instance
  if ( Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( transform ) )
  {
    Asset::EntityAsset* entityClass = entity->GetClassSet()->GetEntityAsset();

    if ( entityClass )
    {
      TransformInstanceParam param;
      param.m_EngineType = GetAssetType( entityClass );

      switch ( param.m_EngineType )
      {
      case RPC::EngineTypes::Tie:
      case RPC::EngineTypes::Ufrag:
        SetCollisionDbValid( false );
        break;
      }

      // only send the command if this instance is a type we support being transformed
      if ( param.m_EngineType != (u8)-1 )
      {
        param.m_ID = transform->GetID();
        memcpy( &param.m_Transform, &transform->GetGlobalTransform(), sizeof( param.m_Transform ) );

        m_LunaViewHost->TransformInstance( &param );

        // if we have a foliage attribute, attempt to transform the foliage as well
        // we assume that the foliage has the same uid as the tie
        Attribute::AttributeViewer< Asset::FoliageAttribute > foliage( entityClass );
        if ( foliage.Valid() )
        {
          param.m_EngineType = RPC::EngineTypes::Foliage;
          m_LunaViewHost->TransformInstance( &param );
        }
      }
    }
  }

  // gameplay controller
  if ( Luna::Controller* controller = Reflect::ObjectCast< Luna::Controller >( transform ) )
  {
    TransformInstanceParam param;
    param.m_EngineType = RPC::EngineTypes::Controller;

    param.m_ID = transform->GetID();
    memcpy( &param.m_Transform, &transform->GetGlobalTransform(), sizeof( param.m_Transform ) );

    m_LunaViewHost->TransformInstance( &param );
  }

  // gameplay volume
  else if ( Luna::Volume* volume = Reflect::ObjectCast< Luna::Volume >( transform ) )
  {
    TransformVolumeParam param;

    param.m_ID = transform->GetID();
    memcpy( &param.m_Transform, &transform->GetGlobalTransform(), sizeof( param.m_Transform ) );

    if(volume->GetIsWeatherBlocker())
    {
      bool globalVolume = GetIsGlobalVolume(transform);

      // we don't transform the global volume (there isn't one)
      if ( !globalVolume )
      {
        param.m_Type  = VolumeTypes::WeatherBlocking;
        m_LunaViewHost->TransformVolume( &param );
      }
    }
    else
    {
      const Content::Volume* volumeData = volume->GetPackage<Content::Volume>();

      param.m_Type = (u8)volumeData->m_Shape;

      m_LunaViewHost->TransformVolume( &param );
    }
  }

  // gameplay curve
  else if ( Luna::Curve* curve = Reflect::ObjectCast< Luna::Curve >( transform ) )
  {
    const Content::Curve* curveData = curve->GetPackage<Content::Curve>();

    //set the number of points in this path
    const Math::V_Vector3& points = curveData->m_Points;

    f32 length = curve->CalculateCurveLength();
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
        previousPoint = &(*itr);
        Math::Vector3 worldPnt = (*itr);
        Math::Matrix4 globalTransform;
        globalTransform = curve->GetGlobalTransform();
        globalTransform.TransformVertex( worldPnt );
        memcpy( &msg_points[ i ], &worldPnt, sizeof( Math::Vector3 ) );
        ConvertEndian( msg_points[i], msg_points[i], true );
      }
    }

    TransformCurveParam param;

    param.m_ID = transform->GetID();
    param.m_Length = length;

    m_LunaViewHost->TransformCurve( &param, (u8*)msg_points, static_cast< u32 >( num_points * sizeof( IG::vec3 ) ) );

    delete [] msg_points;
  }
  // if it's not an entity, it may be a light
  else if ( Luna::Light* light = Reflect::ObjectCast< Luna::Light >( transform ) )
  {
    light->Pack();

    if(Reflect::ObjectCast< Luna::SunLight >( transform )           ||
       Reflect::ObjectCast< Luna::AmbientLight >( transform )       ||
       Reflect::ObjectCast< Luna::ShadowDirection >( transform )    ||
       Reflect::ObjectCast< Luna::DirectionalLight >( transform ))
    {
      SetLinkedBakedLights(light);
    }
    else
    {
      TransformLight( light->GetPackage<Content::Light>() );
    }
  }
  else if( transform->HasType( Reflect::GetType<Luna::LightingVolume>() ) )
  {
    bool globalVolume = GetIsGlobalVolume(transform);

    // we don't transform the global volume
    if ( !globalVolume )
    {
      TransformVolumeParam param;

      param.m_ID    = transform->GetID();
      param.m_Type  = VolumeTypes::Lighting;

      memcpy( &param.m_Transform, &transform->GetGlobalTransform(), sizeof( param.m_Transform ) );

      m_LunaViewHost->TransformVolume( &param );
    }
  }
  else if( transform->HasType( Reflect::GetType<PostProcessingVolume>() ) )
  {
    bool globalVolume = GetIsGlobalVolume(transform);

    // we don't transform the global volume
    if ( !globalVolume )
    {
      TransformVolumeParam param;

      param.m_ID    = transform->GetID();
      param.m_Type  = VolumeTypes::PostProcessing;

      memcpy( &param.m_Transform, &transform->GetGlobalTransform(), sizeof( param.m_Transform ) );

      m_LunaViewHost->TransformVolume( &param );
    }
  }
  else if( transform->HasType( Reflect::GetType<LightScattering>() ) )
  {
    Luna::PostProcessingVolumePtr volume = Reflect::ObjectCast<Luna::PostProcessingVolume>(transform->GetParent());
    if(volume)
    {
      volume->LightScatteringSettingsChanging();
    }
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

//////////////////////////////////////////////////////////////////////////////////////////////////
bool RemoteScene::GetLightsLinkedToLightingVolume(Luna::Instance* lightVolume,  OS_SelectableDumbPtr& lights)
{
  Luna::Scene*  lunaScene = lightVolume->GetScene();

  lights.Clear();

  if(lunaScene != NULL)
  {
    const HM_StrToSceneNodeTypeSmartPtr&          nodes       = lunaScene->GetNodeTypesByName();
    HM_StrToSceneNodeTypeSmartPtr::const_iterator nodeTypeItr = nodes.begin();
    HM_StrToSceneNodeTypeSmartPtr::const_iterator nodeTypeEnd = nodes.end();

    for ( ; nodeTypeItr != nodeTypeEnd; ++nodeTypeItr )
    {
      const SceneNodeTypePtr& nodeType = nodeTypeItr->second;
      if ( Reflect::Registry::GetInstance()->GetClass( nodeType->GetInstanceType() )->HasType( Reflect::GetType<Luna::Layer>() ) )
      {
        HM_SceneNodeSmartPtr::const_iterator instItr = nodeTypeItr->second->GetInstances().begin();
        HM_SceneNodeSmartPtr::const_iterator instEnd = nodeTypeItr->second->GetInstances().end();

        //Begin batching
        for ( ; instItr != instEnd; ++instItr )
        {
          const SceneNodePtr& dependNode    = instItr->second;
          Luna::Layer*        lunaLayer     = Reflect::AssertCast< Luna::Layer >( dependNode );
          Content::Layer*     contentLayer  = lunaLayer->GetPackage<Content::Layer>();
 
          if(contentLayer->m_Type == Content::LayerTypes::LT_Lighting)
          { 
            OS_SelectableDumbPtr            layerMembers  = lunaLayer->GetMembers();

            OS_SelectableDumbPtr::Iterator  memberItr     = layerMembers.Begin();
            OS_SelectableDumbPtr::Iterator  memberEnd     = layerMembers.End();

            for ( ; memberItr != memberEnd; ++memberItr )
            {
              Selectable* member = (*memberItr);

              //Check for the supported types
              if(LightingLayerGrid::IsLightLinkableToVolume(member) )
              {
                lights.Append(member);
              }
            }//Done with the members
          }
        }
      } 
    }
  }

  return (lights.Size() != 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
bool RemoteScene::GetLightingVolumesLinkedToLight(Luna::Instance* light,        OS_SelectableDumbPtr& lightingVolumes)
{
  Luna::Scene*  lunaScene = light->GetScene();
  lightingVolumes.Clear();

  if(lunaScene != NULL)
  {
    const HM_StrToSceneNodeTypeSmartPtr&          nodes       = lunaScene->GetNodeTypesByName();
    HM_StrToSceneNodeTypeSmartPtr::const_iterator nodeTypeItr = nodes.begin();
    HM_StrToSceneNodeTypeSmartPtr::const_iterator nodeTypeEnd = nodes.end();

    for ( ; nodeTypeItr != nodeTypeEnd; ++nodeTypeItr )
    {
      const SceneNodeTypePtr& nodeType = nodeTypeItr->second;
      if ( Reflect::Registry::GetInstance()->GetClass( nodeType->GetInstanceType() )->HasType( Reflect::GetType<Luna::Layer>() ) )
      {
        HM_SceneNodeSmartPtr::const_iterator instItr = nodeTypeItr->second->GetInstances().begin();
        HM_SceneNodeSmartPtr::const_iterator instEnd = nodeTypeItr->second->GetInstances().end();

        //Begin batching
        for ( ; instItr != instEnd; ++instItr )
        {
          const SceneNodePtr& dependNode    = instItr->second;
          Luna::Layer*        lunaLayer     = Reflect::AssertCast< Luna::Layer >( dependNode );
          Content::Layer*     contentLayer  = lunaLayer->GetPackage<Content::Layer>();
 
          if(contentLayer->m_Type == Content::LayerTypes::LT_Lighting)
          { 
            OS_SelectableDumbPtr            layerMembers  = lunaLayer->GetMembers();

            OS_SelectableDumbPtr::Iterator  memberItr     = layerMembers.Begin();
            OS_SelectableDumbPtr::Iterator  memberEnd     = layerMembers.End();

            for ( ; memberItr != memberEnd; ++memberItr )
            {
              Selectable* member = (*memberItr);
              if(Reflect::ObjectCast< Luna::LightingVolume >( member ) != NULL)
              {
                lightingVolumes.Append(member);
              }
            }//Done with the members
          }
        }
      } 
    }
  }

  return (lightingVolumes.Size() != 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void RemoteScene::SetLinkedBakedLights( Luna::Instance* light )
{
  //Global volumes only at the moment
  if(GetIsGlobalVolume(light))
  {
    Luna::V_LightingVolumeDumbPtr   worldLightingVolumes;
    RPC::LinkedBakedLightsParam     param;

    light->GetScene()->GetAll< Luna::LightingVolume >( worldLightingVolumes );
    ZeroMemory(&param, sizeof(LinkedBakedLightsParam));

    if( worldLightingVolumes.empty() == false )
    {
      if( worldLightingVolumes.size() > 1 )
      {
        Console::Warning( "More than one global lighting volume in the world! Picking the first one!\n" );
      }

      Luna::LightingVolume* lightingVolume  = (*worldLightingVolumes.begin());
      OS_SelectableDumbPtr  lightsToUpdate;

      if(GetLightsLinkedToLightingVolume(lightingVolume, lightsToUpdate))
      {
        OS_SelectableDumbPtr::Iterator  memberItr     = lightsToUpdate.Begin();
        OS_SelectableDumbPtr::Iterator  memberEnd     = lightsToUpdate.End();

        for ( ; (memberItr != memberEnd) && (param.m_Count < MAX_LINKED_LIGHTS_COUNT); ++memberItr )
        {
          Selectable*     member  = (*memberItr);
          Luna::Light*    lLight  = Reflect::ObjectCast< Luna::Light >( member );
          Content::Light* cLight  = lLight->GetPackage<Content::Light>();

          //Sun Light
          if(Luna::ShadowDirection* shadowDir = Reflect::ObjectCast< Luna::ShadowDirection >( member ))
          {
            param.m_ShadowDirection.x = -cLight->m_GlobalTransform.z.x;
            param.m_ShadowDirection.y = -cLight->m_GlobalTransform.z.y;
            param.m_ShadowDirection.z = -cLight->m_GlobalTransform.z.z;
            continue;
          }

          //Choose the light to highlight in the engine
          if(lLight == light)
          {
            param.m_ChangedIndex      = param.m_Count;
          }

          //Directional Light
          if(Reflect::ObjectCast< Luna::DirectionalLight >( member ))
          {
            Content::DirectionalLight*  cDirLight   = lLight->GetPackage<Content::DirectionalLight>();
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

          //Ambient Light
          if(Reflect::ObjectCast< Luna::AmbientLight >( member ))
          {
            u32 idx                   = param.m_Count++;
            param.m_Directions[idx].x = 1.0f;
            param.m_Colors[idx]       = ConvertSrgbToSquared(cLight->m_Color.r, cLight->m_Color.g, cLight->m_Color.b);
            param.m_HDR[idx]          = cLight->m_Color.s;
            param.m_Flags[idx]        = IGPS3::IG_BAKED_AMB;
            continue;
          }
        }//Done
      }
    }
    //Send the info
    m_LunaViewHost->SetLinkedBakedLights( &param );
  }
  else
  {
    //Check if this is a Shadow direction
    if(Reflect::ObjectCast< Luna::ShadowDirection >( light ) != NULL)
    {
      //if it is, update the lighting volumes linked to it
      OS_SelectableDumbPtr  lightingVolumesToUpdate;

      if(GetLightingVolumesLinkedToLight(light, lightingVolumesToUpdate))
      {
        OS_SelectableDumbPtr::Iterator  memberItr     = lightingVolumesToUpdate.Begin();
        OS_SelectableDumbPtr::Iterator  memberEnd     = lightingVolumesToUpdate.End();

        for ( ; (memberItr != memberEnd); ++memberItr )
        {
          Selectable*            member       = (*memberItr);
          Luna::LightingVolume*  lightingVol  = Reflect::ObjectCast< Luna::LightingVolume >( member );

          SetLightingVolumeParams(lightingVol);
        }
      }
    }
  }
}

void RemoteScene::TransformCamera( const Math::Matrix4& transform )
{
  if( !m_Enabled )
  {
    return;
  }

  if(!m_Camera)
  {
    return;
  }

  // rotate by 180 for the engine
  Math::Matrix4 rotated = Math::Matrix4::RotateY( Math::Pi );

  rotated = rotated * transform;

  RPC::TransformCameraParam param;
  memcpy( &param.m_Transform, &rotated, sizeof( param.m_Transform ) );

  m_LunaViewHost->TransformCamera( &param );
}

void RemoteScene::SetLightParams( const LightChangeArgs& args )
{
  if( !m_Enabled )
  {
    return;
  }

  Luna::Light* light = args.m_Light;

  light->Pack();

  if(Reflect::ObjectCast< Luna::SunLight >( light )           ||
     Reflect::ObjectCast< Luna::AmbientLight >( light )       ||
     Reflect::ObjectCast< Luna::ShadowDirection >( light )    ||
     Reflect::ObjectCast< Luna::DirectionalLight >( light ))
  {
    SetLinkedBakedLights(light);
    return;
  }

  RemoteEditor::SetLightParams( light->GetPackage<Content::Light>() );
}

void RemoteScene::RealtimeLightExistenceChanged( const RealtimeLightExistenceArgs& args )
{
  if ( m_SubsetScene )
  {
    if ( !m_Scene.Get< Reflect::Element >( args.m_Light->GetID() ) )
    {
      return;
    }
  }

  if ( args.m_Added )
  {
    AddLight( args.m_Light );
  }
  else
  {
    RemoveLight( args.m_Light );
  }
}

void RemoteScene::AddLight( Luna::Light* light )
{
  if( !m_Enabled )
  {
    return;
  }
  light->Pack();

  if ( light->GetRenderType() == Content::LightRenderTypes::Baked )
  {
    //Non-positional linked lights
    if(Reflect::ObjectCast< Luna::SunLight >( light )           ||
       Reflect::ObjectCast< Luna::AmbientLight >( light )       ||
       Reflect::ObjectCast< Luna::ShadowDirection >( light )    ||
       Reflect::ObjectCast< Luna::DirectionalLight >( light ))
    {
      SetLinkedBakedLights(light);
    }

    return;
  }

  RemoteEditor::AddLight( light->GetPackage< Content::Light >() );

  TransformInstance( light );
  SetLightParams( LightChangeArgs( light ) );
}

void RemoteScene::RemoveLight( Luna::Light* light )
{
  if( !m_Enabled )
  {
    return;
  }
  light->Pack();

  //Non-positional linked lights
  if( Reflect::ObjectCast< Luna::SunLight >( light )           ||
      Reflect::ObjectCast< Luna::AmbientLight >( light )       ||
      Reflect::ObjectCast< Luna::ShadowDirection >( light )    ||
      Reflect::ObjectCast< Luna::DirectionalLight >( light ))
  {
    SetLinkedBakedLights(light);
    return;
  }

  RemoteEditor::RemoveLight( light->GetPackage< Content::Light >() );
}

void RemoteScene::TweakLightmapSettings( const LightmapTweakArgs& args )
{
  if( !m_Enabled )
  {
    return;
  }

  const Luna::Entity* entity = args.m_Entity;
  const u32 lm_set_index = args.m_LightmapSetIndex;

  if(!entity)
  {
    return;
  }

  if(entity->IsTransient())
  {
    return;
  }

  const Asset::Entity* package = entity->GetPackage< Asset::Entity >();
  if( package )
  {
    Dependencies::Graph().ClearCache();
    LightingJob::ClearZoneCache();
    LightingJob::ClearVertMapCache();

    // get all the lighting jobs
    std::vector<Content::LightingJob*> lightingJobs;
    // get all the lighting jobs
    Scene* lightingScene = m_SceneEditor->GetSceneManager()->GetLightingScene( true );
    if( lightingScene )
      lightingScene->GetAllPackages< Content::LightingJob >( lightingJobs );

    for each( Content::LightingJob* job in lightingJobs )
    {
      if( job->ContainsRenderTarget( entity->GetID() ) )
      {
        LightingJob::GetInstanceLightingData( *package, *job );
        SendLightingData(job, package, false);
        break;
      }
    }
  }
}

void RemoteScene::TweakCubemapSettings( const CubemapTweakArgs& args )
{
  if( !m_Enabled )
  {
    return;
  }

  Luna::Entity* entity = args.m_Entity;

  if(!entity)
  {
    return;
  }

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
  if ( param.m_EngineType == -1 )
  {
    return;
  }

  param.m_ID = entity->GetID();
  param.m_Intensity = entity->GetCubeMapIntensity();

  m_LunaViewHost->TweakCubemapSettings( &param );
}

void RemoteScene::SceneNodeDeleting( const NodeChangeArgs& args )
{
  if( !m_Enabled )
  {
    return;
  }

  if ( m_SubsetScene )
  {
    if ( !m_Scene.Get< Reflect::Element >( args.m_Node->GetID() ) )
    {
      return;
    }
  }

  Luna::Light* light = Reflect::ObjectCast< Luna::Light >( args.m_Node );
  if ( light )
  {
    light->RemoveChangedListener( LightChangeSignature::Delegate ( this, &RemoteScene::SetLightParams ) );
    light->RemoveRealtimeLightExistenceListener( RealtimeLightExistenceSignature::Delegate ( this, &RemoteScene::RealtimeLightExistenceChanged ) );

    RemoveLight( light );
  }
  else if( args.m_Node->HasType( Reflect::GetType<Luna::LightingVolume>() ) )
  {
    bool globalVolume = GetIsGlobalVolume(args.m_Node);

    if ( !globalVolume )
    {
      RemoveLightingVolume( Reflect::DangerousCast< Luna::LightingVolume >( args.m_Node ) );
    }
  }
  else if( args.m_Node->HasType( Reflect::GetType<PostProcessingVolume>() ) )
  {
    bool globalVolume = GetIsGlobalVolume(args.m_Node);

    if ( !globalVolume )
    {
      RemovePostProcessingVolume( Reflect::DangerousCast< PostProcessingVolume >( args.m_Node ) );
    }
  }
  else if ( Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( args.m_Node ) )
  {
    RemoveEntityInstance( entity );
  }
  else if ( Luna::Volume* volume = Reflect::ObjectCast< Luna::Volume >( args.m_Node ) )
  {
    RemoveVolume( volume );
  }
}

void RemoteScene::SceneNodeCreated( const NodeChangeArgs& args )
{
  if( !m_Enabled || m_SubsetScene )
  {
    return;
  }

  if ( Luna::Light* light = Reflect::ObjectCast< Luna::Light >( args.m_Node ) )
  {
    light->AddChangedListener( LightChangeSignature::Delegate ( this, &RemoteScene::SetLightParams ) );
    light->AddRealtimeLightExistenceListener( RealtimeLightExistenceSignature::Delegate ( this, &RemoteScene::RealtimeLightExistenceChanged ) );

    AddLight( light );
  }
  else if( args.m_Node->HasType( Reflect::GetType<Luna::LightingVolume>() ) )
  {
    AddLightingVolume( Reflect::DangerousCast< Luna::LightingVolume >( args.m_Node ) );
  }
  else if( args.m_Node->HasType( Reflect::GetType<Luna::PostProcessingVolume>() ) )
  {
    AddPostProcessingVolume( Reflect::DangerousCast< Luna::PostProcessingVolume >( args.m_Node ) );
  }
  else if ( Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( args.m_Node ) )
  {
    AddEntityInstance( entity );
  }
  else if ( Luna::Volume* volume = Reflect::ObjectCast< Luna::Volume >( args.m_Node ) )
  {
    AddVolume( volume );
  }
}

void RemoteScene::EntityAssetChanging( const EntityAssetChangeArgs& args )
{
  if( !m_Enabled )
  {
    return;
  }

  if ( m_SubsetScene )
  {
    if ( !m_Scene.Get< Reflect::Element >( args.m_Entity->GetID() ) )
    {
      return;
    }
  }

  RemoveEntityInstance( args.m_Entity, false );
}

void RemoteScene::EntityAssetChanged( const EntityAssetChangeArgs& args )
{
  if( !m_Enabled )
  {
    return;
  }

  if ( m_SubsetScene )
  {
    if ( !m_Scene.Get< Reflect::Element >( args.m_Entity->GetID() ) )
    {
      return;
    }
  }

  AddEntityInstance( args.m_Entity, false );
}

void RemoteScene::EntityJointTransformsChanged( const Asset::JointTransformsChangedArgs& args )
{
  if( !m_Enabled )
  {
    return;
  }

  if ( m_SubsetScene )
  {
    if ( !m_Scene.Get< Reflect::Element >( args.m_Entity->m_ID ) )
    {
      return;
    }
  }

  SendMobyAnimJointMats( args.m_Entity );
}

void RemoteScene::InstancePropertiesChanged( const InstancePropertiesChangeArgs& args )
{
  if((args.m_OldType == WeatherBlockingStr) && (args.m_NewType != WeatherBlockingStr))
  {
    RemoveVolume(args.m_Instance, false);
    return;
  }

  if(args.m_NewType  == WeatherBlockingStr)
  {
    AddVolume(args.m_Instance, false);
    return;
  }
}

void RemoteScene::EnableLighting( bool enable )
{
  m_SyncLighting = enable;

  if ( !m_Enabled )
  {
    return;
  }

  if ( m_SyncLighting )
  {
    std::vector<Content::LightingJob*> lightingJobs;
    // get all the lighting jobs
    Luna::Scene* lightingScene = m_SceneEditor->GetSceneManager()->GetLightingScene( true );
    if( lightingScene )
      lightingScene->GetAllPackages< Content::LightingJob >( lightingJobs );

    Asset::V_Entity entities;
    m_Scene.GetAll< Asset::Entity >( entities );

    for each(Content::LightingJob* job in lightingJobs )
    {
      for each ( const Asset::EntityPtr& entity in entities )
      {
        if( job->ContainsRenderTarget( entity->m_ID ) )
        {
          SendLightingData( job, entity );
        }
      }
    }
  }
}

void RemoteScene::SendEntityInstance( Asset::Entity* entity )
{
  if ( !m_Enabled )
  {
    return;
  }

  RemoteEditor::SendEntityInstance( entity );

  if ( m_SyncLighting )
  {
    std::vector<Content::LightingJob*> lightingJobs;
    // get all the lighting jobs
    Luna::Scene* lightingScene = m_SceneEditor->GetSceneManager()->GetLightingScene( true );
    if( lightingScene )
      lightingScene->GetAllPackages< Content::LightingJob >( lightingJobs );

    for each(Content::LightingJob* job in lightingJobs )
    {
      if( job->ContainsRenderTarget( entity->m_ID ) )
      {
        SendLightingData( job, entity );
      }
    }
  }
}

void RemoteScene::AddEntityInstance( Luna::Entity* entity, bool addListeners )
{
  if( !m_Enabled )
  {
    return;
  }

  Asset::EntityAsset* entityClass = entity->GetClassSet()->GetEntityAsset();

  if ( !entityClass )
  {
    return;
  }

  if ( addListeners )
  {
    entity->AddCubemapTweakedListener( CubemapTweakSignature::Delegate ( this, &RemoteScene::TweakCubemapSettings ) );
    entity->AddLightmapTweakedListener( LightmapTweakSignature::Delegate ( this, &RemoteScene::TweakLightmapSettings ) );
    entity->AddClassChangingListener( EntityAssetChangeSignature::Delegate ( this, &RemoteScene::EntityAssetChanging ) );
    entity->AddClassChangedListener( EntityAssetChangeSignature::Delegate ( this, &RemoteScene::EntityAssetChanged ) );

    entity->GetPackage< Asset::Entity >()->AddJointTransformsChangedListener( Asset::JointTransformsChangedSignature::Delegate ( this, &RemoteScene::EntityJointTransformsChanged ) );
  }

  Asset::EngineType engineType = entityClass->GetEngineType();
  if ( engineType == Asset::EngineTypes::Tie
    || engineType == Asset::EngineTypes::Ufrag )
  {
    SetCollisionDbValid( false );
  }

  entity->Pack();
  Asset::Entity* contentEntity = entity->GetPackage< Asset::Entity >();
  RemoteEditor::AddEntityInstance( contentEntity );

  SendOcclusionId( entity );
}

void RemoteScene::RemoveEntityInstance( Luna::Entity* entity, bool removeListeners )
{
  if( !m_Enabled )
  {
    return;
  }

  Asset::EntityAsset* entityClass = entity->GetClassSet()->GetEntityAsset();

  if ( !entityClass )
  {
    return;
  }

  if ( removeListeners )
  {
    entity->RemoveCubemapTweakedListener( CubemapTweakSignature::Delegate ( this, &RemoteScene::TweakCubemapSettings ) );
    entity->RemoveLightmapTweakedListener( LightmapTweakSignature::Delegate ( this, &RemoteScene::TweakLightmapSettings ) );
    entity->RemoveClassChangingListener( EntityAssetChangeSignature::Delegate ( this, &RemoteScene::EntityAssetChanging ) );
    entity->RemoveClassChangedListener( EntityAssetChangeSignature::Delegate ( this, &RemoteScene::EntityAssetChanged ) );

    entity->GetPackage< Asset::Entity >()->RemoveJointTransformsChangedListener( Asset::JointTransformsChangedSignature::Delegate ( this, &RemoteScene::EntityJointTransformsChanged ) );
  }

  Asset::EngineType engineType = entityClass->GetEngineType();
  if ( engineType == Asset::EngineTypes::Tie
    || engineType == Asset::EngineTypes::Ufrag )
  {
    SetCollisionDbValid( false );
  }

  m_SimulatedMobys.erase( entity->GetID() );

  entity->Pack();
  RemoteEditor::RemoveEntityInstance( entity->GetPackage< Asset::Entity >() );
}

void RemoteScene::SetLightingVolumeParams( const LightingVolumeChangeArgs& args )
{
  if( !m_Enabled )
  {
    return;
  }

  Luna::LightingVolume* light = args.m_LightingVolume;
  bool globalVolume  = GetIsGlobalVolume(light);

  LightingVolumeParametersParam param;
  ZeroMemory(&param, sizeof(LightingVolumeParametersParam));

  {
    // identify a global volume w/ the null uid
    param.m_ID = globalVolume ? TUID::Null : light->GetID();
    Content::LightingVolume*  lightVol  = light->GetPackage< Content::LightingVolume >();

    param.m_OverrideFlags              |= lightVol->m_ExcludeGlobalLights ? IGPS3::LIGHT_VOL_EXCLUDE_GLOBAL_LINKED_LIGHTS : 0;

    // Sun Shadow direction
    {
      OS_SelectableDumbPtr  linkedLights;
      param.m_ShadowDirection.z = 1.0f;

      if(GetLightsLinkedToLightingVolume(light, linkedLights))
      {
        OS_SelectableDumbPtr::Iterator  memberItr     = linkedLights.Begin();
        OS_SelectableDumbPtr::Iterator  memberEnd     = linkedLights.End();

        for ( ; (memberItr != memberEnd); ++memberItr )
        {
          Selectable*     member  = (*memberItr);
          Luna::Light*    lLight  = Reflect::ObjectCast< Luna::Light >( member );

          //Sun Shadow Direction
          if(Luna::ShadowDirection* shadowDir = Reflect::ObjectCast< Luna::ShadowDirection >( member ))
          {
            Content::Light* cLight    = lLight->GetPackage<Content::Light>();
            param.m_ShadowDirection.x = -cLight->m_GlobalTransform.z.x;
            param.m_ShadowDirection.y = -cLight->m_GlobalTransform.z.y;
            param.m_ShadowDirection.z = -cLight->m_GlobalTransform.z.z;
            param.m_OverrideFlags    |= IGPS3::LIGHT_VOL_OVERRIDE_SHADOW_DIRECTION;

            break;
          }
        }
      }
    }

    // Sun shadow merge
    {
      Attribute::AttributeViewer< Content::SunShadowMergeAttribute > shadowMergeAttr( lightVol);

      //Defaults for global
      if(globalVolume)
      {
        param.m_SunShadowFadeDists[0]             = 36.f;
        param.m_SunShadowOverlapWeights[0]        = 2.f;
        param.m_SunShadowMergeXYThresholds[0]     = 35.f;
        param.m_SunShadowMergeScoreThresholds[0]  = 0.6f;

        param.m_SunShadowFadeDists[1]             = 80.f;
        param.m_SunShadowOverlapWeights[1]        = 2.f;
        param.m_SunShadowMergeXYThresholds[1]     = 100.f;
        param.m_SunShadowMergeScoreThresholds[1]  = 0.8f;
      }

      if(shadowMergeAttr.Valid())
      {
        param.m_OverrideFlags                    |= IGPS3::LIGHT_VOL_OVERRIDE_SHADOW_MERGE;
        param.m_SunShadowFadeDists[0]             = shadowMergeAttr->m_FadeDistNear;
        param.m_SunShadowFadeDists[1]             = shadowMergeAttr->m_FadeDistFar;
        param.m_SunShadowOverlapWeights[0]        = shadowMergeAttr->m_OverlayWeightNear;
        param.m_SunShadowOverlapWeights[1]        = shadowMergeAttr->m_OverlayWeightFar;
        param.m_SunShadowMergeXYThresholds[0]     = shadowMergeAttr->m_MergeXYThresholdNear;
        param.m_SunShadowMergeXYThresholds[1]     = shadowMergeAttr->m_MergeXYThresholdFar;
        param.m_SunShadowMergeScoreThresholds[0]  = shadowMergeAttr->m_MergeScoreThresholdNear;
        param.m_SunShadowMergeScoreThresholds[1]  = shadowMergeAttr->m_MergeScoreThresholdFar;
      }
    }

    //Ground light settings
    {
      Attribute::AttributeViewer< Content::GroundLightAttribute > groundLightAttr( lightVol);

      //Defaults for global
      if(globalVolume)
      {
        param.m_BouncedStrength           = 0.05f;
        param.m_AmbientOcclusionStrength  = 0.80f;
      }

      if(groundLightAttr.Valid())
      {
        param.m_OverrideFlags            |=  IGPS3::LIGHT_VOL_OVERRIDE_GROUND_LIGHTING_SETTINGS;

        param.m_BouncedStrength           = groundLightAttr->m_BouncedStrength         ;
        param.m_AmbientOcclusionStrength  = groundLightAttr->m_AmbientOcclusionStrength;
      }
    }

    //Gloss controls
    {
      Attribute::AttributeViewer< Content::GlossControlAttribute > glossControlAttr( lightVol);

      //Defaults for global
      if(globalVolume)
      {
        param.m_AmbientLevel       = 0.01f;
        param.m_AttenuationCutoff  = 0.60f;
      }

      if(glossControlAttr.Valid())
      {
        param.m_OverrideFlags     |=  IGPS3::LIGHT_VOL_OVERRIDE_GLOSS_SETTINGS;

        param.m_AmbientLevel       = glossControlAttr->m_AmbientLevel     ;
        param.m_AttenuationCutoff  = glossControlAttr->m_AttenuationCutoff;
      }
    }
  }
  m_LunaViewHost->SetLightingVolumeParameters( &param);
}

///////////////////////////////////////////////////////////////////////////////////////////
void RemoteScene::SendVolume( Luna::Instance* volume )
{
  if( !m_Enabled || GetIsGlobalVolume(volume) || !volume->GetIsWeatherBlocker())
  {
    return;
  }

  AddVolumeParam param;

  param.m_ID          = volume->GetID();
  param.m_Type        = VolumeTypes::WeatherBlocking;
  param.m_Flags       = volume->GetSkipParticles() ? WEATHER_BLOCKING_VOLUME_SKIP_PARTICLES : 0;
  param.m_BorderSize  = volume->GetBorderSize();

  strncpy( param.m_Name, volume->GetName().c_str(), sizeof( param.m_Name ));
  param.m_Name[ sizeof(param.m_Name)-1 ] = 0;

  m_LunaViewHost->AddVolume( &param );
}

///////////////////////////////////////////////////////////////////////////////////////////
void RemoteScene::AddVolume( Luna::Instance* volume,  bool addListeners)
{
  if( !m_Enabled || GetIsGlobalVolume(volume) )
  {
    return;
  }

  if(addListeners)
  {
    volume->AddConfiguredTypeChangedListener( InstancePropertiesChangeSignature::Delegate( this, &RemoteScene::InstancePropertiesChanged ) );
  }

  //Only send it over the network if it is recognizable
  if(volume->GetIsWeatherBlocker())
  {
    SendVolume( volume );
    TransformInstance( volume );
  }
  volume->Pack();
}

///////////////////////////////////////////////////////////////////////////////////////////
void RemoteScene::RemoveVolume(  Luna::Instance* volume,  bool removeListeners)
{
  if( !m_Enabled || GetIsGlobalVolume(volume))
  {
    return;
  }

  if(removeListeners)
  {
    volume->RemoveConfiguredTypeChangedListener( InstancePropertiesChangeSignature::Delegate( this, &RemoteScene::InstancePropertiesChanged ) );
  }

  RemoveVolumeParam param;
  param.m_ID    = volume->GetID();
  param.m_Type  = VolumeTypes::WeatherBlocking;
  m_LunaViewHost->RemoveVolume( &param );
}

///////////////////////////////////////////////////////////////////////////////////////////
void RemoteScene::AddLightingVolume( Luna::LightingVolume* volume )
{
  if( !m_Enabled )
  {
    return;
  }

  volume->AddChangedListener( LightingVolumeChangeSignature::Delegate ( this, &RemoteScene::SetLightingVolumeParams ) );

  SendLightingVolume( volume );
  TransformInstance( volume );
  SetLightingVolumeParams( volume );

  volume->Pack();
}

template <class T>
void SetupPostProcessingVolumeCustomMap( RemoteScene* scene, PostProcessingVolume* volume, u32 index)
{
  Content::PostProcessingVolume*  pkg = volume->GetPackage<Content::PostProcessingVolume>();

  if(pkg)
  {
    Attribute::AttributeViewer< T >                    customMapAttrViewer(pkg);
    PostEffectsCustomMapAttributeChangeArgs args(volume);
    args.m_Index  = index;

    if(customMapAttrViewer.Valid())
    {
      args.m_TexturePath   =  customMapAttrViewer->GetFilePath();
      args.m_ForceRebuild  =  false;

      scene->SetPostEffectsCustomMapAttributeParams( args );
      return;
    }
    scene->SetPostEffectsCustomMapAttributeParams( args );
  }
}

void RemoteScene::SetupPostProcessingVolume( PostProcessingVolume* volume )
{
  SendPostProcessingVolume( volume );
  TransformInstance( volume );

  SetPostEffectsLightScatteringAttributeParams(volume );
  SetPostEffectsColorCorrectionAttributeParams(volume );
  SetPostEffectsCurveControlAttributeParams(volume );
  SetPostEffectsDepthOfFieldAttributeParams(volume );
  SetPostEffectsGraphShaderAttributeParams(volume );
  SetPostEffectsMotionBlurAttributeParams(volume );
  SetPostEffectsFilmGrainAttributeParams(volume );
  SetPostEffectsCausticsAttributeParams(volume );
  SetPostEffectsBloomAttributeParams(volume );
  SetPostEffectsColorAttributeParams(volume );
  SetPostEffectsBlurAttributeParams(volume );
  SetPostEffectsFogAttributeParams(volume );
  SetPostEffectsHDRAttributeParams(volume );

  SetupPostProcessingVolumeCustomMap<Asset::CustomMapAAttribute>(this, volume, 0);
  SetupPostProcessingVolumeCustomMap<Asset::CustomMapBAttribute>(this, volume, 1);

  volume->Pack();
}

void RemoteScene::AddPostProcessingVolume( PostProcessingVolume* volume )
{
  if( !m_Enabled )
  {
    return;
  }

  volume->AddChangedListener( PostEffectsLightScatteringAttributeChangeSignature::Delegate ( this, &RemoteScene::SetPostEffectsLightScatteringAttributeParams ) );
  volume->AddChangedListener( PostEffectsColorCorrectionAttributeChangeSignature::Delegate ( this, &RemoteScene::SetPostEffectsColorCorrectionAttributeParams ) );
  volume->AddChangedListener( PostEffectsCurveControlAttributeChangeSignature::Delegate ( this, &RemoteScene::SetPostEffectsCurveControlAttributeParams ) );
  volume->AddChangedListener( PostEffectsDepthOfFieldAttributeChangeSignature::Delegate ( this, &RemoteScene::SetPostEffectsDepthOfFieldAttributeParams ) );
  volume->AddChangedListener( PostEffectsGraphShaderAttributeChangeSignature::Delegate ( this, &RemoteScene::SetPostEffectsGraphShaderAttributeParams ) );
  volume->AddChangedListener( PostEffectsCustomMapAttributeChangeSignature::Delegate ( this, &RemoteScene::SetPostEffectsCustomMapAttributeParams ) );
  volume->AddChangedListener( PostEffectsMotionBlurAttributeChangeSignature::Delegate ( this, &RemoteScene::SetPostEffectsMotionBlurAttributeParams ) );
  volume->AddChangedListener( PostEffectsFilmGrainAttributeChangeSignature::Delegate ( this, &RemoteScene::SetPostEffectsFilmGrainAttributeParams ) );
  volume->AddChangedListener( PostEffectsCausticsAttributeChangeSignature::Delegate ( this, &RemoteScene::SetPostEffectsCausticsAttributeParams ) );
  volume->AddChangedListener( PostEffectsBloomAttributeChangeSignature::Delegate ( this, &RemoteScene::SetPostEffectsBloomAttributeParams ) );
  volume->AddChangedListener( PostEffectsColorAttributeChangeSignature::Delegate ( this, &RemoteScene::SetPostEffectsColorAttributeParams ) );
  volume->AddChangedListener( PostEffectsBlurAttributeChangeSignature::Delegate ( this, &RemoteScene::SetPostEffectsBlurAttributeParams ) );
  volume->AddChangedListener( PostEffectsFogAttributeChangeSignature::Delegate ( this, &RemoteScene::SetPostEffectsFogAttributeParams ) );
  volume->AddChangedListener( PostEffectsHDRAttributeChangeSignature::Delegate ( this, &RemoteScene::SetPostEffectsHDRAttributeParams ) );

  SetupPostProcessingVolume( volume );
}

void RemoteScene::RemoveLightingVolume( LightingVolume* volume )
{
  if( !m_Enabled )
  {
    return;
  }

  volume->RemoveChangedListener( LightingVolumeChangeSignature::Delegate ( this, &RemoteScene::SetLightingVolumeParams ) );

  RemoveVolumeParam param;

  bool globalVolume = GetIsGlobalVolume(volume);

  param.m_ID    = globalVolume ? TUID::Null : volume->GetID();
  param.m_Type  = VolumeTypes::Lighting;

  m_LunaViewHost->RemoveVolume( &param );
}


void RemoteScene::RemovePostProcessingVolume( PostProcessingVolume* volume )
{
  if( !m_Enabled )
  {
    return;
  }

  volume->RemoveChangedListener( PostEffectsLightScatteringAttributeChangeSignature::Delegate ( this, &RemoteScene::SetPostEffectsLightScatteringAttributeParams ) );
  volume->RemoveChangedListener( PostEffectsColorCorrectionAttributeChangeSignature::Delegate ( this, &RemoteScene::SetPostEffectsColorCorrectionAttributeParams ) );
  volume->RemoveChangedListener( PostEffectsCurveControlAttributeChangeSignature::Delegate ( this, &RemoteScene::SetPostEffectsCurveControlAttributeParams ) );
  volume->RemoveChangedListener( PostEffectsDepthOfFieldAttributeChangeSignature::Delegate ( this, &RemoteScene::SetPostEffectsDepthOfFieldAttributeParams ) );
  volume->RemoveChangedListener( PostEffectsGraphShaderAttributeChangeSignature::Delegate ( this, &RemoteScene::SetPostEffectsGraphShaderAttributeParams ) );
  volume->RemoveChangedListener( PostEffectsCustomMapAttributeChangeSignature::Delegate ( this, &RemoteScene::SetPostEffectsCustomMapAttributeParams ) );
  volume->RemoveChangedListener( PostEffectsMotionBlurAttributeChangeSignature::Delegate ( this, &RemoteScene::SetPostEffectsMotionBlurAttributeParams ) );
  volume->RemoveChangedListener( PostEffectsFilmGrainAttributeChangeSignature::Delegate ( this, &RemoteScene::SetPostEffectsFilmGrainAttributeParams ) );
  volume->RemoveChangedListener( PostEffectsCausticsAttributeChangeSignature::Delegate ( this, &RemoteScene::SetPostEffectsCausticsAttributeParams ) );
  volume->RemoveChangedListener( PostEffectsBloomAttributeChangeSignature::Delegate ( this, &RemoteScene::SetPostEffectsBloomAttributeParams ) );
  volume->RemoveChangedListener( PostEffectsColorAttributeChangeSignature::Delegate ( this, &RemoteScene::SetPostEffectsColorAttributeParams ) );
  volume->RemoveChangedListener( PostEffectsBlurAttributeChangeSignature::Delegate ( this, &RemoteScene::SetPostEffectsBlurAttributeParams ) );
  volume->RemoveChangedListener( PostEffectsFogAttributeChangeSignature::Delegate ( this, &RemoteScene::SetPostEffectsFogAttributeParams ) );
  volume->RemoveChangedListener( PostEffectsHDRAttributeChangeSignature::Delegate ( this, &RemoteScene::SetPostEffectsHDRAttributeParams ) );

  RemoveVolumeParam param;
  bool globalVolume = GetIsGlobalVolume(volume);

  param.m_ID    = globalVolume ? TUID::Null : volume->GetID();
  param.m_Type  = VolumeTypes::PostProcessing;

  m_LunaViewHost->RemoveVolume( &param );
}

void RemoteScene::SendLightingVolume( Luna::LightingVolume* volume )
{
  AddVolumeParam param;

  bool globalVolume = GetIsGlobalVolume(volume);
  param.m_ID    = globalVolume ? TUID::Null : volume->GetID();
  param.m_Type  = VolumeTypes::Lighting;

  strncpy( param.m_Name, volume->GetName().c_str(), sizeof( param.m_Name ));
  param.m_Name[ sizeof(param.m_Name)-1 ] = 0;

  m_LunaViewHost->AddVolume( &param );

  //Propagate the attached lights
  SetLinkedBakedLights(volume);
}


void RemoteScene::SendPostProcessingVolume( PostProcessingVolume* volume )
{
  AddVolumeParam param;

  bool globalVolume = GetIsGlobalVolume(volume);

  param.m_ID    = globalVolume ? TUID::Null : volume->GetID();
  param.m_Type  = VolumeTypes::PostProcessing;

  strncpy( param.m_Name, volume->GetName().c_str(), sizeof( param.m_Name ));
  param.m_Name[ sizeof(param.m_Name)-1 ] = 0;

  m_LunaViewHost->AddVolume( &param );
}

//
// to do -- share this with OcclusionBuilder.cpp and LevelOccl.cpp
//

// duplicate from igOcclusion.h
#define OCCL_CL_FILE_VERSION               0x0001

// duplicate from igOcclusion.h
struct OcclClusterHeader
{
  u32             m_version_id;
  u32             m_pad;
  u32             m_ufrag_ofs;
  u32             m_ufrag_cnt;
  u32             m_tie_ofs;
  u32             m_tie_cnt;
  u32             m_foliage_ofs;
  u32             m_foliage_cnt;
  u32             m_shrub_ofs;
  u32             m_shrub_cnt;
  u16             m_sky_valid;
  u16             m_sky_id;
  u16             m_pad2[2];
};

// duplicate from igOcclusion.h
#define OCCL_CLUSTER_INVALID   0

void RemoteScene::LoadOcclusion()
{
  m_OcclusionMap.clear();

  Asset::LevelAssetPtr level = m_SceneEditor->GetSceneManager()->GetCurrentLevel();

  if ( !level.ReferencesObject() )
  {
    return;
  }

  // copy occlusion data for this level from the network
  std::string relativeProcessedFolder = NSL::RelativeProcessed() + FinderSpecs::Occlusion::BUILT_FOLDER.GetRelativeFolder() + Asset::AssetClass::GetQualifiedName( level );
  std::string localProcessedFolder = FinderSpecs::Occlusion::BUILT_FOLDER.GetFolder() + Asset::AssetClass::GetQualifiedName( level ) + "/";

  try
  {
    NSL::CopyFromNetwork( relativeProcessedFolder );
  }
  catch ( NSL::Exception& )
  {
  }

  std::string file_name = FinderSpecs::Occlusion::CLUSTERS_FILE.GetFile( localProcessedFolder );

  // make sure the cluster file exists before going forward
  if ( !FileSystem::Exists( file_name ) )
  {
    return;
  }

  // clusters file is a raw file not an IGSerializer, this is a kinda funky way to load it and get a pointer to the data
  Nocturnal::BasicBufferPtr clusters_buf = new Nocturnal::BasicBuffer();
  clusters_buf->AddFile( file_name );
  OcclClusterHeader* cluster_header = (OcclClusterHeader*)clusters_buf->GetData();

  // check for cluster file version
  if (cluster_header->m_version_id != OCCL_CL_FILE_VERSION)
  {
    return;
  }

  m_SkyOcclId = OCCL_CLUSTER_INVALID;
  if (cluster_header->m_sky_valid)
  {
    u16 cluster_id = cluster_header->m_sky_id;
    m_SkyOcclId = ((cluster_id >> 3) << 8) | (1 << (cluster_id & 0x7));
  }

  // TODO: add GUID to data occlgen outputs for ufrag, so we can actually sync them up
  u16* pcluster = (u16*)( (u8*)cluster_header + cluster_header->m_ufrag_ofs );
  /*for (u32 iufrag = 0; iufrag < cluster_header->m_ufrag_cnt; iufrag++, pcluster += 4)
  {
  u16 cluster_index_cnt = pcluster[1];
  u32 cluster_hash      = *(u32*)(pcluster + 2);

  OcclusionCluster cluster;
  cluster.m_BoundsHash = cluster_hash;

  if ( iufrag < ufrag_count )
  {
  BasicBufferPtr ufrag_buf = m_main_file.GetChunk(IGG::FILECHUNK_UFRAG_FRAGMENTS, iufrag);
  IGPS3::Ufrag* ufrag = (IGPS3::Ufrag*)ufrag_buf->GetData();

  u32 ufrag_index_cnt   = ConvertEndian( ufrag->m_index_count, true );
  u32 ufrag_hash        = OcclHashBounds( (u32*)(ufrag->m_obb + 3) );

  if ( ufrag_index_cnt == cluster_index_cnt && ufrag_hash == cluster_hash )
  {
  u16 cluster_id = pcluster[0];
  u16 occl_id = ((cluster_id >> 3) << 8) | (1 << (cluster_id & 0x7));
  ufrag->m_occl_id = ConvertEndian( occl_id, true );
  ufrag_valid++;
  }
  else
  {
  ufrag->m_occl_id = OCCL_CLUSTER_INVALID;
  }
  }
  }*/

  // ties
  pcluster = (u16*)( (u8*)cluster_header + cluster_header->m_tie_ofs );
  for (u32 itie = 0; itie < cluster_header->m_tie_cnt; itie++, pcluster += 8)
  {
    u32 cluster_hash     = *(u32*)(pcluster + 2);

    tuid uid;
    memcpy( &uid, pcluster + 4, sizeof( uid ) );

    u16 cluster_id = pcluster[0];
    u16 occl_id = ((cluster_id >> 3) << 8) | (1 << (cluster_id & 0x7));
    m_OcclusionMap.insert( std::make_pair( uid, (u32)occl_id ) );
  }

  // shrubs
  pcluster = (u16*)( (u8*)cluster_header + cluster_header->m_shrub_ofs );
  for (u32 ishrub = 0; ishrub < cluster_header->m_shrub_cnt; ishrub++, pcluster += 8)
  {
    u32 cluster_hash      = *(u32*)(pcluster + 2);

    tuid uid;
    memcpy( &uid, pcluster + 4, sizeof( uid ) );

    u16 cluster_id = pcluster[0];
    u16 occl_id = ((cluster_id >> 3) << 8) | (1 << (cluster_id & 0x7));
    m_OcclusionMap.insert( std::make_pair( uid, (u32)occl_id ) );
  }
}

void RemoteScene::SendOcclusionId( Luna::Entity* entity )
{
  if( !m_Enabled )
  {
    return;
  }

  UniqueID::HM_TUIDU32::iterator it = m_OcclusionMap.find( entity->GetID() );
  if ( it != m_OcclusionMap.end() )
  {
    OcclusionIdParam param;

    Asset::EntityAsset* entityClass = entity->GetClassSet()->GetEntityAsset();

    if ( entityClass )
    {
      param.m_Type = GetAssetType( entityClass );

      if ( param.m_Type != (u8)-1 )
      {
        param.m_ID = entity->GetID();
        param.m_OcclusionId = (u16)it->second;

        m_LunaViewHost->SetOcclusionId( &param );
      }
    }
  }
}

struct EntityAssetComparer
{
  bool operator()( const Asset::EntityPtr& elem1, const Asset::EntityPtr& elem2 )
  {
    return elem1->GetEntityAssetID() < elem2->GetEntityAssetID();
  }
};

//
//  Foliage is a bit of a special case
//

struct FoliageInstance
{
  Asset::EntityPtr instance;
  tuid             class_tuid;
};

typedef std::vector< FoliageInstance > V_FoliageInstance;

struct FoliageComparer
{
  bool operator()( const FoliageInstance& elem1, const FoliageInstance& elem2 )
  {
    return elem1.class_tuid < elem2.class_tuid;
  }
};

void RemoteScene::BuildCollisionDb()
{
  if ( !m_Enabled || !RuntimeConnection::IsConnected() )
  {
    return;
  }

  try
  {
    Luna::SceneManager* manager = m_SceneEditor->GetSceneManager();
    const M_SceneSmartPtr& scenes = manager->GetScenes();

    Asset::V_Entity tieInstances, ufragInstances;

    for each ( const M_SceneSmartPtr::value_type& val in scenes )
    {
      ScenePtr scene = val.second;

      Luna::V_EntityDumbPtr entities;
      scene->GetAll< Luna::Entity >( entities );

      V_EntityDumbPtr::iterator entityIt = entities.begin();
      V_EntityDumbPtr::iterator entityEnd = entities.end();
      for ( ; entityIt != entityEnd; ++entityIt )
      {
        Luna::Entity* entity = *entityIt;

        if ( entity->IsTransient() )
        {
          continue;
        }

        Asset::EntityAsset* entityClass = entity->GetClassSet()->GetEntityAsset();

        if ( !entityClass )
        {
          continue;
        }

        Asset::EngineType engineType = entityClass->GetEngineType();
        if ( engineType == Asset::EngineTypes::Tie )
        {
          entity->Pack();
          tieInstances.push_back( entity->GetPackage< Asset::Entity >() );
        }

        if ( engineType == Asset::EngineTypes::Ufrag )
        {
          entity->Pack();
          ufragInstances.push_back( entity->GetPackage< Asset::Entity >() );
        }
      }
    }

    std::sort( tieInstances.begin(), tieInstances.end(), EntityAssetComparer() );
    std::sort( ufragInstances.begin(), ufragInstances.end(), EntityAssetComparer() );

    IG::CollScene collScene;
    BuilderUtil::CreateCollisionSceneFromTieInstances( collScene, tieInstances );

    IG::IGSerializer tieCollFile( true );
    collScene.WriteScene( tieCollFile, false, 12.0, NULL );

    BuilderUtil::CollisionBuilder builder;

    IG::IGSerializer tempCollFile( true );
    builder.AddCollision( "", tempCollFile, tieCollFile, "fake zone" );

    for each ( const Asset::EntityPtr& entity in ufragInstances )
    {
      Asset::AssetClassPtr ufragAsset = entity->GetEntityAsset();

      std::string builtFilename = FinderSpecs::Ufrag::BUILT_FILE.GetFile( ufragAsset->GetBuiltDir() );

      if ( !FileSystem::Exists( builtFilename ) )
      {
        continue;
      }

      IGSerializer ufrag_main(true);
      ufrag_main.ReadFromFile( builtFilename.c_str() );

      builder.AddCollision( "", tempCollFile, ufrag_main, "ufrag" );
    }

    IG::IGSerializer finalCollFile( true );
    builder.FinalizeCollision( tempCollFile, finalCollFile );

    std::string filePath = FinderSpecs::Level::UBERVIEW_COLLISION_FILE.GetFile( FinderSpecs::Level::UBERVIEW_BUILT_FOLDER );

    FileSystem::MakePath( filePath, true );
    finalCollFile.WriteToFile( filePath.c_str(), IGG::FILECHUNK_COLL_DATA, 0 );

    LoadCollisionDbParam param;
    m_LunaViewHost->LoadCollisionDb( &param );

    SetCollisionDbValid( true );
  }
  catch( Nocturnal::Exception& e )
  {
    std::stringstream str;
    str << "Could not build collision db for scene:\n";
    str << e.what();
    wxMessageBox( str.str(), "Error building collision db", wxOK | wxICON_ERROR | wxCENTER, m_SceneEditor );
  }
}

void RemoteScene::SetCollisionDbValid( bool valid )
{
  m_CollisionDbValid = valid;
}

void RemoteScene::SendSkyOcclusionId()
{
  OcclusionIdParam param;
  param.m_Type = RPC::EngineTypes::Sky;
  param.m_OcclusionId = m_SkyOcclId;
  m_LunaViewHost->SetOcclusionId( &param );
}

void RemoteScene::SendScene()
{
  __super::SendScene();

  SendLevelData();

  if ( m_CollisionDbValid )
  {
    LoadCollisionDbParam param;
    m_LunaViewHost->LoadCollisionDb( &param );
  }

  SendSimulate();
  SendSimulationData();
}

void RemoteScene::SendLevelData()
{
  Asset::LevelAssetPtr level = m_SceneEditor->GetSceneManager()->GetCurrentLevel();

  if(level)
  {
    SetLevelWeatherAttributes(level, Luna::WEATHER_OP_TEXTURES2D_RELOAD);
    SetLevelAttributes(level);
  }
  else
  {
    ClearLevelAttributes();
  }

  TransformCamera();

  SendSkyOcclusionId();

  SendLightingVolumes();

  SendPostProcessingVolumes();

  SendWeatherBlockingVolumes();
}

void RemoteScene::SendLightingData( const Content::LightingJob* job, const Asset::Entity* entity, bool assignIndices )
{
  if ( !m_Enabled )
  {
    return;
  }

  Asset::EntityAsset* entityClass = entity->GetEntityAsset();
  if((entityClass->GetEngineType() != Asset::EngineTypes::Tie ) &&
     (entityClass->GetEngineType() != Asset::EngineTypes::Ufrag))
  {
    // uberview only supports lighting on tie and ufrag.
    // stop now, or else we'll just leak lighting data
    //
    return;
  }

  RPC::LoadLightingDataParam param;
  {
    memcpy( &param.m_ID, &entity->m_ID, sizeof( param.m_ID ) );

    std::string filePath;
    entity->m_ID.ToString( filePath );
    filePath = FinderSpecs::Zone::INSTANCE_LIGHTING_FILE.GetFile( job->GetBuiltDir() + filePath + "/" );

    FileSystem::StripPrefix( Finder::ProjectAssets(), filePath );
    filePath = "/" + filePath;

    strcpy( param.m_FilePath, filePath.c_str() );
    param.m_AssignIndices = assignIndices;

    Attribute::AttributeViewer< Asset::BakedLightingAttribute > bakedLighting( entity );
    if( bakedLighting.Valid() )
    {
      param.m_Format1 = (u8)bakedLighting->m_RuntimeLMFormat1;
      param.m_Format2 = (u8)bakedLighting->m_RuntimeLMFormat2;

      param.m_Size1 = (u8)bakedLighting->m_RuntimeLMSize1;
      param.m_Size2 = (u8)bakedLighting->m_RuntimeLMSize2;
    }
  }
  LoadLightingData( param );
}

void RemoteScene::BuildLightingData()
{
  Dependencies::Graph().ClearCache();
  LightingJob::ClearZoneCache();
  LightingJob::ClearVertMapCache();


  std::vector<Content::LightingJob*> lightingJobs;

  // get all the lighting jobs
  Luna::Scene* lightingScene = m_SceneEditor->GetSceneManager()->GetLightingScene( true );
  if( lightingScene )
    lightingScene->GetAllPackages< Content::LightingJob >( lightingJobs );

  Asset::V_Entity entities;
  m_Scene.GetAll< Asset::Entity >( entities );

  for each(Content::LightingJob* job in lightingJobs )
  {
    for each ( const Asset::EntityPtr& entity in entities )
    {
      if( job->ContainsRenderTarget( entity->m_ID ) )
      {
        LightingJob::BuildInstanceLightingData( *entity, *job );

        if ( m_SyncLighting )
        {
          SendLightingData( job, entity );
        }
      }
    }

    job->ClearCondensedData();
  }

  S_SceneSmartPtr scenes;
  scenes = GetScenes();
  S_SceneSmartPtr::const_iterator it = scenes.begin();
  S_SceneSmartPtr::const_iterator end = scenes.end();
  for ( ; it != end; ++it )
  {
    Luna::Scene* scene = *it;
    V_LightingVolumeDumbPtr volumes;
    scene->GetAll< Luna::LightingVolume >( volumes );

    V_LightingVolumeDumbPtr::iterator volIt = volumes.begin();
    V_LightingVolumeDumbPtr::iterator volEnd = volumes.end();
    for ( ; volIt != volEnd; ++volIt )
    {
      Content::LightingVolumePtr contentVolume = (*volIt)->GetPackage<Content::LightingVolume>();
      SetLightingVolumeParams( (*volIt) );
    }
  }
}

static inline u32 LightVolSampleLookup(u32 x, u32 y, u32 z, u8* samples, u32 x_cnt, u32 y_cnt, u32 z_cnt)
{
  if ((x < 0) || (x >= x_cnt) ||
    (y < 0) || (y >= y_cnt) ||
    (z < 0) || (z >= z_cnt))
    return -1;

  return samples[ x + (y * x_cnt) + (z * x_cnt * y_cnt) ];
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
//  filter luminance samples to smooth out results (uses a hard-coded filter kernel)
//
////////////////////////////////////////////////////////////////////////////////////////////////
static void LightVolFilterSamples( u8* samples, u32 sample_x_cnt, u32 sample_y_cnt, u32 sample_z_cnt )
{
  if (!samples || (sample_x_cnt < 2) || (sample_y_cnt < 2) || (sample_z_cnt < 2))
    return;

  i32 sample_cnt = (sample_x_cnt * sample_y_cnt * sample_z_cnt);

  // this can be replaced by malloc() in the tools
  u8* new_samples = new u8[sample_cnt];

  for (u32 iz = 0; iz < sample_z_cnt; iz++)
  {
    for (u32 iy = 0; iy < sample_y_cnt; iy++)
    {
      for (u32 ix = 0; ix < sample_x_cnt; ix++)
      {
        // just do a simple average between value and its neighbors
        i32 ngh_0 = LightVolSampleLookup(ix - 1, iy    , iz    , samples, sample_x_cnt, sample_y_cnt, sample_z_cnt);
        i32 ngh_1 = LightVolSampleLookup(ix + 1, iy    , iz    , samples, sample_x_cnt, sample_y_cnt, sample_z_cnt);
        i32 ngh_2 = LightVolSampleLookup(ix    , iy - 1, iz    , samples, sample_x_cnt, sample_y_cnt, sample_z_cnt);
        i32 ngh_3 = LightVolSampleLookup(ix    , iy + 1, iz    , samples, sample_x_cnt, sample_y_cnt, sample_z_cnt);
        i32 ngh_4 = LightVolSampleLookup(ix    , iy    , iz - 1, samples, sample_x_cnt, sample_y_cnt, sample_z_cnt);
        i32 ngh_5 = LightVolSampleLookup(ix    , iy    , iz + 1, samples, sample_x_cnt, sample_y_cnt, sample_z_cnt);

        i32 ngh_cnt = 0;
        ngh_cnt += (ngh_0 >= 0);
        ngh_cnt += (ngh_1 >= 0);
        ngh_cnt += (ngh_2 >= 0);
        ngh_cnt += (ngh_3 >= 0);
        ngh_cnt += (ngh_4 >= 0);
        ngh_cnt += (ngh_5 >= 0);

        i32 cur = LightVolSampleLookup(ix    , iy    , iz    , samples, sample_x_cnt, sample_y_cnt, sample_z_cnt);

        if (ngh_cnt > 0)
        {
          i32 ngh_avg = (((ngh_0 + ngh_1 + ngh_2 + ngh_3 + ngh_4 + ngh_5) + (6 - ngh_cnt)) + 3) / ngh_cnt;
          cur = (cur + ngh_avg) / 2;
        }

        new_samples[ ix + (iy * sample_x_cnt) + (iz * sample_x_cnt * sample_y_cnt) ] = cur;
      }
    }
  }
  memcpy(samples, new_samples, sample_cnt);

  // this can be replaced by free() in the tools
  delete [] new_samples;
}

S_SceneSmartPtr RemoteScene::GetScenes()
{
  S_SceneSmartPtr scenes;

  Luna::SceneManager* manager = m_SceneEditor->GetSceneManager();
  if( manager->GetRootScene() )
  {
    scenes.insert( manager->GetRootScene() );
  }

  M_SceneSmartPtr::const_iterator nonRootSceneIt = manager->GetScenes().begin();
  M_SceneSmartPtr::const_iterator nonRootSceneEnd = manager->GetScenes().end();
  for ( ; nonRootSceneIt != nonRootSceneEnd; ++nonRootSceneIt )
  {
    scenes.insert( nonRootSceneIt->second );
  }
  return scenes;
}

void RemoteScene::SendLightingVolumes()
{
  S_SceneSmartPtr scenes;
  scenes = GetScenes();

  S_SceneSmartPtr::const_iterator it = scenes.begin();
  S_SceneSmartPtr::const_iterator end = scenes.end();
  for ( ; it != end; ++it )
  {
    Luna::Scene* scene = *it;

    V_LightingVolumeDumbPtr volumes;
    scene->GetAll< Luna::LightingVolume >( volumes );

    V_LightingVolumeDumbPtr::iterator volIt = volumes.begin();
    V_LightingVolumeDumbPtr::iterator volEnd = volumes.end();
    for ( ; volIt != volEnd; ++volIt )
    {
      Content::LightingVolumePtr  contentVolume  = (*volIt)->GetPackage<Content::LightingVolume>();
      Luna::LightingVolume*       volume         = *volIt;
      SendLightingVolume( volume );
      TransformInstance( volume );
      SetLightingVolumeParams( volume );

      volume->Pack();
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RemoteScene::SendPostProcessingVolumes()
{
  S_SceneSmartPtr scenes;
  scenes = GetScenes();

  S_SceneSmartPtr::const_iterator it;
  S_SceneSmartPtr::const_iterator end = scenes.end();

  {
    it = scenes.begin();
    for ( ; it != end; ++it )
    {
      Luna::Scene* scene = *it;

      V_PostProcessingVolumeDumbPtr volumes;
      scene->GetAll< Luna::PostProcessingVolume >( volumes );

      for each ( const PostProcessingVolumePtr volume in volumes )
      {
        if ( !volume->IsTransient() )
        {
          SetupPostProcessingVolume(volume);
        }
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RemoteScene::SendWeatherBlockingVolumes()
{
  S_SceneSmartPtr scenes;
  scenes = GetScenes();

  S_SceneSmartPtr::const_iterator it;
  S_SceneSmartPtr::const_iterator end = scenes.end();

  {
    it = scenes.begin();
    for ( ; it != end; ++it )
    {
      Luna::Scene* scene = *it;

      V_VolumeDumbPtr volumes;
      scene->GetAll< Luna::Volume >( volumes );

      for each ( const VolumePtr volume in volumes )
      {
        if ( !volume->IsTransient() )
        {
          AddVolume( volume );
        }
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RemoteScene::SetPostEffectsLightScatteringAttributeParams( const PostEffectsLightScatteringAttributeChangeArgs& args )
{
  if( !m_Enabled )
  {
    return;
  }

  Luna::PostProcessingVolume* lVolume = args.m_PostProcessingVolume;
  bool globalVolume = GetIsGlobalVolume(lVolume);

  PostEffectsLightScatteringAttributeParam param;
  ZeroMemory(&param, sizeof(PostEffectsLightScatteringAttributeParam));

  // Light Scattering
  {
    Attribute::AttributeViewer< Content::PostEffectsLightScatteringAttribute > lightScatteringAttr( lVolume->GetPackage<Content::PostProcessingVolume>());
    param.m_Override = false;
    param.m_ID       = globalVolume ? 0 : lVolume->GetID();

    if(lightScatteringAttr.Valid())
    {
      f32 r, g, b;
      lightScatteringAttr->m_Color.Get(r, g, b);

      param.m_ColorR          = SrgbToLinear(r);
      param.m_ColorG          = SrgbToLinear(g);
      param.m_ColorB          = SrgbToLinear(b);

      param.m_Intensity       = lightScatteringAttr->m_Intensity;
      param.m_MieCoefficient  = lightScatteringAttr->m_MieCoefficient;
      param.m_MieDirectional  = lightScatteringAttr->m_MieDirectional;
      param.m_Override        = true;
    }
    else
    {
      //Check for the new light scattering attribute
      const Luna::LightScattering*  lightScatteringInst = lVolume->GetLightScatteringInstance();

      if((lightScatteringInst != NULL) && lightScatteringInst->GetIsEnabled())
      {
        Math::Vector3   direction;
        f32             r, g, b;

        lightScatteringInst->GetColor().Get(r, g, b);
        lightScatteringInst->GetDirection(direction);

        param.m_DirectionX      = direction.x;
        param.m_DirectionY      = direction.y;
        param.m_DirectionZ      = direction.z;

        param.m_ColorR          = SrgbToLinear(r);
        param.m_ColorG          = SrgbToLinear(g);
        param.m_ColorB          = SrgbToLinear(b);

        param.m_Intensity       = lightScatteringInst->GetIntensity();
        param.m_MieCoefficient  = lightScatteringInst->GetMieCoefficient();
        param.m_MieDirectional  = lightScatteringInst->GetMieDirectional();
        param.m_Override        = true;
      }
    }
  }

  m_LunaViewHost->SetPostEffectsLightScatteringAttributeParameters( &param);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RemoteScene::SetPostEffectsColorCorrectionAttributeParams( const PostEffectsColorCorrectionAttributeChangeArgs& args )
{
  if( !m_Enabled )
  {
    return;
  }

  PostProcessingVolume* lVolume = args.m_PostProcessingVolume;
  bool globalVolume = GetIsGlobalVolume(lVolume);

  PostEffectsColorCorrectionAttributeParam param;
  ZeroMemory(&param, sizeof(PostEffectsColorCorrectionAttributeParam));

  // Color Correction
  {
    Attribute::AttributeViewer< Content::PostEffectsColorCorrectionAttribute > colorCorrectionAttr( lVolume->GetPackage<Content::PostProcessingVolume>());
    param.m_Override = false;
    param.m_ID       = globalVolume ? 0 : lVolume->GetID();

    if(colorCorrectionAttr.Valid())
    {
      tuid   customPalette = colorCorrectionAttr->m_CustomPalette;
      size_t weightsSize   = colorCorrectionAttr->m_DepthEffectWeight.size();
      size_t colorSize     = colorCorrectionAttr->m_DepthEffectColor.size();

      //Check if we have any keyed values or a custom palette
      if(weightsSize || colorSize || (customPalette != TUID::Null))
      {
        //We have a valid palette
        ColorPalette::Palette::GeneratePalette(colorCorrectionAttr->m_DepthEffectWeight,
                                               colorCorrectionAttr->m_DepthEffectColor,
                                               colorCorrectionAttr->m_CustomPalette,
                                               param.m_Data);
        param.m_Override  = true;
      }
    }
  }

  m_LunaViewHost->SetPostEffectsColorCorrectionAttributeParameters( &param);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RemoteScene::SetPostEffectsCurveControlAttributeParams( const PostEffectsCurveControlAttributeChangeArgs& args )
{
  if( !m_Enabled )
  {
    return;
  }

  PostProcessingVolume* lVolume       = args.m_PostProcessingVolume;
  bool                  globalVolume  = GetIsGlobalVolume(lVolume);

  PostEffectsCurveControlAttributeParam param;
  ZeroMemory(&param, sizeof(PostEffectsCurveControlAttributeParam));

  // Curve Control
  {
    Attribute::AttributeViewer< Content::PostEffectsCurveControlAttribute > curveControlAttr( lVolume->GetPackage<Content::PostProcessingVolume>());
    param.m_Override = false;
    param.m_ID       = globalVolume ? 0 : lVolume->GetID();

    // Check if the level has curves :P
    if(globalVolume != false)
    {
      Asset::LevelAssetPtr level = m_SceneEditor->GetSceneManager()->GetCurrentLevel();

      if(level)
      {
        if(BuilderUtil::GenerateACVPalette(level->m_CurveControl, level->m_CurveControl_CRT, param.m_Data) == true)
        {
          if(curveControlAttr.Valid())
          {
            Console::Warning("Global post-processing volume and level both have CurveControl palettes, choosing level's!" );
          }

          param.m_Override  = true;
          m_LunaViewHost->SetPostEffectsCurveControlAttributeParameters( &param);

          // Done
          return;
        }
      }
    }

    // Proceed with the attribute
    if(curveControlAttr.Valid())
    {
      if(BuilderUtil::GenerateACVPalette(curveControlAttr->m_CurveId, curveControlAttr->m_CurveId_CRT, param.m_Data) == true)
      {
        param.m_Override  = true;
      }
    }
  }

  m_LunaViewHost->SetPostEffectsCurveControlAttributeParameters( &param);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RemoteScene::SetPostEffectsDepthOfFieldAttributeParams( const PostEffectsDepthOfFieldAttributeChangeArgs& args )
{
  if( !m_Enabled )
  {
    return;
  }

  PostProcessingVolume* lVolume = args.m_PostProcessingVolume;
  bool globalVolume = GetIsGlobalVolume(lVolume);

  PostEffectsDepthOfFieldAttributeParam param;
  ZeroMemory(&param, sizeof(PostEffectsDepthOfFieldAttributeParam));

  // DepthOfField
  {
    Attribute::AttributeViewer< Content::PostEffectsDepthOfFieldAttribute > depthOfFieldAttr( lVolume->GetPackage<Content::PostProcessingVolume>());
    param.m_Override          = false;
    param.m_ID                = globalVolume ? 0 : lVolume->GetID();

    if(depthOfFieldAttr.Valid())
    {
      param.m_FarStartDistance  = depthOfFieldAttr->m_FarStartDistance;
      param.m_FarEndDistance    = depthOfFieldAttr->m_FarEndDistance;
      param.m_FarMaxBlur        = depthOfFieldAttr->m_FarMaxBlur;
      param.m_Override          = true;
    }
  }

  m_LunaViewHost->SetPostEffectsDepthOfFieldAttributeParameters( &param);
}

/////////////////////////////////////////////////////////////////////////////////////
void CompileAndStreamCustomGraphShader(const Content::PostProcessingVolumePtr& volume,
                                       ILunaViewHost* host,
                                       const tuid&    tuid)
{
  // GraphShader
  Attribute::AttributeViewer< Content::PostEffectsGraphShaderAttribute > graphShaderAttr( volume );

  std::string  builtFile;
  std::string  builtFolder = Finder::GetBuiltFolder( graphShaderAttr->m_GraphFile );

  BuilderUtil::GraphShaderBuildPostEffects builder;

  builder.SetShaderFileTUIDAndBuiltDirectory(graphShaderAttr->m_GraphFile, builtFolder);
  builtFile = builder.GetBuiltFilePath();

  u32  fpDataSize       = 0;
  u8*  fpData           = NULL;

  PostEffectsGraphShaderAttributeParam param;
  param.m_ID            = tuid;
  param.m_GraphShaderID = graphShaderAttr->m_GraphFile;
  param.m_Flags         = 0;
  param.m_WrapU         = graphShaderAttr->m_WrapU;
  param.m_WrapV         = graphShaderAttr->m_WrapV;

  //Compile
  if(builder.Compile() != false)
  {
    IGSerializer built_data(true);

    //Read the fragment program info
    built_data.ReadFromFile(builtFile.c_str());

    Nocturnal::BasicBufferPtr  src_fp_data =  built_data.GetChunk(IGG::FILECHUNK_FRAGMENT_PROGRAM_DATA,  0);

    //If we have valid data, update our shader with it
    if(src_fp_data )
    {
      //Copy the flags
      param.m_Flags = builder.m_Flags;

      fpDataSize  = src_fp_data->GetSize();
      fpData      = new u8[src_fp_data->GetSize()];
      memcpy(fpData, src_fp_data->GetData(), fpDataSize);
    }
  }
  else
  {
    File::ManagedFilePtr  graphFile = File::GlobalManager().GetManagedFile( graphShaderAttr->m_GraphFile );
    Console::Error("Failed compiling graphshader: %s", graphFile->m_Path.c_str());
  }

  //Finalize the shader
  host->SetPostEffectsGraphShaderAttributeParameters( &param, fpData, fpDataSize);
  delete[] fpData;
}

///////////////////////////////////////////////////////////////////////////
// Thread for updating a posteffect graph shader.
//
class PostEffectsGraphShaderUpdateThread : public UIToolKit::DialogWorkerThread
{
private:
  Content::PostProcessingVolumePtr  m_Volume;
  ILunaViewHost*                    m_LunaViewHost;
  tuid                              m_VolumeTUID;

public:
  // Constructor
  PostEffectsGraphShaderUpdateThread( HANDLE                            evtHandle,
                                      wxDialog*                         dlg,
                                      Content::PostProcessingVolumePtr  volume,
                                      const tuid&                       volumeID,
                                      ILunaViewHost*                    lunaViewHost )
    : UIToolKit::DialogWorkerThread( evtHandle, dlg )
    , m_Volume(  volume )
    , m_VolumeTUID( volumeID )
    , m_LunaViewHost( lunaViewHost )
  {
  }

  // Do the work of building the graph shader and sending to the devkit.
  virtual void DoWork() NOC_OVERRIDE
  {
    if ( RuntimeConnection::IsConnected())
    {
      CompileAndStreamCustomGraphShader(m_Volume, m_LunaViewHost, m_VolumeTUID);
    }
  }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RemoteScene::SetPostEffectsGraphShaderAttributeParams( const PostEffectsGraphShaderAttributeChangeArgs& args )
{
  if( !m_Enabled )
  {
    return;
  }

  PostProcessingVolumePtr lVolume =  args.m_PostProcessingVolume;
  bool globalVolume    =  GetIsGlobalVolume(args.m_PostProcessingVolume);

  //Global volumes don't have it
  if(globalVolume)
  {
    return;
  }

  Content::PostProcessingVolumePtr  postprocVolume  = lVolume->GetPackage<Content::PostProcessingVolume>();
  // GraphShader
  Attribute::AttributeViewer< Content::PostEffectsGraphShaderAttribute > graphShaderAttr( postprocVolume );

  PostEffectsGraphShaderAttributeParam param;
  ZeroMemory(&param, sizeof(PostEffectsGraphShaderAttributeParam));

  param.m_ID        = lVolume->GetID();
  u32  fpDataSize   = 0;
  u8*  fpData       = NULL;

  if(graphShaderAttr.Valid())
  {
    //Verify that we have a valid graphfile before dropping the big hammer!
    if(graphShaderAttr->m_GraphFile != TUID::Null)
    {
      if(args.m_ForceCompile == true)
      {
        std::string msg( "Updating PostEffect Graph Shader: " );
        msg += File::GlobalManager().GetPath( graphShaderAttr->m_GraphFile ) + ".";

        ObjectUpdateDialog dlg( m_SceneEditor, "PostEffect Graph Shader Update", msg );
        dlg.InitThread( new PostEffectsGraphShaderUpdateThread( CreateEvent( NULL,
                                                                             TRUE,
                                                                             FALSE,
                                                                             "PostEffect Graph Shader Update Thread Event" ),
                                                                             &dlg,
                                                                             postprocVolume,
                                                                             lVolume->GetID(),
                                                                             m_LunaViewHost ) );
        dlg.ShowModal();
        return;
      }

      param.m_GraphShaderID     = graphShaderAttr->m_GraphFile;
      param.m_Flags             = IGPS3::IG_GRAPH_SHADER_CAPTURE_FRAME_BUFFER_HQ  |
                                  IGPS3::IG_GRAPH_SHADER_CAPTURE_DEPTH_BUFFER;

      param.m_WrapU             = graphShaderAttr->m_WrapU;
      param.m_WrapV             = graphShaderAttr->m_WrapV;

      std::string  builtFolder  = Finder::GetBuiltFolder( graphShaderAttr->m_GraphFile );
      std::string  builtFile    = FinderSpecs::Shader::GRAPH_SHADER_GRAPH_BUILT_FILE.GetFile(builtFolder);

      //File content
      IGSerializer builtData(true);

      //Read the fragment program container
      try
      {
        builtData.ReadFromFile(builtFile.c_str());

        //Locate the fragment program
        try
        {
          Nocturnal::BasicBufferPtr  srcFpData =  builtData.GetChunk(IGG::FILECHUNK_FRAGMENT_PROGRAM_DATA,  0);

          fpDataSize  = srcFpData->GetSize();
          fpData      = new u8[srcFpData->GetSize()];
          memcpy(fpData, srcFpData->GetData(), fpDataSize);
        }
        catch (const Nocturnal::Exception& ex){  Console::Error( "%s\n", ex.what() ); }
      }
      catch (const Nocturnal::Exception& ex)
      {
        //This is an exceptional case where the attribute was assigned during an offline session
        //and the data was never generated for this texture
        Console::Error( "%s\n", ex.what() );
        CompileAndStreamCustomGraphShader(postprocVolume, m_LunaViewHost, lVolume->GetID());
        return;
      }
    }
  }

  //Finalize the shader
  m_LunaViewHost->SetPostEffectsGraphShaderAttributeParameters( &param, fpData, fpDataSize);
  //Clean-up
  delete[] fpData;
}

///////////////////////////////////////////////////////////////////////////
template <class T>
void CompileAndStreamCustomMap(const Luna::PostProcessingVolumePtr& lVolume, ILunaViewHost* host, u32 index)
{
  const Content::PostProcessingVolume* pkg = lVolume->GetPackage<Content::PostProcessingVolume>();
  std::string dataPath;

  if(pkg)
  {
    Attribute::AttributeViewer< T >  customMapAttr(pkg);
    if (customMapAttr.Valid())
    {
      std::string texturePath = customMapAttr->GetFilePath();

      if(texturePath.size() != 0)
      {
        IG::OutputColorFormat format        = (IG::OutputColorFormat)customMapAttr->m_TextureFormat;
        bool                  generateMips  = (customMapAttr->m_GenerateMipMaps  == Asset::GenerateMipsOptions::CUSTOM_MAP_GENERATE_MIPS_YES);
        bool                  convertsRGB   = (customMapAttr->m_IsColorMap       == Asset::CustomMapTypes::CUSTOM_MAP_IS_COLOR_MAP_YES);
        bool                  expandRange   = (customMapAttr->m_ExpandRange      == Asset::RangeExpansionOptions::CUSTOM_MAP_EXPAND_RANGE_YES);
        bool                  UVClampU      = false;
        bool                  UVClampV      = false;

        Attribute::AttributeViewer< Content::PostEffectsGraphShaderAttribute > graphShaderAttr(pkg);

        if(graphShaderAttr.Valid())
        {
          UVClampU      = (graphShaderAttr->m_WrapU  == false);
          UVClampV      = (graphShaderAttr->m_WrapV  == false);
        }

        IG::Texture* texture = IG::Texture::LoadFile(texturePath.c_str(), convertsRGB, NULL);

        //Validate the texture
        if(texture == NULL)
        {
          Console::Warning("Unable to load texture: %s\n", texturePath.c_str());
        }
        else
        {
          IG::MipGenOptions mg;
          mg.m_Levels         = generateMips ? 0 : 1;
          mg.m_Filter         = (IG::FilterType)(customMapAttr->m_MipGenFilter);
          mg.m_ConvertToSrgb  = convertsRGB;
          mg.m_OutputFormat   = format;

          IG::MipSet::RuntimeSettings runtime;
          runtime.m_wrap_u        = UVClampU ? IG::UV_CLAMP : IG::UV_WRAP;
          runtime.m_wrap_v        = UVClampV ? IG::UV_CLAMP : IG::UV_WRAP;
          runtime.m_wrap_w        = IG::UV_WRAP;
          runtime.m_filter        = (IG::TextureFilter)(customMapAttr->m_TexFilter);
          runtime.m_direct_uvs    = false;
          runtime.m_expand_range  = expandRange;

          //Generate the mips
          IG::MipSet* mips    = texture->GenerateMipSet(mg, runtime);

          //No longer need this around
          delete texture;

          //Validate the mips
          if(mips == NULL)
          {
            Console::Warning("Unable to generate mips: %s\n", texturePath.c_str());
          }
          else
          {
            tuid  textureID                 = File::GlobalManager().GetID(texturePath);
            dataPath                        = Finder::GetBuiltFolder( textureID ) + "texture.dat";

            {
              IG::IGSerializer  dataWriter(true);

              Nocturnal::BasicBufferPtr data   = dataWriter.AddChunk(IGG::FILECHUNK_TEXTURE_DATA, IG::CHUNK_TYPE_SINGLE,0,0,0);
              Nocturnal::BasicBufferPtr header = dataWriter.AddChunk(IGG::FILECHUNK_TEXTURE_ARRAY,IG::CHUNK_TYPE_ARRAY, sizeof(IG::IGTexture),0,0);
              Nocturnal::BasicBufferPtr misc   = dataWriter.AddChunk(IGG::FILECHUNK_TEXTURE_DEBUG,IG::CHUNK_TYPE_ARRAY, sizeof(TextureProcess::TextureDebugInfo),0,0);
              TextureProcess::WriteMipSet(mips, header, data, misc);
              dataWriter.WriteToFile(dataPath.c_str());
              delete mips;
            }
          }
        }
      }
    }
  }

  PostEffectsCustomMapAttributeParam param;
  ZeroMemory(&param, sizeof(PostEffectsGraphShaderAttributeParam));

  param.m_ID          = lVolume->GetID();
  param.m_Index       = index;
  u32  texDataSize    = 0;
  u8*  texData        = NULL;

  if(dataPath.empty() == false)
  {
    //File content
    IGSerializer builtData(true);

    //Read the texture data container
    try
    {
      builtData.ReadFromFile(dataPath.c_str());

      //Locate the data and the header
      try
      {
        Nocturnal::BasicBufferPtr  srcTexData   =  builtData.GetChunk(IGG::FILECHUNK_TEXTURE_DATA,   0);
        Nocturnal::BasicBufferPtr  srcTexHeader =  builtData.GetChunk(IGG::FILECHUNK_TEXTURE_ARRAY,  0);

        texDataSize  = srcTexData->GetSize();
        texData      = new u8[srcTexData->GetSize()];
        memcpy(texData, srcTexData->GetData(), texDataSize);

        NOC_ASSERT(sizeof(param.m_TextureHeader) == srcTexHeader->GetSize());

        memcpy(param.m_TextureHeader, srcTexHeader->GetData(), srcTexHeader->GetSize());
      }
      catch (const Nocturnal::Exception& ex){  Console::Error( "%s\n", ex.what() ); }
    }
    catch (const Nocturnal::Exception& ex){  Console::Error( "%s\n", ex.what() );   }
  }

  host->SetPostEffectsCustomMapAttributeParameters( &param, texData, texDataSize);
  delete[] texData;
}

///////////////////////////////////////////////////////////////////////////
// Thread for updating a post-effect custom map.
//
class PostEffectsCustomMapUpdateThread : public UIToolKit::DialogWorkerThread
{
private:
  Luna::PostProcessingVolumePtr  m_Volume;
  u32                       m_MapIndex;
  ILunaViewHost*            m_LunaViewHost;

public:
  // Constructor
  PostEffectsCustomMapUpdateThread( HANDLE                   evtHandle,
                                    wxDialog*                dlg,
                                    Luna::PostProcessingVolumePtr volume,
                                    const u32                mapIndex,
                                    ILunaViewHost*           lunaViewHost )
    : UIToolKit::DialogWorkerThread( evtHandle, dlg )
    , m_Volume(  volume )
    , m_MapIndex( mapIndex )
    , m_LunaViewHost( lunaViewHost )
  {
  }

  // Do the work of building the graph shader and sending to the devkit.
  virtual void DoWork() NOC_OVERRIDE
  {
    if ( RuntimeConnection::IsConnected())
    {
      //Deal with Custom Map A
      if(m_MapIndex == 0)
      {
        CompileAndStreamCustomMap<Asset::CustomMapAAttribute>(m_Volume, m_LunaViewHost, m_MapIndex);
        return;
      }

      //Otherwise deal with custom Map B
      CompileAndStreamCustomMap<Asset::CustomMapBAttribute>(m_Volume, m_LunaViewHost, m_MapIndex);
    }
  }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RemoteScene::SetPostEffectsCustomMapAttributeParams( const PostEffectsCustomMapAttributeChangeArgs& args )
{
  if( !m_Enabled )
  {
    return;
  }

  Luna::PostProcessingVolumePtr lVolume =  args.m_PostProcessingVolume;
  bool globalVolume    = GetIsGlobalVolume(args.m_PostProcessingVolume);

  //Global volumes don't have it
  if(globalVolume)
  {
    return;
  }

  PostEffectsCustomMapAttributeParam param;
  ZeroMemory(&param, sizeof(PostEffectsGraphShaderAttributeParam));

  param.m_ID          = lVolume->GetID();
  param.m_Index       = args.m_Index;
  u32  texDataSize    = 0;
  u8*  texData        = NULL;

  if(args.m_TexturePath.empty() == false)
  {
    //Check if we need to rebuild the custom map
    if(args.m_ForceRebuild)
    {
      std::string msg( "Updating PostEffect Custom Map: " );
      msg += args.m_TexturePath + ".";

      ObjectUpdateDialog dlg( m_SceneEditor, "PostEffect Custom Map Update", msg );
      dlg.InitThread( new PostEffectsCustomMapUpdateThread( CreateEvent(NULL,
                                                                        TRUE,
                                                                        FALSE,
                                                                        "PostEffect Custom Map Update Thread Event" ),
                                                                        &dlg,
                                                                        lVolume,
                                                                        args.m_Index,
                                                                        m_LunaViewHost ) );
      dlg.ShowModal();
      return;
    }

    tuid        textureID   = File::GlobalManager().GetID(args.m_TexturePath);
    std::string builtFile   = Finder::GetBuiltFolder( textureID ) + "texture.dat";

    //File content
    IGSerializer builtData(true);

    //Read the fragment program container
    try
    {
      builtData.ReadFromFile(builtFile.c_str());

      //Locate the data and the header
      try
      {
        Nocturnal::BasicBufferPtr  srcTexData   =  builtData.GetChunk(IGG::FILECHUNK_TEXTURE_DATA,   0);
        Nocturnal::BasicBufferPtr  srcTexHeader =  builtData.GetChunk(IGG::FILECHUNK_TEXTURE_ARRAY,  0);

        texDataSize  = srcTexData->GetSize();
        texData      = new u8[srcTexData->GetSize()];
        memcpy(texData, srcTexData->GetData(), texDataSize);

        NOC_ASSERT(sizeof(param.m_TextureHeader) == srcTexHeader->GetSize());

        memcpy(param.m_TextureHeader, srcTexHeader->GetData(), srcTexHeader->GetSize());
      }
      catch (const Nocturnal::Exception& ex){  Console::Error( "%s\n", ex.what() ); }
    }
    catch (const Nocturnal::Exception& ex)
    {
      //This is an awkward case where the attribute was assigned during an off-line session
      //and the data was never generated for this texture
      Console::Error( "%s\n", ex.what() );

      //Deal with Custom Map A
      if(args.m_Index == 0)
      {
        CompileAndStreamCustomMap<Asset::CustomMapAAttribute>(lVolume, m_LunaViewHost, 0);
        return;
      }

      //Otherwise deal with custom Map B
      CompileAndStreamCustomMap<Asset::CustomMapBAttribute>(lVolume, m_LunaViewHost, 1);
      return;
    }
  }

  m_LunaViewHost->SetPostEffectsCustomMapAttributeParameters( &param, texData, texDataSize);

  delete[] texData;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RemoteScene::SetPostEffectsMotionBlurAttributeParams( const PostEffectsMotionBlurAttributeChangeArgs& args )
{
  if( !m_Enabled )
  {
    return;
  }

  PostProcessingVolume* lVolume = args.m_PostProcessingVolume;
  bool globalVolume = GetIsGlobalVolume(lVolume);

  PostEffectsMotionBlurAttributeParam param;
  ZeroMemory(&param, sizeof(PostEffectsMotionBlurAttributeParam));

  // Camera Motion Blur
  {
    Attribute::AttributeViewer< Content::PostEffectsMotionBlurAttribute > motionBlurAttr( lVolume->GetPackage<Content::PostProcessingVolume>());
    param.m_Override  = false;
    param.m_ID        = globalVolume ? 0 : lVolume->GetID();

    if(motionBlurAttr.Valid())
    {
      param.m_TranslationThreshold =  motionBlurAttr->m_TranslationThreshold;
      param.m_RotationThreshold    =  motionBlurAttr->m_RotationThreshold   ;
      param.m_ZoomThreshold        =  motionBlurAttr->m_ZoomThreshold       ;
      param.m_Scale                =  motionBlurAttr->m_Scale               ;
      param.m_Override             = true;
    }
  }

  m_LunaViewHost->SetPostEffectsMotionBlurAttributeParameters( &param);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RemoteScene::SetPostEffectsFilmGrainAttributeParams( const PostEffectsFilmGrainAttributeChangeArgs& args )
{
  if( !m_Enabled )
  {
    return;
  }

  PostProcessingVolume* lVolume = args.m_PostProcessingVolume;
  bool globalVolume = GetIsGlobalVolume(lVolume);

  PostEffectsFilmGrainAttributeParam param;
  ZeroMemory(&param, sizeof(PostEffectsFilmGrainAttributeParam));

  // FilmGrain
  {
    Attribute::AttributeViewer< Content::PostEffectsFilmGrainAttribute > filmGrainAttr( lVolume->GetPackage<Content::PostProcessingVolume>());
    param.m_Override  = false;
    param.m_ID        = globalVolume ? 0 : lVolume->GetID();

    if(filmGrainAttr.Valid())
    {
      param.m_Intensity    = filmGrainAttr->m_Intensity;
      param.m_Size         = filmGrainAttr->m_Size;
      param.m_Override     = true;
    }
  }

  m_LunaViewHost->SetPostEffectsFilmGrainAttributeParameters( &param);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RemoteScene::SetPostEffectsCausticsAttributeParams( const PostEffectsCausticsAttributeChangeArgs& args )
{
  if( !m_Enabled )
  {
    return;
  }

  PostProcessingVolume* lVolume = args.m_PostProcessingVolume;
  bool globalVolume = GetIsGlobalVolume(lVolume);

  PostEffectsCausticsAttributeParam param;
  ZeroMemory(&param, sizeof(PostEffectsCausticsAttributeParam));

  // Caustics
  {
    const Content::PostProcessingVolume*  pkg = lVolume->GetPackage<Content::PostProcessingVolume>();
    Attribute::AttributeViewer< Content::PostEffectsCausticsAttribute > causticsAttr(pkg);

    param.m_Override      = false;
    param.m_ID            = globalVolume ? 0 : lVolume->GetID();

    param.m_CausticsType  = pkg->m_IsUnderWater ? IGPS3::POSTPROC_VOL_TYPE_UNDER_WATER : 0;

    if(globalVolume && pkg->m_IsUnderWater)
    {
      param.m_CausticsType  = 0;
    }

    if(causticsAttr.Valid())
    {
      f32 r, g, b;
      causticsAttr->m_Color.Get(r, g, b);

      param.m_ColorR            = SrgbToLinear(r);
      param.m_ColorG            = SrgbToLinear(g);
      param.m_ColorB            = SrgbToLinear(b);

      param.m_UVScrollSpeed     = causticsAttr->m_UVScrollSpeed    ;
      param.m_UVScrollAngle     = causticsAttr->m_UVScrollAngle    ;
      param.m_UVScale           = causticsAttr->m_UVScale          ;
      param.m_StartFade         = causticsAttr->m_StartFade        ;
      param.m_EndFade           = causticsAttr->m_EndFade          ;
      param.m_CompositeStrength = causticsAttr->m_CompositeStrength;
      param.m_NormalFalloff     = causticsAttr->m_NormalFalloff    ;
      param.m_Override          = true;
    }
  }

  m_LunaViewHost->SetPostEffectsCausticsAttributeParameters( &param);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RemoteScene::SetPostEffectsBloomAttributeParams( const PostEffectsBloomAttributeChangeArgs& args )
{
  if( !m_Enabled )
  {
    return;
  }

  PostProcessingVolume* lVolume = args.m_PostProcessingVolume;
  bool globalVolume = GetIsGlobalVolume(lVolume);

  PostEffectsBloomAttributeParam param;
  ZeroMemory(&param, sizeof(PostEffectsBloomAttributeParam));

  // Bloom
  {
    Attribute::AttributeViewer< Content::PostEffectsBloomAttribute > bloomAttr( lVolume->GetPackage<Content::PostProcessingVolume>());
    param.m_Override  = false;
    param.m_ID        = globalVolume ? 0 : lVolume->GetID();

    if(bloomAttr.Valid())
    {
      param.m_CompositeWeight   = bloomAttr->m_CompositeWeight;
      param.m_Distribution      = bloomAttr->m_Distribution;
      param.m_Persistance       = bloomAttr->m_Persistance;
      param.m_Prescale          = bloomAttr->m_Prescale;
      param.m_Exponent          = bloomAttr->m_Exponent;
      param.m_Override          = true;
    }
  }

  m_LunaViewHost->SetPostEffectsBloomAttributeParameters( &param);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RemoteScene::SetPostEffectsColorAttributeParams( const PostEffectsColorAttributeChangeArgs& args )
{
  if( !m_Enabled )
  {
    return;
  }

  PostProcessingVolume* lVolume = args.m_PostProcessingVolume;
  bool globalVolume = GetIsGlobalVolume(lVolume);

  PostEffectsColorAttributeParam param;
  ZeroMemory(&param, sizeof(PostEffectsColorAttributeParam));

  // Color
  {
    Attribute::AttributeViewer< Content::PostEffectsColorAttribute > colorAttr( lVolume->GetPackage<Content::PostProcessingVolume>());
    param.m_ID          = globalVolume ? 0 : lVolume->GetID();
    param.m_ColorR      = 1.0f;
    param.m_ColorG      = 1.0f;
    param.m_ColorB      = 1.0f;
    param.m_Saturation  = 1.0f;
    param.m_Brightness  = 1.0f;
    param.m_Override    = false;

    if(colorAttr.Valid())
    {
      f32 r, g, b;
      colorAttr->m_Tint.Get(r, g, b);

      param.m_ColorR        = SrgbToLinear(r);
      param.m_ColorG        = SrgbToLinear(g);
      param.m_ColorB        = SrgbToLinear(b);
      param.m_Saturation    = colorAttr->m_Saturation;
      param.m_Brightness    = colorAttr->m_Brightness;
      param.m_Override      = true;
    }
  }

  m_LunaViewHost->SetPostEffectsColorAttributeParameters( &param);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RemoteScene::SetPostEffectsBlurAttributeParams( const PostEffectsBlurAttributeChangeArgs& args )
{
  if( !m_Enabled )
  {
    return;
  }

  PostProcessingVolume* lVolume = args.m_PostProcessingVolume;
  bool globalVolume = GetIsGlobalVolume(lVolume);

  PostEffectsBlurAttributeParam param;
  ZeroMemory(&param, sizeof(PostEffectsBlurAttributeParam));

  // Blur
  {
    Attribute::AttributeViewer< Content::PostEffectsBlurAttribute > blurAttr( lVolume->GetPackage<Content::PostProcessingVolume>());
    param.m_Override  = false;
    param.m_ID        = globalVolume ? 0 : lVolume->GetID();

    if(blurAttr.Valid())
    {
      param.m_PassCount     = blurAttr->m_PassCount;
      param.m_Extra         = blurAttr->m_Extra;
      param.m_Scale         = blurAttr->m_Scale;
      param.m_Override      = true;
    }
  }

  m_LunaViewHost->SetPostEffectsBlurAttributeParameters( &param);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RemoteScene::SetPostEffectsFogAttributeParams( const PostEffectsFogAttributeChangeArgs& args )
{
  if( !m_Enabled )
  {
    return;
  }

  PostProcessingVolume* lVolume = args.m_PostProcessingVolume;
  bool globalVolume = GetIsGlobalVolume(lVolume);

  PostEffectsFogAttributeParam param;
  ZeroMemory(&param, sizeof(PostEffectsFogAttributeParam));

  param.m_Override = false;

  //Check for the weather attribute
  if(globalVolume)
  {
    Asset::LevelAssetPtr level = m_SceneEditor->GetSceneManager()->GetCurrentLevel();
    
    if(level)
    {
      Attribute::AttributeViewer< Asset::WeatherAttribute > weatherAttr( level );

      if(weatherAttr.Valid())
      {
        tuid   customPalette = weatherAttr->m_FogCustomPalette;
        size_t weightsSize   = weatherAttr->m_FogWeights.size();
        size_t colorSize     = weatherAttr->m_FogColors.size();

        //Check if we have any keyed values or a custom palette
        if(weightsSize || colorSize || (customPalette != TUID::Null))
        {
          //We have a valid palette
          ColorPalette::Palette::GeneratePalette(weatherAttr->m_FogWeights,
                                                 weatherAttr->m_FogColors,
                                                 weatherAttr->m_FogCustomPalette,
                                                 param.m_Data);
          m_LunaViewHost->SetPostEffectsFogAttributeParameters( &param);

          //Done
          return;
        }
      }
    }
  }
  else
  {
    param.m_ID       = lVolume->GetID();
  }

  // Fog
  {
    Attribute::AttributeViewer< Content::PostEffectsFogAttribute > fogAttr( lVolume->GetPackage<Content::PostProcessingVolume>());
    if(fogAttr.Valid())
    {
      tuid   customPalette = fogAttr->m_CustomPalette;
      size_t weightsSize   = fogAttr->m_Weight.size();
      size_t colorSize     = fogAttr->m_Color.size();

      //Check if we have any keyed values or a custom palette
      if(weightsSize || colorSize || (customPalette != TUID::Null))
      {
        //We have a valid palette
        ColorPalette::Palette::GeneratePalette(fogAttr->m_Weight,
                                               fogAttr->m_Color,
                                               fogAttr->m_CustomPalette,
                                               param.m_Data);
        param.m_Override  = true;
      }
    }
  }

  m_LunaViewHost->SetPostEffectsFogAttributeParameters( &param);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RemoteScene::SetPostEffectsHDRAttributeParams( const PostEffectsHDRAttributeChangeArgs& args )
{
  if( !m_Enabled )
  {
    return;
  }

  PostProcessingVolume* lVolume = args.m_PostProcessingVolume;
  bool globalVolume = GetIsGlobalVolume(lVolume);

  PostEffectsHDRAttributeParam param;
  ZeroMemory(&param, sizeof(PostEffectsHDRAttributeParam));

  // HDR
  {
    Attribute::AttributeViewer< Content::PostEffectsHDRAttribute > HDRAttrViewer( lVolume->GetPackage<Content::PostProcessingVolume>());
    const Content::PostEffectsHDRAttribute  defaultHDRAttr;
    const Content::PostEffectsHDRAttribute* HDRAttrPtr;
    const f32 c_scale = 255.0f;

    param.m_Override  = false;
    param.m_ID        = globalVolume ? 0 : lVolume->GetID();

    if(HDRAttrViewer.Valid())
    {
      HDRAttrPtr        = HDRAttrViewer.operator->();
      param.m_Override  = true;
    }
    else
    {
      HDRAttrPtr        = &defaultHDRAttr;
    }

    param.m_Exposure              = HDRAttrPtr->m_Exposure;
    param.m_ExposureBracketLow    = HDRAttrPtr->m_ExposureBracketLow;
    param.m_ExposureBracketHigh   = HDRAttrPtr->m_ExposureBracketHigh;
    param.m_ExposureCoverageRate  = HDRAttrPtr->m_ExposureCoverageRate;
    param.m_MovingAverageWeight   = HDRAttrPtr->m_MovingAverageWeight;
    param.m_MedianLuminanceLow    = HDRAttrPtr->m_MedianLuminanceLow;
    param.m_MedianLuminanceHigh   = HDRAttrPtr->m_MedianLuminanceHigh;

    for(u32 idx = 0; idx < HISTORGRAM_BIN_COUNT; ++idx)
    {
      param.m_BinWeights[idx] = u8(HDRAttrPtr->m_BinWeights[idx]*c_scale + 0.5f);
    }

    param.m_CurrentBinIndex       = HDRAttrPtr->m_CurrentBinIndex;
    param.m_AutoExposure          = HDRAttrPtr->m_AutoExposure;
  }

  m_LunaViewHost->SetPostEffectsHDRAttributeParameters( &param);
}

void RemoteScene::SetInstanceCollision( const Asset::Entity* entity )
{
  __super::SetInstanceCollision( entity );

  if ( entity->GetEntityAsset()->GetEngineType() == Asset::EngineTypes::Tie )
  {
    SetCollisionDbValid( false );
  }
}

void RemoteScene::Screenshot( )
{
  m_LunaViewHost->Screenshot( );
}
