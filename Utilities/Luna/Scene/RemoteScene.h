#pragma once

#include "API.h"

#include "Asset/AssetClass.h"
#include "Asset/LevelAsset.h"
#include "Live/RemoteEditor.h"

#include "Scene.h"
#include "LightingVolume.h"

#include "PostProcessingVolume.h"
#include "UniqueID/TUID.h"
#include "Content/LightingJob.h"

#include "rpc/interfaces/rpc_lunaview_host.h"
#include "rpc/interfaces/rpc_lunaview_tool.h"
#include "rpc/interfaces/rpc_physics_host.h"
#include "rpc/interfaces/rpc_physics_tool.h"
#include "rpc/interfaces/rpc_common.h"

namespace Content
{
  class LightingJob;
  class LightingVolume;
}

namespace Asset
{
  struct JointTransformsChangedArgs;
}

namespace Luna
{
  class Editor;
  class Transform;
  class SceneEditor;
  class Light;
  class LightingVolume;
  class Entity;
  class Scene;
  class Volume;
  struct NodeChangeArgs;
  struct SceneChangeArgs;
  struct LoadArgs;
  struct SceneGraphEvaluatedArgs;
  struct CubemapTweakArgs;
  struct LightmapTweakArgs;
  struct LightingVolumeChangeArgs;
  struct LightChangeArgs;
  struct RealtimeLightExistenceArgs;
  struct RemoteCameraStatusArgs;
  struct CameraMovedArgs;
  struct EditorChangedArgs;
  struct EntityAssetChangeArgs;
  struct BuildCollisionDbArgs;
  struct InstancePropertiesChangeArgs;

  typedef Nocturnal::SmartPtr< Luna::Entity > EntityPtr;
  typedef std::vector< Luna::Entity* > V_EntityDumbPtr;

    struct TaskFinishedArgs;
  struct LunaViewTool;
  struct PhysicsTool;

  class RemoteScene : public RemoteEditor
  {
  public:
    RemoteScene( Editor* editor );
    virtual ~RemoteScene();

    SceneEditor* GetSceneEditor() { return m_SceneEditor; }
    virtual bool IsCameraEnabled() { return m_Camera; }

    void Enable( bool enable ) NOC_OVERRIDE;
    void Enable( bool enable, bool subset );
    void EnableCamera( bool enable );
    void EnablePhysics( bool enable );
    void EnableLighting( bool enable );

    void ResetScene();

    void TransformInstance( Luna::Transform* transform );

    void TransformCamera( const Math::Matrix4& transform );
    void TransformCamera();
    
    void AddLight( Luna::Light* light );
    void RemoveLight( Luna::Light* light );
    
    void AddLightingVolume( Luna::LightingVolume* volume );
    void RemoveLightingVolume( Luna::LightingVolume* volume );
    void SendLightingVolume( Luna::LightingVolume* volume );

    void AddPostProcessingVolume( Luna::PostProcessingVolume* volume );
    void RemovePostProcessingVolume( Luna::PostProcessingVolume* volume );
    void SendPostProcessingVolume( Luna::PostProcessingVolume* volume );
    void SetupPostProcessingVolume( Luna::PostProcessingVolume* volume );

    void AddVolume( Luna::Instance* instance, bool addListeners = true );
    void RemoveVolume( Luna::Instance* instance, bool removeListeners = true );
    void SendVolume( Luna::Instance* instance );

    void AddEntityInstance( Luna::Entity* entity, bool addListeners = true );
    void RemoveEntityInstance( Luna::Entity* entity, bool removeListeners = true );
    virtual void SendEntityInstance( Asset::Entity* entity ) NOC_OVERRIDE;

    void AddScene( Luna::Scene* scene );
    void RemoveScene( Luna::Scene* scene );

    void AddSceneListeners( Luna::Scene* scene );
    void RemoveSceneListeners( Luna::Scene* scene );

    void LoadOcclusion();
    void SendOcclusionId( Luna::Entity* entity );
    void SendSkyOcclusionId();

    void BuildCollisionDb(); 
    void SetCollisionDbValid( bool valid );

    void BuildLightingData();

    virtual void SendScene() NOC_OVERRIDE;
    void SendLevelData();
    virtual void ViewAsset( tuid assetId ) NOC_OVERRIDE;

    void CreateUIDArray( const V_EntityDumbPtr& entities, u8*& uids, u32& size );
    void AddMobysToSim( const V_EntityDumbPtr& mobys );
    void RemoveMobysFromSim( const V_EntityDumbPtr& mobys );
    void SyncMobys( const V_EntityDumbPtr& mobys );
    const UniqueID::S_TUID& GetSimulatedMobys();
    void SendSimulate();
    void SendSimulationData();

    S_SceneSmartPtr GetScenes();

    void SendLightingVolumes();
    void SendPostProcessingVolumes();
    void SendWeatherBlockingVolumes();

    void SetInstanceCollision( const Asset::Entity* entity ) NOC_OVERRIDE;
    void Screenshot( );

    //Functions to get lights linked to each other
    bool  GetLightsLinkedToLightingVolume(Luna::Instance* lightVolume,  OS_SelectableDumbPtr& lights);
    bool  GetLightingVolumesLinkedToLight(Luna::Instance* light,        OS_SelectableDumbPtr& lightingVolumes);

  protected:
    void SendLightingData( const Content::LightingJob* job, const Asset::Entity* entity, bool assignIndices = true );
    void RealtimeLightExistenceChanged( const RealtimeLightExistenceArgs& args );
    void TweakLightmapSettings( const LightmapTweakArgs& args );
    void TweakCubemapSettings( const CubemapTweakArgs& args );
    void CameraMoved( const CameraMovedArgs& args );
    void SetLightParams( const LightChangeArgs& args );
    void SceneNodeDeleting( const NodeChangeArgs& args );
    void SceneNodeCreated( const NodeChangeArgs& args );
    void EntityAssetChanging( const EntityAssetChangeArgs& args );
    void EntityAssetChanged( const EntityAssetChangeArgs& args );
    void EntityJointTransformsChanged( const Asset::JointTransformsChangedArgs& args );
    void InstancePropertiesChanged( const InstancePropertiesChangeArgs& args );
    void SetLightingVolumeParams( const LightingVolumeChangeArgs& args );
    void BuildFinished( const TaskFinishedArgs& args );
    void SceneAdded( const Luna::SceneChangeArgs& args );
    void SceneRemoved( const Luna::SceneChangeArgs& args );
    void SceneLoaded( const Luna::LoadArgs& args );
    void SceneGraphEvaluated( const SceneGraphEvaluatedArgs& args );
    void LoadLightingData( RPC::LoadLightingDataParam& param );

    void SetPostEffectsLightScatteringAttributeParams( const PostEffectsLightScatteringAttributeChangeArgs& args );
    void SetPostEffectsColorCorrectionAttributeParams( const PostEffectsColorCorrectionAttributeChangeArgs& args );
    void SetPostEffectsCurveControlAttributeParams( const PostEffectsCurveControlAttributeChangeArgs& args );
    void SetPostEffectsDepthOfFieldAttributeParams( const PostEffectsDepthOfFieldAttributeChangeArgs& args );
    void SetPostEffectsGraphShaderAttributeParams( const PostEffectsGraphShaderAttributeChangeArgs& args );
    void SetPostEffectsMotionBlurAttributeParams( const PostEffectsMotionBlurAttributeChangeArgs& args );
    void SetPostEffectsFilmGrainAttributeParams( const PostEffectsFilmGrainAttributeChangeArgs& args );
    void SetPostEffectsCausticsAttributeParams( const PostEffectsCausticsAttributeChangeArgs& args );
    void SetPostEffectsBloomAttributeParams( const PostEffectsBloomAttributeChangeArgs& args );
    void SetPostEffectsColorAttributeParams( const PostEffectsColorAttributeChangeArgs& args );
    void SetPostEffectsBlurAttributeParams( const PostEffectsBlurAttributeChangeArgs& args );
    void SetPostEffectsFogAttributeParams( const PostEffectsFogAttributeChangeArgs& args );
    void SetPostEffectsHDRAttributeParams( const PostEffectsHDRAttributeChangeArgs& args );
 
    virtual void SetLinkedBakedLights( Luna::Instance* light );

  public:
    void SetPostEffectsCustomMapAttributeParams( const PostEffectsCustomMapAttributeChangeArgs& args );

  protected:
    SceneEditor*         m_SceneEditor;

    RPC::ILunaViewHost*   m_LunaViewHost;
    LunaViewTool*         m_LunaViewTool;

    RPC::IPhysicsHost*    m_PhysicsHost;
    PhysicsTool*          m_PhysicsTool;

    bool                  m_Camera;
    bool                  m_SyncLighting;

    u16                   m_SkyOcclId;
    UniqueID::HM_TUIDU32  m_OcclusionMap;

    bool                  m_SubsetScene;

    // this MUST be set through SetCollisionDbValid()
    bool                  m_CollisionDbValid;

    bool                  m_Simulate;
    UniqueID::S_TUID        m_SimulatedMobys;
  };
}
