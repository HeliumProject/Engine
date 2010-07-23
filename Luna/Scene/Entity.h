#pragma once

#include "Instance.h"

#include "Pipeline/Asset/Classes/EntityInstance.h"

namespace Luna
{
  struct LightmapTweakArgs
  {
    class Entity* m_Entity;
    u32      m_LightmapSetIndex;

    LightmapTweakArgs( class Luna::Entity* entity, u32 lightmapSetIndex )
      : m_Entity( entity )
      , m_LightmapSetIndex( lightmapSetIndex )
    {}
  };
  typedef Nocturnal::Signature< void, const LightmapTweakArgs& > LightmapTweakSignature;

  struct CubemapTweakArgs
  {
    class Entity* m_Entity;

    CubemapTweakArgs( class Luna::Entity* entity )
      : m_Entity( entity )
    {}
  };
  typedef Nocturnal::Signature< void, const CubemapTweakArgs& > CubemapTweakSignature;

  struct EntityAssetChangeArgs
  {
    class Luna::Entity* m_Entity;
    Nocturnal::Path m_OldPath;
    Nocturnal::Path m_NewPath;
    
    EntityAssetChangeArgs( class Luna::Entity* entity, const Nocturnal::Path& oldPath, const Nocturnal::Path& newPath )
      : m_Entity( entity )
      , m_OldPath( oldPath )
      , m_NewPath( newPath )
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
    class EntityAssetSet* m_ClassSet;
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
    Entity(Luna::Scene* s, Asset::EntityInstance* entity);
    virtual ~Entity();

    void ConstructorInit();

    virtual tstring GenerateName() const NOC_OVERRIDE;
    virtual tstring GetApplicationTypeName() const NOC_OVERRIDE;
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
    class Luna::EntityAssetSet* GetClassSet();
    const class Luna::EntityAssetSet* GetClassSet() const;
    void SetClassSet( class Luna::EntityAssetSet* artClass );

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

    virtual bool ValidatePanel(const tstring& name) NOC_OVERRIDE;

  private:
    static void CreatePanel( CreatePanelArgs& args );


    //
    // UI
    //

  public:
    tstring GetEntityAssetPath() const;
    void SetEntityAssetPath( const tstring& entityClass );

    //
    // Callbacks
    //
  protected:
    void OnComponentAdded( const Component::ComponentCollectionChanged& args );
    void OnComponentRemoved( const Component::ComponentCollectionChanged& args );

    //
    // Events
    //
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
