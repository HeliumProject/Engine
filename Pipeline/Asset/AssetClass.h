#pragma once

#include <hash_map>
#include <set>

#include "Pipeline/API.h"

#include "Pipeline/Asset/AssetFactory.h"

#include "Foundation/Component/Component.h"
#include "Foundation/Component/ComponentCollection.h" 
#include "Foundation/Container/OrderedSet.h"
#include "Foundation/Document/Document.h"
#include "Foundation/File/Path.h"

namespace Helium
{
    namespace Asset
    {
        namespace AssetProperties
        {
            static const tchar_t* ShortDescription  = TXT( "ShortDescription" );
            static const tchar_t* LongDescription   = TXT( "LongDescription" );
            static const tchar_t* SmallIcon         = TXT( "SmallIcon" );
            static const tchar_t* FileFilter        = TXT( "FileFilter" );
        }

        class AssetClass;
        typedef Helium::StrongPtr< AssetClass > AssetClassPtr;
        typedef std::vector< AssetClassPtr > V_AssetClass;

        typedef std::set<AssetClassPtr> S_AssetClass;
        typedef Helium::OrderedSet< Asset::AssetClassPtr > OS_AssetClass;

        class PIPELINE_API AssetClass HELIUM_ABSTRACT : public Component::ComponentCollection
        {
        private:
            Helium::Path m_SourcePath;   // optional path where this asset was deserialized from
            Helium::Path m_ContentPath; // path to the asset's backing/art file

            tstring m_Description;
            std::set< tstring > m_Tags;

        private:
            static tstring s_BaseBuiltDirectory;
            static std::map< tstring, class AssetFactory* > s_AssetFactories;

        public:
            REFLECT_DECLARE_ABSTRACT( AssetClass, ComponentCollection );
            static void AcceptCompositeVisitor( Reflect::Composite& comp );

        public:
            AssetClass();

        public:
            static void SetBaseBuiltDirectory( const tstring& path )
            {
                s_BaseBuiltDirectory = path;
            }

            static AssetClassPtr LoadAssetClass( const Path& path );

            template <class T>
            static Helium::StrongPtr<T> LoadAssetClass( const Path& path )
            {
                return Reflect::TryCast<T>( LoadAssetClass( path ) );
            }

        public:
            static void RegisterFactory( const tstring& extension, AssetFactory* factory )
            {
                s_AssetFactories[ extension ] = factory;
            }

            static void UnregisterFactory( const tstring& extension )
            {
                s_AssetFactories.erase( extension );
            }

            static void UnregisterFactory( const AssetFactory* factory )
            {
                for ( std::map< tstring, AssetFactory* >::iterator itr = s_AssetFactories.begin(), end = s_AssetFactories.end(); itr != end; )
                {
                    if ( (*itr).second == factory )
                    {
                        s_AssetFactories.erase( itr++ );

                        if ( itr == end )
                        {
                            break;
                        }
                    }

                    ++itr;
                }
            }

            static AssetClassPtr Create( const Helium::Path& path )
            {
                std::map< tstring, AssetFactory* >::iterator itr = s_AssetFactories.find( path.Extension() );
                
                if ( itr != s_AssetFactories.end() )
                {
                    return (*itr).second->Create( path );
                }

                return NULL;
            }

            static void GetExtensions( std::set< tstring >& extensions )
            {
                for ( std::map< tstring, AssetFactory* >::const_iterator itr = s_AssetFactories.begin(), end = s_AssetFactories.end(); itr != end; ++itr )
                {
                    (*itr).second->GetExtensions( extensions );
                }
            }

        public:

            void SetSourcePath( const Helium::Path& path )
            {
                m_SourcePath = path;
            }

            const Helium::Path& GetSourcePath()
            {
                return m_SourcePath;
            }

            void SetContentPath( const Helium::Path& path )
            {
                m_ContentPath = path;
            }
            const Helium::Path& GetContentPath()
            {
                return m_ContentPath;
            }

            Helium::Path GetBuiltDirectory();

            // stuff/fruitBasketFromHell/appleSuccubus.HeliumEntity
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
            virtual void GatherSearchableProperties( Helium::SearchableProperties* properties ) const HELIUM_OVERRIDE;
            virtual void GetFileReferences( std::set< Helium::Path >& fileReferences );

        public:
            // we were changed by somebody, reclassify
            virtual void ComponentChanged( const Component::ComponentBase* component = NULL ) HELIUM_OVERRIDE;

            // add to or set an attribute in the collection
            virtual bool SetComponent( const Component::ComponentPtr& component, bool validate = true, tstring* error = NULL ) HELIUM_OVERRIDE;

            // remove attribute from a slot
            virtual bool RemoveComponent( const Reflect::Class* type ) HELIUM_OVERRIDE;

        public:

            // Returns true by default. Override to specify more stringent requirements on the asset.
            virtual bool ValidateClass( tstring& error ) const;

            // validate the incoming attribute as ok to consume
            virtual bool ValidateCompatible( const Component::ComponentPtr &component, tstring& error ) const HELIUM_OVERRIDE;


        public:
            void ConnectDocument( Document* document );
            void DisconnectDocument( const Document* document );

            // Callback for when a document is saved.
            void OnDocumentSave( const DocumentEventArgs& args );

            mutable DocumentObjectChangedSignature::Event e_HasChanged;


            // write to the location on disk
            virtual bool Serialize();

            // callback when this AssetClass has finished loading off disk
            virtual void LoadFinished();
        };
    }
}