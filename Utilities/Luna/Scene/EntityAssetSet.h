#pragma once

#include "InstanceSet.h"

#include "TUID/TUID.h"
#include "Asset/EntityAsset.h"
#include "Asset/EntityManifest.h"

namespace Luna
{
  class Entity;
  class EntityAssetSet;
  class EntityType;
  class PrimitiveCube;
  struct SharedFileChangeArgs;

  struct EntityAssetSetChangeArgs
  {
    Luna::EntityAssetSet* m_EntityAssetSet;

    EntityAssetSetChangeArgs( Luna::EntityAssetSet* entityClassSet )
      : m_EntityAssetSet( entityClassSet )
    {
    }
  };
  typedef Nocturnal::Signature< void, const EntityAssetSetChangeArgs& > EntityAssetSetChangeSignature;

  class EntityAssetSet : public Luna::InstanceSet
  {
    //
    // Members
    //

  protected:
    // file resolver id
    tuid m_ClassID;
    bool m_ClassMissing;

    // loaded class data, not shared via cache
    Asset::EntityAssetPtr m_Class;

    // manifest information exported from the content
    Asset::EntityManifestPtr m_Manifest;

    // the master file from the content attribute
    std::string m_ArtFile;

    // shape to render with
    Luna::Primitive* m_Shape;

    // the name of the class, derived from the path
    std::string m_Name;

    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::EntityAssetSet, Luna::InstanceSet );
    static void InitializeType();
    static void CleanupType();


    //
    // Implementation
    //

  public:
    EntityAssetSet(Luna::EntityType* type, tuid id);

    virtual ~EntityAssetSet();

    tuid GetEntityAssetID() const
    {
      return m_ClassID;
    }

    Asset::EntityAsset* GetEntityAsset() const
    {
      return m_Class;
    }

    const std::string& GetContentFile() const
    {
      return m_ArtFile;
    }

    Luna::Primitive* GetShape()
    {
      return m_Shape;
    }

    const Luna::Primitive* GetShape() const
    {
      return m_Shape;
    }

    virtual const std::string& GetName() const NOC_OVERRIDE
    {
      return m_Name;
    }

    virtual void Create();
    virtual void Delete();

    virtual void AddInstance(Luna::Instance* i) NOC_OVERRIDE;
    virtual void RemoveInstance(Luna::Instance* i) NOC_OVERRIDE;

    virtual void LoadAssetClass();

  private:
    void FileChanged( const SharedFileChangeArgs& args );

  private:
    EntityAssetSetChangeSignature::Event m_ClassLoaded;
  public:
    void AddClassLoadedListener( const EntityAssetSetChangeSignature::Delegate& listener )
    {
      m_ClassLoaded.Add( listener );
    }
    void RemoveClassLoadedListener( const EntityAssetSetChangeSignature::Delegate& listener )
    {
      m_ClassLoaded.Remove( listener );
    }
  };
}
