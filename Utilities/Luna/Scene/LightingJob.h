#pragma once

#include "Instance.h"
#include "Content/LightingJob.h"
#include "Core/Enumerator.h"
#include "Undo/BatchCommand.h"

namespace Luna
{
  class CubeMapProbe;
  class Entity;
  class Light;
  class LightingEnvironment;
  class LightingJob;
  class LightingVolume;
  class SceneNode;
  class Zone;
  struct SceneChangeArgs;
  struct NodeChangeArgs;

  // Event and arguments for when membership of a lighting job changes.
  struct LightingJobMemberChangeArgs
  {
    Luna::LightingJob* m_LightingJob;
    Luna::SceneNode* m_Member;

    LightingJobMemberChangeArgs( Luna::LightingJob* job, Luna::SceneNode* member )
    : m_LightingJob( job )
    , m_Member( member )
    {
    }
  };

  typedef Nocturnal::Signature< void, const LightingJobMemberChangeArgs& > LightingJobMemberChangeSignature;
  
  /////////////////////////////////////////////////////////////////////////////
  // Lighting jobs group together a bunch of lights, render targets, and shadow
  // casters so that lighting data can be calculated for the geometry.  The members
  // of a lighting job are allowed to span multiple scenes.
  // 
  class LightingJob : public Luna::SceneNode
  {
  private:
    typedef std::pair< tuid, UniqueID::TUID > P_TuidToUid;
    typedef std::map< P_TuidToUid, Undo::BatchCommandPtr > M_NodeToRemoveCmd;

  private:
    LightingJobMemberChangeSignature::Event m_LightAdded;
    LightingJobMemberChangeSignature::Event m_LightRemoved;
    LightingJobMemberChangeSignature::Event m_ShadowCasterAdded;
    LightingJobMemberChangeSignature::Event m_ShadowCasterRemoved;
    LightingJobMemberChangeSignature::Event m_RenderTargetAdded;
    LightingJobMemberChangeSignature::Event m_RenderTargetRemoved;
    LightingJobMemberChangeSignature::Event m_VolumeAdded;
    LightingJobMemberChangeSignature::Event m_VolumeRemoved;
    LightingJobMemberChangeSignature::Event m_ProbeAdded;
    LightingJobMemberChangeSignature::Event m_ProbeRemoved;
    LightingJobMemberChangeSignature::Event m_LightingEnvironmentAdded;
    LightingJobMemberChangeSignature::Event m_LightingEnvironmentRemoved;
    LightingJobMemberChangeSignature::Event m_ZoneAdded;
    LightingJobMemberChangeSignature::Event m_ZoneRemoved;      

    M_NodeToRemoveCmd m_RemoveCmds;

    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::LightingJob, Luna::SceneNode );
    static void InitializeType();
    static void CleanupType();
    static bool IsLightable( Luna::SceneNode* instance );

    //
    // Member functions
    //

    LightingJob( Luna::Scene* scene );
    LightingJob( Luna::Scene* scene, Content::LightingJob* light );
    virtual ~LightingJob();
    
  private:
    void Init();
    bool Add( const tuid& zoneID, Luna::SceneNode* node, LightingJobMemberChangeSignature::Event& evt, UniqueID::S_TUID Content::LightingJobZoneItems::* pointerToMember );
    bool Remove( const tuid& zoneID, Luna::SceneNode* node, LightingJobMemberChangeSignature::Event& evt, UniqueID::S_TUID Content::LightingJobZoneItems::* pointerToMember );
    const UniqueID::S_TUID& Get( const tuid& zoneID, UniqueID::S_TUID Content::LightingJobZoneItems::* pointerToMember ) const;    

  public:
    virtual i32 GetImageIndex() const NOC_OVERRIDE;
    virtual std::string GetApplicationTypeName() const NOC_OVERRIDE;

    void GetZoneIDs( S_tuid& outZones ) const;

    bool AddLight( const tuid& zoneID, Luna::Light* light );
    bool RemoveLight( const tuid& zoneID, Luna::Light* light );
    const UniqueID::S_TUID& GetLights( const tuid& zoneID ) const;

    bool AddShadowCaster( const tuid& zoneID, Luna::Entity* instance );
    bool RemoveShadowCaster( const tuid& zoneID, Luna::Entity* instance );
    const UniqueID::S_TUID& GetShadowCasters( const tuid& zoneID ) const;

    bool AddRenderTarget( const tuid& zoneID, Luna::Entity* instance );
    bool RemoveRenderTarget( const tuid& zoneID, Luna::Entity* instance );
    const UniqueID::S_TUID& GetRenderTargets( const tuid& zoneID ) const;

    bool AddCubeMapProbe( const tuid& zoneID, Luna::CubeMapProbe* probe );
    bool RemoveCubeMapProbe( const tuid& zoneID, Luna::CubeMapProbe* probe );
    const UniqueID::S_TUID& GetCubeMapProbes( const tuid& zoneID ) const;

    bool AddLightingEnvironment( const tuid& zoneID, Luna::LightingEnvironment* env );
    bool RemoveLightingEnvironment( const tuid& zoneID, Luna::LightingEnvironment* env );
    const UniqueID::S_TUID& GetLightingEnvironments( const tuid& zoneID ) const;

    bool AddZone( const tuid& zoneID, Zone* env );
    bool RemoveZone( const tuid& zoneID, Zone* env );
    const UniqueID::S_TUID& GetZones( const tuid& zoneID ) const;

    bool GetFinalGather() const;
    void SetFinalGather( bool enable );
    
    u32 GetFinalGatherAccuracy() const;
    void SetFinalGatherAccuracy( u32 accuracy );

    bool GetGlobillum() const;
    void SetGlobillum( bool enable );

    f32 GetRContrast() const;
    f32 GetGContrast() const;
    f32 GetBContrast() const;
    f32 GetAContrast() const;

    void SetRContrast( f32 value );
    void SetGContrast( f32 value );
    void SetBContrast( f32 value );
    void SetAContrast( f32 value );

    i32 GetMinSamples() const;
    void SetMinSamples( i32 accuracy );

    i32 GetMaxSamples() const;
    void SetMaxSamples( i32 accuracy );

    f32 GetFinalGatherMinRad() const;
    void SetFinalGatherMinRad( f32 value );
    
    f32 GetFinalGatherMaxRad() const;
    void SetFinalGatherMaxRad( f32 value );

    i32 GetRenderQuality() const;
    void SetRenderQuality( i32 quality );

    Math::Color3 GetFinalGatherColor() const;
    void SetFinalGatherColor( Math::Color3 color );

    float GetFinalGatherScale() const;
    void SetFinalGatherScale( float intensity );

    Math::Color3 GetGlobillumColor() const;
    void SetGlobillumColor( Math::Color3 color );

    float GetGlobillumScale() const;
    void SetGlobillumScale( float intensity );

    u32 GetGlobillumAccuracy() const;
    void SetGlobillumAccuracy( u32 accuracy );

    f32 GetGlobillumRadius() const;
    void SetGlobillumRadius( f32 value );

    u32 GetBSPDepth() const;
    void SetBSPDepth( u32 value );

    u32 GetBSPSize() const;
    void SetBSPSize( u32 value );

    bool GetEnabled() const;
    void SetEnabled( bool value );

    bool GetCalculateUnoccludedDir() const;
    void SetCalculateUnoccludedDir( bool value );


    

    virtual bool ValidatePanel(const std::string& name) NOC_OVERRIDE;
    static void CreatePanel( CreatePanelArgs& args );

  private:
    void AddSceneListeners( Luna::Scene* scene );
    void RemoveSceneListeners( Luna::Scene* scene );
    void RemoveAllSceneListeners();

  private:
    void SceneAdded( const SceneChangeArgs& args );
    void SceneRemoving( const SceneChangeArgs& args );
    void SceneNodeAdded( const NodeChangeArgs& args );
    void SceneNodeRemoved( const NodeChangeArgs& args );

  public:
    // Listeners
    void AddLightAddedListener( const LightingJobMemberChangeSignature::Delegate& listener )
    {
      m_LightAdded.Add( listener );
    }

    void RemoveLightAddedListener( const LightingJobMemberChangeSignature::Delegate& listener )
    {
      m_LightAdded.Remove( listener );
    }

    void AddLightRemovedListener( const LightingJobMemberChangeSignature::Delegate& listener )
    {
      m_LightRemoved.Add( listener );
    }

    void RemoveLightRemovedListener( const LightingJobMemberChangeSignature::Delegate& listener )
    {
      m_LightRemoved.Remove( listener );
    }

    void AddShadowCasterAddedListener( const LightingJobMemberChangeSignature::Delegate& listener )
    {
      m_ShadowCasterAdded.Add( listener );
    }

    void RemoveShadowCasterAddedListener( const LightingJobMemberChangeSignature::Delegate& listener )
    {
      m_ShadowCasterAdded.Remove( listener );
    }

    void AddShadowCasterRemovedListener( const LightingJobMemberChangeSignature::Delegate& listener )
    {
      m_ShadowCasterRemoved.Add( listener );
    }

    void RemoveShadowCasterRemovedListener( const LightingJobMemberChangeSignature::Delegate& listener )
    {
      m_ShadowCasterRemoved.Remove( listener );
    }

    void AddRenderTargetAddedListener( const LightingJobMemberChangeSignature::Delegate& listener )
    {
      m_RenderTargetAdded.Add( listener );
    }

    void RemoveRenderTargetAddedListener( const LightingJobMemberChangeSignature::Delegate& listener )
    {
      m_RenderTargetAdded.Remove( listener );
    }

    void AddRenderTargetRemovedListener( const LightingJobMemberChangeSignature::Delegate& listener )
    {
      m_RenderTargetRemoved.Add( listener );
    }

    void RemoveRenderTargetRemovedListener( const LightingJobMemberChangeSignature::Delegate& listener )
    {
      m_RenderTargetRemoved.Remove( listener );
    }

    void AddVolumeAddedListener( const LightingJobMemberChangeSignature::Delegate& listener )
    {
      m_VolumeAdded.Add( listener );
    }

    void RemoveVolumeAddedListener( const LightingJobMemberChangeSignature::Delegate& listener )
    {
      m_VolumeAdded.Remove( listener );
    }

    void AddVolumeRemovedListener( const LightingJobMemberChangeSignature::Delegate& listener )
    {
      m_VolumeRemoved.Add( listener );
    }

    void RemoveVolumeRemovedListener( const LightingJobMemberChangeSignature::Delegate& listener )
    {
      m_VolumeRemoved.Remove( listener );
    }

    void AddProbeAddedListener( const LightingJobMemberChangeSignature::Delegate& listener )
    {
      m_ProbeAdded.Add( listener );
    }

    void RemoveProbeAddedListener( const LightingJobMemberChangeSignature::Delegate& listener )
    {
      m_ProbeAdded.Remove( listener );
    }

    void AddProbeRemovedListener( const LightingJobMemberChangeSignature::Delegate& listener )
    {
      m_ProbeRemoved.Add( listener );
    }

    void RemoveProbeRemovedListener( const LightingJobMemberChangeSignature::Delegate& listener )
    {
      m_ProbeRemoved.Remove( listener );
    }

    void AddLightingEnvironmentAddedListener( const LightingJobMemberChangeSignature::Delegate& listener )
    {
      m_LightingEnvironmentAdded.Add( listener );
    }

    void RemoveLightingEnvironmentAddedListener( const LightingJobMemberChangeSignature::Delegate& listener )
    {
      m_LightingEnvironmentAdded.Remove( listener );
    }

    void AddLightingEnvironmentRemovedListener( const LightingJobMemberChangeSignature::Delegate& listener )
    {
      m_LightingEnvironmentRemoved.Add( listener );
    }

    void RemoveLightingEnvironmentRemovedListener( const LightingJobMemberChangeSignature::Delegate& listener )
    {
      m_LightingEnvironmentRemoved.Remove( listener );
    }

    void AddZoneAddedListener( const LightingJobMemberChangeSignature::Delegate& listener )
    {
      m_ZoneAdded.Add( listener );
    }

    void RemoveZoneAddedListener( const LightingJobMemberChangeSignature::Delegate& listener )
    {
      m_ZoneAdded.Remove( listener );
    }

    void AddZoneRemovedListener( const LightingJobMemberChangeSignature::Delegate& listener )
    {
      m_ZoneRemoved.Add( listener );
    }

    void RemoveZoneRemovedListener( const LightingJobMemberChangeSignature::Delegate& listener )
    {
      m_ZoneRemoved.Remove( listener );
    }
  };

  typedef Nocturnal::SmartPtr< Luna::LightingJob > LightingJobPtr;
  typedef std::vector< Luna::LightingJob* > V_LightingJobDumbPtr;
}
