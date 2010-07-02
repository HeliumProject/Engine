#pragma once

#include <hash_map>
#include <set>

#include "Pipeline/API.h"
#include "AssetFlags.h"
#include "AssetVersion.h"
#include "AssetType.h"

#include "Foundation/Container/OrderedSet.h"
#include "Foundation/File/Path.h"
#include "Foundation/Component/Component.h"
#include "Foundation/Component/ComponentCollection.h" 

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

    class PIPELINE_API AssetClass NOC_ABSTRACT : public Component::ComponentCollection
    {
        //
        // Member Data
        //
    private:

        Nocturnal::Path m_Path;

        // description of this asset
        tstring m_Description;

        //
        // RTTI
        //

    public:
        REFLECT_DECLARE_ABSTRACT(AssetClass, ComponentCollection);

        static void EnumerateClass( Reflect::Compositor<AssetClass>& comp );

        //
        // Member functions
        //

    public:
        AssetClass();

        static void SetBaseBuiltDirectory( const tstring& path )
        {
            s_BaseBuiltDirectory = path;
        }

        static AssetClassPtr LoadAssetClass( const tchar* path );
        static AssetClassPtr LoadAssetClass( const tstring& path )
        {
            return LoadAssetClass( path.c_str() );
        }
        static AssetClassPtr LoadAssetClass( const Nocturnal::Path& path )
        {
            return LoadAssetClass( path.Get().c_str() );
        }

        template <class T>
        static Nocturnal::SmartPtr<T> LoadAssetClass( const tchar* path )
        {
            return Reflect::TryCast<T>( LoadAssetClass( path ) );
        }

        template <class T>
        static Nocturnal::SmartPtr<T> LoadAssetClass( const tstring& path )
        {
            return Reflect::TryCast<T>( LoadAssetClass( path.c_str() ) );
        }

        template <class T>
        static Nocturnal::SmartPtr<T> LoadAssetClass( const Nocturnal::Path& path )
        {
            return Reflect::TryCast<T>( LoadAssetClass( path.Get().c_str() ) );
        }

        void SetPath( const Nocturnal::Path& path )
        {
            m_Path = path;
        }
        const Nocturnal::Path& GetPath()
        {
            return m_Path;
        }

        const Nocturnal::Path& GetFilePath();
        Nocturnal::Path GetDataDir();
        Nocturnal::Path GetBuiltDirectory();

        // x:\rcf\assets\entities\fruitBasketFromHell\appleSuccubus.entity.rb -> entities\fruitBasketFromHell\appleSuccubus.entity.rb
        tstring GetFullName() const;

        // x:\rcf\assets\entities\fruitBasketFromHell\appleSuccubus.entity.rb -> appleSuccubus
        tstring GetShortName() const;

        const tstring& GetDescription() const
        {
            return m_Description;
        }
        void SetDescription( const tstring& description )
        {
            m_Description = description;
        }
    
        // AssetTypeInfo funcitons
        static tstring GetAssetTypeName( const AssetType assetType );
        static tstring GetAssetTypeBuilder( const AssetType AssetType );
        static tstring GetAssetTypeIcon( const AssetType AssetType );

        // configure this instance as the default instance of the derived class
        virtual void MakeDefault() {}

        // we were changed by somebody, reclassify
        virtual void ComponentChanged( const Component::ComponentBase* attr = NULL ) NOC_OVERRIDE;

        // write to the location on disk backed by the file manager id
        virtual void Serialize( const AssetVersionPtr &version = new AssetVersion () );

        // add to or set an attribute in the collection
        virtual void SetComponent( const Component::ComponentPtr& attr, bool validate = true ) NOC_OVERRIDE;

        // remove attribute from a slot
        virtual void RemoveComponent( i32 typeID ) NOC_OVERRIDE;

        // Returns true by default. Override to specify more stringent requirements on the asset.
        virtual bool ValidateClass( tstring& error ) const;

        // validate the incoming attribute as ok to consume
        virtual bool ValidateCompatible( const Component::ComponentPtr &component, tstring& error ) const NOC_OVERRIDE;

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

        static tstring s_BaseBuiltDirectory;

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