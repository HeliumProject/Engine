#pragma once

#include "API.h"

#include "AssetCollection.h"
#include "DependencyLoader.h"

namespace Luna
{

  ///////////////////////////////////////////////////////////////////////////////
  /// class DependencyCollection
  ///////////////////////////////////////////////////////////////////////////////
  class DependencyCollection : public AssetCollection
  {
  public:
    DependencyCollection();
    DependencyCollection( const std::string& name, const u32 flags = AssetCollectionFlags::Default, const bool reverse = false );
    ~DependencyCollection();

    virtual void InitializeCollection() NOC_OVERRIDE;
    virtual void CleanupCollection() NOC_OVERRIDE;

    virtual std::string GetDisplayName() const;

    void SetRootID( const tuid rootID );
    const tuid& GetRootID() const { return m_RootID; }

    std::string GetAssetName() const;

    virtual void SetFlags( const u32 flags );

    bool IsReverse() const { return m_IsReverse; }
    void SetReverse( const bool reverse );

    u32 GetRecursionDepthForLoad() const;

    void LoadDependencies( bool threaded = true );

    bool IsLoading() const { return m_IsLoading; }
    void IsLoading( bool isLoading );

  private:
    void OnPreferencesChanged( const Reflect::ElementChangeArgs& args );

  public:
    REFLECT_DECLARE_CLASS( DependencyCollection, AssetCollection );
    static void EnumerateClass( Reflect::Compositor<DependencyCollection>& comp );
    virtual void PreDeserialize() NOC_OVERRIDE;
    virtual void PostDeserialize() NOC_OVERRIDE;

  private:
    tuid m_RootID;
    bool m_IsReverse;

    // Cache
    Asset::AssetFilePtr m_AssetFile;

    bool m_IsLoading;
    DependencyLoader m_DependencyLoader;
  };
  typedef Nocturnal::SmartPtr< DependencyCollection > DependencyCollectionPtr;

}
