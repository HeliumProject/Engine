#pragma once

#include <hash_map>
#include <set>

#include "API.h"
#include "AssetFlags.h"
#include "AssetVersion.h"
#include "AssetType.h"

#include "TUID/TUID.h"
#include "Common/Container/OrderedSet.h"
#include "Common/File/Path.h"
#include "Attribute/Attribute.h" 
#include "Attribute/AttributeCollection.h" 
#include "File/Reference.h"

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
    struct AssetTypeChangeArgs
    {
        const AssetClass* m_Asset;
        AssetType m_PreviousAssetType;

        AssetTypeChangeArgs( const AssetClass* asset, AssetType previousType )
            : m_Asset( asset )
            , m_PreviousAssetType( previousType )
        {
        }
    };
    typedef Nocturnal::Signature< void, const AssetTypeChangeArgs& > AssetTypeChangeSignature;


    /////////////////////////////////////////////////////////////////////////////////
    //
    // The Definition of an Asset Class
    //

    class ASSET_API AssetClass NOC_ABSTRACT : public Attribute::AttributeCollection
    {
        //
        // Member Data
        //
    private:

        File::ReferencePtr m_AssetFileRef;

        // description of this asset
        std::string m_Description;

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

        static void SetBaseBuiltDirectory( const std::string& path )
        {
            s_BaseBuiltDirectory = path;
        }

        static AssetClassPtr LoadAssetClass( const std::string& path );
        static AssetClassPtr LoadAssetClass( File::Reference& fileRef )
        {
            return LoadAssetClass( fileRef.GetPath() );
        }

        template <class T>
        static Nocturnal::SmartPtr<T> LoadAssetClass( const std::string& path )
        {
            return Reflect::TryCast<T>( LoadAssetClass( path ) );
        }

        template <class T>
        static Nocturnal::SmartPtr<T> LoadAssetClass( File::Reference& fileRef )
        {
            return Reflect::TryCast<T>( LoadAssetClass( fileRef.GetPath() ) );
        }

        void SetAssetFileRef( File::Reference& fileRef )
        {
            if ( m_AssetFileRef )
            {
                delete m_AssetFileRef;
            }
            
            m_AssetFileRef = new File::Reference( fileRef );
        }
        File::ReferencePtr GetAssetFileRef()
        {
            return m_AssetFileRef;
        }

        Nocturnal::Path GetFilePath();
        Nocturnal::Path GetDataDir();
        Nocturnal::Path GetBuiltDirectory();

        // x:\rcf\assets\entities\fruitBasketFromHell\appleSuccubus.entity.rb -> entities\fruitBasketFromHell\appleSuccubus.entity.rb
        std::string GetFullName() const;

        // x:\rcf\assets\entities\fruitBasketFromHell\appleSuccubus.entity.rb -> appleSuccubus
        std::string GetShortName() const;

        const std::string& GetDescription() const
        {
            return m_Description;
        }
        void SetDescription( const std::string& description )
        {
            m_Description = description;
        }
    
        // AssetTypeInfo funcitons
        static std::string GetAssetTypeName( const AssetType assetType );
        static std::string GetAssetTypeBuilder( const AssetType AssetType );
        static std::string GetAssetTypeIcon( const AssetType AssetType );

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

        // classify the asset based on its type and its attributes
        AssetType GetAssetType() const;

        static std::string s_BaseBuiltDirectory;

        // 
        // Listeners
        // 
    private:
        mutable AssetTypeChangeSignature::Event m_AssetTypeChanged;
    public:
        void AddAssetTypeChangedListener( const AssetTypeChangeSignature::Delegate& listener )
        {
            m_AssetTypeChanged.Add( listener );
        }
        void RemoveAssetTypeChangedListener( const AssetTypeChangeSignature::Delegate& listener )
        {
            m_AssetTypeChanged.Remove( listener );
        }
    };
}