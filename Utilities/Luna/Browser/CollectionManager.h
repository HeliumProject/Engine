#pragma once

#include "API.h"

#include "AssetCollection.h"
#include "DependencyCollection.h"

#include "Asset/AssetFile.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Reflect/Registry.h"
#include "Reflect/Serializers.h"
#include "Foundation/TUID.h"

namespace Luna
{
  struct CollectionManagerArgs
  {
    AssetCollection* m_Collection;

    CollectionManagerArgs( AssetCollection* collection )
      : m_Collection( collection )
    {
    }
  };
  typedef Nocturnal::Signature< void, const CollectionManagerArgs& > CollectionManagerSignature;

  ///////////////////////////////////////////////////////////////////////////////
  /// class CollectionManager
  ///////////////////////////////////////////////////////////////////////////////
  class CollectionManager : public Reflect::Element
  {
  public:
    CollectionManager();
    ~CollectionManager();

    const M_AssetCollections& GetCollections() const { return m_AssetCollections; }

    bool ContainsCollection( AssetCollection* collection ) const;

    bool IsCollectionNameAvailable( AssetCollection* collection, const std::string& name, std::string& errors );
    void GetUniqueName( std::string& name, const char* baseName = "New Collection" );

    AssetCollection* FindCollection( const std::string& name ) const;
    bool AddCollection( AssetCollection* collection );

    bool RenameCollection( AssetCollection* collection, const std::string& name, std::string& errors );
    void DeleteCollection( AssetCollection* collection );

    AssetCollection* OpenCollection( const std::string& path, bool copyLocal = false );
    void CloseCollection( AssetCollection* collection );

    AssetCollection* ImportCollection( const std::string& path );
    bool ImportIntoStaticCollection( AssetCollection* collection, const std::string& path );

    bool SaveCollection( AssetCollection* collection, const std::string& path = "" );
    void SaveAllCollections();

  public:
    REFLECT_DECLARE_CLASS( CollectionManager, Reflect::Element );
    static void EnumerateClass( Reflect::Compositor<CollectionManager>& comp );
    virtual void PreSerialize() NOC_OVERRIDE;
    virtual void PostDeserialize() NOC_OVERRIDE; 

  private:
    void Dirty( const AssetCollection* collection );

    // Collection management wrappers
    Nocturnal::Insert<M_AssetCollections>::Result AddCollectionToMap( AssetCollection* collection );
    bool RemoveCollectionFromMap( AssetCollection* collection );
    void ClearCollectionMap();

    void OnAssetCollectionsChanged( const Reflect::ElementChangeArgs& args );
    void OnCollectionModified( const Reflect::ElementChangeArgs& args );

  private:
    M_AssetCollections m_AssetCollections;
    S_u64 m_DirtyCollectionHashes;

    // This is what actually gets saved to disc
    S_string m_CollectionFilePaths;

  private:
    CollectionManagerSignature::Event m_CollectionAdded;
  public:
    void AddCollectionAddedListener( const CollectionManagerSignature::Delegate& listener )
    {
      m_CollectionAdded.Add( listener );
    }
    void RemoveCollectionAddedListener( const CollectionManagerSignature::Delegate& listener )
    {
      m_CollectionAdded.Remove( listener );
    }

  private:
    CollectionManagerSignature::Event m_CollectionRemoving;
  public:
    void AddCollectionRemovingListener( const CollectionManagerSignature::Delegate& listener )
    {
      m_CollectionRemoving.Add( listener );
    }
    void RemoveCollectionRemovingListener( const CollectionManagerSignature::Delegate& listener )
    {
      m_CollectionRemoving.Remove( listener );
    }

  private:
    CollectionManagerSignature::Event m_ClearAll;
  public:
    void AddClearAllListener( const CollectionManagerSignature::Delegate& listener )
    {
      m_ClearAll.Add( listener );
    }
    void RemoveClearAllListener( const CollectionManagerSignature::Delegate& listener )
    {
      m_ClearAll.Remove( listener );
    }
  };
  typedef Nocturnal::SmartPtr< CollectionManager > CollectionManagerPtr;
}