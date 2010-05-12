#pragma once

#include <hash_map>
#include <set>

#include "API.h"
#include "AssetFlags.h"
#include "AssetVersion.h"
#include "EngineType.h"

#include "TUID/TUID.h"
#include "Common/Container/OrderedSet.h"
#include "Attribute/Attribute.h" 
#include "Attribute/AttributeCollection.h" 

#define REGEX_LEVEL_DIR "levels\\/(?:test\\/){0,1}([0-9a-zA-Z \\-_]+)?"

namespace Asset
{
  class AssetClass;
  typedef Nocturnal::SmartPtr< AssetClass > AssetClassPtr;
  typedef std::vector< AssetClassPtr > V_AssetClass;

  typedef std::set<AssetClassPtr> S_AssetClass;
  typedef Nocturnal::OrderedSet< Asset::AssetClassPtr > OS_AssetClass;

  // 
  // Events and args for when an asset is classified.
  // 
  struct EngineTypeChangeArgs
  {
    const AssetClass* m_Asset;
    EngineType m_PreviousEngineType;
    
    EngineTypeChangeArgs( const AssetClass* asset, EngineType previousType )
      : m_Asset( asset )
      , m_PreviousEngineType( previousType )
    {
    }
  };
  typedef Nocturnal::Signature< void, const EngineTypeChangeArgs& > EngineTypeChangeSignature;


  /////////////////////////////////////////////////////////////////////////////////
  //
  // The Definition of an Asset Class
  //

  class ASSET_API AssetClass NOC_ABSTRACT : public Attribute::AttributeCollection
  {
    //
    // Member Data
    //

  public:
    // asset class's id in the file manager
    tuid m_AssetClassID;

    // description of this asset
    std::string m_Description;

  private:
    // the folder for our built data
    mutable std::string m_ClassBuiltDirectory;

    // the folder for our data?
    mutable std::string m_ClassDataDirectory;

    // unqiue name
    mutable std::string m_FullName;

    // non-unqiue name
    mutable std::string m_ShortName;

    // the engine we have been classified as (invalidates upon changing)
    mutable EngineType m_EngineType;

    //
    // RTTI
    //

  public:
    REFLECT_DECLARE_ABSTRACT(AssetClass, AttributeCollection);

    static void EnumerateClass( Reflect::Compositor<AssetClass>& comp );

    //
    // Member functions
    //

  public:
    AssetClass();

    // useful paths
    tuid GetFileID() const
    {
      return m_AssetClassID;
    }
    std::string GetFilePath() const;
    virtual std::string GetBuiltDir() const;
    std::string GetDataDir() const;

    // x:\rcf\assets\entities\fruitBasketFromHell\appleSuccubus.entity.irb -> entities\fruitBasketFromHell\appleSuccubus.entity.irb
    const std::string& GetFullName() const;

    // x:\rcf\assets\entities\fruitBasketFromHell\appleSuccubus.entity.irb -> appleSuccubus
    const std::string& GetShortName() const;

    // Only for use with levels, movies and cinematics
    // x:\rcf\assets\entities\fruitBasketFromHell\appleSuccubus.entity.irb -> fruitBasketFromHell\appleSuccubus
    static std::string GetQualifiedName( const AssetClass* assetClass );

    // the classified type for this asset class
    virtual EngineType GetEngineType() const;
    std::string GetEngineTypeName() const;

    static void GetEngineTypeName( const Asset::AssetClass* assetClass, std::string& engineTypeName );

    // EngineTypeInfo funcitons
    static const std::string& GetEngineTypeBuilderDLL( const EngineType engineType );
    static const std::string& GetEngineTypeIcon( const EngineType engineType );

    // configure this instance as the default instance of the derived class
    virtual void MakeDefault() {}

    // we were changed by somebody, reclassify
    virtual void AttributeChanged( const Attribute::AttributeBase* attr = NULL ) NOC_OVERRIDE;

    // write to the location on disk backed by the file manager id
    virtual void Serialize( const AssetVersionPtr &version = new AssetVersion () );

    // add to or set an attribute in the collection
    virtual void SetAttribute( const Attribute::AttributePtr& attr, bool validate = true ) NOC_OVERRIDE;

    // remove attribute from a slot
    virtual void RemoveAttribute( i32 typeID ) NOC_OVERRIDE;

    // Returns true by default. Override to specify more stringent requirements on the asset.
    virtual bool ValidateClass( std::string& error ) const;

    // validate the incoming attribute as ok to consume
    virtual bool ValidateCompatible( const Attribute::AttributePtr &attr, std::string& error ) const NOC_OVERRIDE;

    // callback when this AssetClass has finished loading off disk
    virtual void LoadFinished();

    // can this asset type be built
    virtual bool IsBuildable() const;

    // can this asset type be viewed
    virtual bool IsViewable() const;

    // copy this asset and its attributes into the destination
    virtual void CopyTo(const Reflect::ElementPtr& destination) NOC_OVERRIDE;

    //
    // Caching API, this holds instances in memory to keep the disk from thrashing during large builds
    //

    // init and cleanup
    static void InitializeCache();
    static void CleanupCache();

    // invalidate a specific entry
    static void InvalidateCache( tuid assetClassID );

    // invalidate the entire cache
    static void InvalidateCache();

    static tuid FindAuthoritativeTuid( tuid assetClassID );
    static AssetClassPtr FindAssetClass( tuid assetClassID, bool useCache = true );

    template <class T>
    static Nocturnal::SmartPtr<T> GetAssetClass( tuid assetClassID, bool useCache = true )
    {
      return Reflect::TryCast<T>( FindAssetClass( assetClassID, useCache ) );
    }

    template <class T>
    static Nocturnal::SmartPtr<T> GetCachedAssetClass( tuid assetClassID )
    {
      return Reflect::TryCast<T>( FindAssetClass( assetClassID, true ) );
    }

    template <class T>
    static Nocturnal::SmartPtr<T> GetSavedAssetClass( tuid assetClassID )
    {
      return Reflect::TryCast<T>( FindAssetClass( assetClassID, false ) );
    }


    //
    // Classify API
    //

  private:
    void Classify() const;
    static EngineType Classify( const AssetClass* assetClass );

    AssetClassPtr NearestEngineType( const Attribute::AttributeBase* addedAttribute = NULL );

    // 
    // Listeners
    // 
  private:
    mutable EngineTypeChangeSignature::Event m_EngineTypeChanged;
  public:
    void AddEngineTypeChangedListener( const EngineTypeChangeSignature::Delegate& listener )
    {
      m_EngineTypeChanged.Add( listener );
    }
    void RemoveEngineTypeChangedListener( const EngineTypeChangeSignature::Delegate& listener )
    {
      m_EngineTypeChanged.Remove( listener );
    }
  };
}