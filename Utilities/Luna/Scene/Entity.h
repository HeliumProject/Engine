#pragma once

#include "Instance.h"

#include "Asset/Entity.h"

namespace Luna
{
  class EntityType;
  class EntityAssetSet;
  class Entity;

  struct LightmapTweakArgs
  {
    Luna::Entity* m_Entity;
    u32      m_LightmapSetIndex;

    LightmapTweakArgs( Luna::Entity* entity, u32 lightmapSetIndex )
      : m_Entity( entity )
      , m_LightmapSetIndex( lightmapSetIndex )
    {}
  };
  typedef Nocturnal::Signature< void, const LightmapTweakArgs& > LightmapTweakSignature;

  struct CubemapTweakArgs
  {
    Luna::Entity* m_Entity;

    CubemapTweakArgs( Luna::Entity* entity )
      : m_Entity( entity )
    {}
  };
  typedef Nocturnal::Signature< void, const CubemapTweakArgs& > CubemapTweakSignature;

  struct EntityAssetChangeArgs
  {
    Luna::Entity* m_Entity;
    tuid m_OldId;
    tuid m_NewId;
    
    EntityAssetChangeArgs( Luna::Entity* entity, tuid oldId, tuid newId )
      : m_Entity( entity )
      , m_OldId( oldId )
      , m_NewId( newId )
    {}
  };
  typedef Nocturnal::Signature< void, const EntityAssetChangeArgs& > EntityAssetChangeSignature;

  /////////////////////////////////////////////////////////////////////////////
  // Luna's wrapper for an entity instance.
  //
  class LUNA_SCENE_API Entity : public Luna::Instance
  {
    //
    // Members
    //

  protected:
    Luna::EntityAssetSet* m_ClassSet;
    mutable Luna::Scene* m_NestedSceneArt;
    mutable Luna::Scene* m_NestedSceneCollision;
    mutable Luna::Scene* m_NestedScenePathfinding;


    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::Entity, Luna::Instance );
    static void InitializeType();
    static void CleanupType();


    //
    // Member functions
    //

    Entity(Luna::Scene* s);
    Entity(Luna::Scene* s, Asset::Entity* entity);
    virtual ~Entity();

    void ConstructorInit();

    virtual std::string GenerateName() const NOC_OVERRIDE;
    virtual std::string GetApplicationTypeName() const NOC_OVERRIDE;
    virtual SceneNodeTypePtr CreateNodeType( Luna::Scene* scene ) const NOC_OVERRIDE;

    // retrieve the nested scene from the scene manager
    Luna::Scene* GetNestedScene(GeometryMode mode, bool load_on_demand = true) const;


    //
    // Should we show the pointer
    //

    bool IsPointerVisible() const;
    void SetPointerVisible(bool visible);


    //
    // Should we show our bounds
    //

    bool IsBoundsVisible() const;
    void SetBoundsVisible(bool visible);


    //
    // Should we show geometry while we are drawing?
    //

    bool IsGeometryVisible() const;
    void SetGeometryVisible(bool visible);


    //
    // Sets help us organize groups of entities together
    //

    // class set is the object common to all entities with common class within the same type
    Luna::EntityAssetSet* GetClassSet();
    const Luna::EntityAssetSet* GetClassSet() const;
    void SetClassSet(Luna::EntityAssetSet* artClass);
    virtual void CheckSets() NOC_OVERRIDE;
    virtual void ReleaseSets() NOC_OVERRIDE;

    // match by entity class
    virtual void FindSimilar(V_HierarchyNodeDumbPtr& similar) const NOC_OVERRIDE;
    virtual bool IsSimilar(const HierarchyNodePtr& node) const NOC_OVERRIDE;

    // get some useful info
    virtual std::string GetDescription() const;

    // gather some manifest data
    virtual void PopulateManifest( Asset::SceneManifest* manifest ) const NOC_OVERRIDE;


    //
    // Evaluate and Render
    //

  public:
    virtual void Evaluate(GraphDirection direction) NOC_OVERRIDE;

    virtual void Render( RenderVisitor* render ) NOC_OVERRIDE;
    static void DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
    static void DrawBounds( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

    virtual bool Pick( PickVisitor* pick ) NOC_OVERRIDE;

    virtual bool ValidatePanel(const std::string& name) NOC_OVERRIDE;

  private:
    static void CreatePanel( CreatePanelArgs& args );


    //
    // UI
    //

  public:
    std::string GetEntityAssetPath() const;
    void SetEntityAssetPath( const std::string& entityClass );

    std::string GetEngineTypeName() const;
    void SetEngineTypeName( const std::string& type );


    //
    // Lighting UI
    //

  public:
    bool GetIndirectLightingEnabled() const;
    void SetIndirectLightingEnabled( bool enabled );

    i32 GetLightmapSetting() const;
    void SetLightmapSetting( i32 setting );

    float GetMaxIndirectDistance() const;
    void  SetMaxIndirectDistance( float distance );

    float GetIndirectMultiplier() const;
    void  SetIndirectMultiplier( float multiplier );

    Math::Color3 GetShadowColor() const;
    void SetShadowColor( Math::Color3 color );

    u32   GetRuntimeLM1Format() const;
    void  SetRuntimeLM1Format( u32 format );

    u32   GetRuntimeLM1Size() const;
    void  SetRuntimeLM1Size( u32 size );

    u32   GetRuntimeLM2Format() const;
    void  SetRuntimeLM2Format( u32 format );

    u32   GetRuntimeLM2Size() const;
    void  SetRuntimeLM2Size( u32 size );

    std::string GetCubeMapPath() const;
    void SetCubeMapPath( const std::string& cubeMap );

    f32 GetCubeMapIntensity() const;
    void  SetCubeMapIntensity( f32 intensity );

    bool IsSpecialized() const; 
    void SetSpecialized(bool specialized ); 

    f32 GetTexelsPerMeter() const;

    //have to do this instead of use a default param, because of the Inspect template interfaces
    void SetTexelsPerMeter( f32 tpm );
    void SetTexelsPerMeterAndRuntime( f32 tpm, bool setRuntimeLM  );

    bool GetSetRuntimeLightmap() const;
    void SetSetRuntimeLightmap( bool set );

    bool GetBiasNormals() const;
    void SetBiasNormals( bool enabled );

    //
    // Callbacks
    //
  protected:
    void OnInstanceCollisionAttributeModified( const Reflect::ElementChangeArgs& args );
    void OnAttributeAdded( const Attribute::AttributeCollectionChanged& args );
    void OnAttributeRemoved( const Attribute::AttributeCollectionChanged& args );
    void OnShaderGroupAttributeModified( const Reflect::ElementChangeArgs& args );
    void TweakShaderGroup();

    //
    // Events
    //
  protected:
    LightmapTweakSignature::Event m_LightmapTweaked;
  public:
    void AddLightmapTweakedListener( const LightmapTweakSignature::Delegate& listener )
    {
      m_LightmapTweaked.Add( listener );
    }

    void RemoveLightmapTweakedListener( const LightmapTweakSignature::Delegate& listener )
    {
      m_LightmapTweaked.Remove( listener );
    }

  protected:
    CubemapTweakSignature::Event m_CubemapTweaked;
  public:
    void AddCubemapTweakedListener( const CubemapTweakSignature::Delegate& listener )
    {
      m_CubemapTweaked.Add( listener );
    }

    void RemoveCubemapTweakedListener( const CubemapTweakSignature::Delegate& listener )
    {
      m_CubemapTweaked.Remove( listener );
    }

  protected:
    EntityAssetChangeSignature::Event m_ClassChanging;
  public:
    void AddClassChangingListener( const EntityAssetChangeSignature::Delegate& listener )
    {
      m_ClassChanging.Add( listener );
    }

    void RemoveClassChangingListener( const EntityAssetChangeSignature::Delegate& listener )
    {
      m_ClassChanging.Remove( listener );
    }

  protected:
    EntityAssetChangeSignature::Event m_ClassChanged;
  public:
    void AddClassChangedListener( const EntityAssetChangeSignature::Delegate& listener )
    {
      m_ClassChanged.Add( listener );
    }

    void RemoveClassChangedListener( const EntityAssetChangeSignature::Delegate& listener )
    {
      m_ClassChanged.Remove( listener );
    }
  };

  typedef Nocturnal::SmartPtr<Luna::Entity> EntityPtr;
  typedef std::vector<Luna::Entity*> V_EntityDumbPtr;
}
