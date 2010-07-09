#pragma once

#include "Luna/API.h"

#include "AssetCollection.h"
#include "DependencyCollection.h"

#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Serializers.h"
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

    bool IsCollectionNameAvailable( AssetCollection* collection, const tstring& name, tstring& errors );
    void GetUniqueName( tstring& name, const tchar* baseName = TXT( "New Collection" ) );

    AssetCollection* FindCollection( const tstring& name ) const;
    bool AddCollection( AssetCollection* collection );

    bool RenameCollection( AssetCollection* collection, const tstring& name, tstring& errors );
    void DeleteCollection( AssetCollection* collection );

    AssetCollection* OpenCollection( const tstring& path, bool copyLocal = false );
    void CloseCollection( AssetCollection* collection );

    AssetCollection* ImportCollection( const tstring& path );
    bool ImportIntoStaticCollection( AssetCollection* collection, const tstring& path );

    bool SaveCollection( AssetCollection* collection, const tstring& path = TXT( "" ) );
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
    std::set< u64 > m_DirtyCollectionHashes;

    // This is what actually gets saved to disc
    std::set< tstring > m_CollectionFilePaths;

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