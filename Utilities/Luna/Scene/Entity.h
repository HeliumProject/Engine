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
    File::ReferencePtr m_OldRef;
    File::ReferencePtr m_NewRef;
    
    EntityAssetChangeArgs( Luna::Entity* entity, File::ReferencePtr oldRef, File::ReferencePtr newRef )
      : m_Entity( entity )
      , m_OldRef( oldRef )
      , m_NewRef( newRef )
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

    std::string GetAssetTypeName() const;
    void SetAssetTypeName( const std::string& type );

  public:

    //
    // Callbacks
    //
  protected:
    void OnInstanceCollisionAttributeModified( const Reflect::ElementChangeArgs& args );
    void OnAttributeAdded( const Attribute::AttributeCollectionChanged& args );
    void OnAttributeRemoved( const Attribute::AttributeCollectionChanged& args );

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
