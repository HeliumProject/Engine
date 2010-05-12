#pragma once

#include "API.h"
#include "Content/Instance.h"
#include "Asset/EntityAsset.h"
#include "Common/Automation/Event.h"

namespace Asset
{
  class ShaderGroup;
  typedef Nocturnal::SmartPtr< ShaderGroup > ShaderGroupPtr;
  typedef std::vector< ShaderGroupPtr > V_ShaderGroupSmartPtr;

  typedef Nocturnal::Signature<Asset::EntityAssetPtr, tuid> EntityAssetFinder;

  class Entity;

  struct JointTransformsChangedArgs
  {
    JointTransformsChangedArgs( Entity* entity )
      : m_Entity( entity )
    {

    }

    Entity* m_Entity;
  };
  typedef Nocturnal::Signature< void, const JointTransformsChangedArgs& > JointTransformsChangedSignature;

  class ASSET_API Entity : public Content::Instance
  {
  private:
    REFLECT_DECLARE_CLASS(Entity, Instance);
    static void EnumerateClass( Reflect::Compositor<Entity>& comp );

  public:
    Entity ();
    Entity (const UniqueID::TUID& id);

    Entity (const tuid& classID);
    Entity (const UniqueID::TUID& id, const tuid& classID);

    // manage class id
    tuid GetEntityAssetID() const;
    void SetEntityAssetID( const tuid& newID );

    // get the path to the class
    const std::string& GetEntityAssetPath();

    // helper to find the entity class
    Asset::EntityAssetPtr GetEntityAsset() const;

    // do we want to use a cached entity class instance?
    void UseCachedClass(bool useCache);

    //
    // AttributeCollection overrides
    //

    virtual bool ValidatePersistent( const Attribute::AttributePtr& attr ) const NOC_OVERRIDE;
    virtual const Attribute::AttributePtr& GetAttribute( i32 typeID ) const NOC_OVERRIDE;
    virtual void SetAttribute( const Attribute::AttributePtr& attr, bool validate = true ) NOC_OVERRIDE;

  private:
    // id of the EntityAsset this instance was created from
    tuid m_ClassID; 

    // cached path to the class
    std::string m_ClassPath;

    // class finder delegate
    EntityAssetFinder::Delegate m_ClassFinder;

    // WTF is this doing here?  seems like it should be in an attribute -Geoff
    JointTransformsChangedSignature::Event m_JointTransformsChanged;

  public:
    // draw bound in editor
    bool m_ShowPointer;

    // draw bound in editor
    bool m_ShowBounds;

    // show geometry in editor
    bool m_ShowGeometry;
  };

  typedef Nocturnal::SmartPtr<Entity> EntityPtr;
  typedef std::vector<EntityPtr> V_Entity;
}