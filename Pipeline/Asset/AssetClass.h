#pragma once

#include <hash_map>
#include <set>

#include "Pipeline/API.h"

#include "Foundation/Container/OrderedSet.h"
#include "Foundation/File/Path.h"
#include "Foundation/Component/Component.h"
#include "Foundation/Component/ComponentCollection.h" 

#define REGEX_LEVEL_DIR "levels\\/(?:test\\/){0,1}([0-9a-zA-Z \\-_]+)?"

namespace Asset
{
    namespace AssetProperties
    {
        static const tchar* ShortDescription  = TXT( "ShortDescription" );
        static const tchar* LongDescription   = TXT( "ShortDescription" );
        static const tchar* SmallIcon         = TXT( "SmallIcon" );
        static const tchar* RootFolderSpec    = TXT( "RootFolderSpec" );
        static const tchar* FileFilter        = TXT( "FileFilter" );
        static const tchar* AssetTemplates    = TXT( "AssetTemplates" );
    }

    class AssetClass;
    typedef Nocturnal::SmartPtr< AssetClass > AssetClassPtr;
    typedef std::vector< AssetClassPtr > V_AssetClass;

    typedef std::set<AssetClassPtr> S_AssetClass;
    typedef Nocturnal::OrderedSet< Asset::AssetClassPtr > OS_AssetClass;

    class PIPELINE_API AssetClass NOC_ABSTRACT : public Component::ComponentCollection
    {
    private:

        Nocturnal::Path m_Path;

        tstring m_Description;
        std::set< tstring > m_Tags;

    private:
        static tstring s_BaseBuiltDirectory;

    public:
        //
        // RTTI
        //
        REFLECT_DECLARE_ABSTRACT( AssetClass, ComponentCollection );

        static void EnumerateClass( Reflect::Compositor< AssetClass >& comp );


    public:
        AssetClass();

    public:
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

    public:

        void SetPath( const Nocturnal::Path& path )
        {
            m_Path = path;
        }
        const Nocturnal::Path& GetPath()
        {
            return m_Path;
        }

        Nocturnal::Path GetBuiltDirectory();

        // stuff/fruitBasketFromHell/appleSuccubus.entity.nrb
        tstring GetFullName() const;

        // appleSuccubus
        tstring GetShortName() const;

        const tstring& GetDescription() const
        {
            return m_Description;
        }
        void SetDescription( const tstring& description )
        {
            m_Description = description;
        }

        const std::set< tstring >& GetTags() const
        {
            return m_Tags;
        }
        void SetTags( const std::set< tstring >& tags )
        {
            m_Tags = tags;
        }
        void AddTag( const tstring& tag )
        {
            m_Tags.insert( tag );
        }
        void RemoveTag( const tstring& tag )
        {
            m_Tags.erase( tag );
        }

    public:
        virtual void GatherSearchableProperties( Nocturnal::SearchableProperties* properties ) const NOC_OVERRIDE;
        virtual void GetFileReferences( std::set< Nocturnal::Path >& fileReferences );

    public:
        // we were changed by somebody, reclassify
        virtual void ComponentChanged( const Component::ComponentBase* attr = NULL ) NOC_OVERRIDE;

        // add to or set an attribute in the collection
        virtual void SetComponent( const Component::ComponentPtr& attr, bool validate = true ) NOC_OVERRIDE;

        // remove attribute from a slot
        virtual void RemoveComponent( i32 typeID ) NOC_OVERRIDE;

    public:

        // Returns true by default. Override to specify more stringent requirements on the asset.
        virtual bool ValidateClass( tstring& error ) const;

        // validate the incoming attribute as ok to consume
        virtual bool ValidateCompatible( const Component::ComponentPtr &component, tstring& error ) const NOC_OVERRIDE;


    public:
        // write to the location on disk backed by the file manager id
        virtual void Serialize();

        // callback when this AssetClass has finished loading off disk
        virtual void LoadFinished();

        // copy this asset and its attributes into the destination
        virtual void CopyTo(const Reflect::ElementPtr& destination) NOC_OVERRIDE;
    };
}