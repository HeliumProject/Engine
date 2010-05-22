#include "AssetVisitor.h"

#include "Asset/Exceptions.h"
#include "Asset/Entity.h"
#include "Asset/EntityAsset.h"
#include "Asset/DependenciesAttribute.h"

#include "Asset/ArtFileAttribute.h"
#include "Attribute/AttributeHandle.h"
#include "Common/Container/Insert.h" 
#include "Common/Flags.h"
#include "Common/Types.h"
#include "Content/Region.h"
#include "Content/Scene.h"
#include "Content/Zone.h"
#include "FileSystem/FileSystem.h"
#include "Finder/ContentSpecs.h"
#include "Finder/Finder.h"
#include "Finder/AssetSpecs.h"
#include "Finder/ExtensionSpecs.h"
#include "RCS/RCS.h"
#include "Reflect/Archive.h"
#include "Reflect/Class.h"
#include "Reflect/Field.h"
#include "Reflect/Serializers.h"
#include "Reflect/Version.h"
#include "Reflect/Visitor.h"
#include "TUID/TUID.h"
#include "UniqueID/TUID.h"

using Nocturnal::Insert; 
using namespace Asset;
using namespace Asset::AssetFlags;

using namespace Asset;


/////////////////////////////////////////////////////////////////////////////
inline bool CheckStopRequested( bool* cancel )
{
    if ( cancel == NULL )
    {
        return false;
    }
    return *cancel;
}

/////////////////////////////////////////////////////////////////////////////
AssetVisitor::AssetVisitor( M_AssetFiles* assetFiles, Asset::AssetClass* assetClass, bool* stopRequested )
: m_AssetFiles( assetFiles )
, m_AssetClass( assetClass )
, m_StopRequested( stopRequested )
{
    if ( !assetFiles || !assetClass )
    {
        NOC_BREAK();
    }

    PushHash( m_AssetClass->GetAssetFileRef()->GetHash() );
    m_CurrentElement = m_AssetClass;


    //
    // Register custom handlers
    //

    // Elements
    m_ElementHandlerLookup.insert( ElementHandlerLookup::value_type( 
        Reflect::GetType<Asset::ArtFileAttribute>(),
        &AssetVisitor::HandleArtFileAttribute ) );

    // Field
    m_FieldFilterTypes.insert( (i32)Reflect::FieldFlags::Discard );

    // Files
    m_FileHandlerLookup.insert( FileHandlerLookup::value_type( 
        &FinderSpecs::Asset::WORLD_DECORATION,
        &AssetVisitor::HandleWorldFile ) );

    m_FileHandlerLookup.insert( FileHandlerLookup::value_type( 
        &FinderSpecs::Asset::ZONE_DECORATION,
        &AssetVisitor::HandleZoneFile ) );

    m_FileHandlerLookup.insert( FileHandlerLookup::value_type( 
        &FinderSpecs::Extension::MAYA_BINARY,
        &AssetVisitor::HandleMayaFile ) );

    m_FileHandlerLookup.insert( FileHandlerLookup::value_type( 
        &FinderSpecs::Extension::REFLECT_BINARY,
        &AssetVisitor::HandleAssetFile ) ); 
}

/////////////////////////////////////////////////////////////////////////////
AssetVisitor::~AssetVisitor()
{
    m_VisitedHashStack.clear();
    m_CurrentElement = NULL;
    m_ElementFilterTypes.clear();
    m_ElementHandlerLookup.clear();
    m_FileHandlerLookup.clear();
    m_AssetFiles = NULL;
    m_AssetClass = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Visitor class override
//
bool AssetVisitor::VisitElement( Reflect::Element* element )
{
    if ( m_ElementFilterTypes.find( element->GetType() ) != m_ElementFilterTypes.end() )
    {
        return false;
    }

    if ( m_CurrentElement && m_CurrentElement == element )
    {
        return true;
    }

    //Console::Bullet processBullet( "\nVisitElement: %s (%s)\n",
    //  element->GetClass()->m_UIName.c_str(),
    //  Reflect::Registry::GetInstance()->GetClass( element->GetType() )->m_FullName.c_str() );

    //-----------------------------------------------
    // Custom element processors
    ElementHandlerLookup::iterator findHandler = m_ElementHandlerLookup.find( element->GetType() );
    if ( findHandler != m_ElementHandlerLookup.end() )
    {
        ElementHandlerFunc handlerFunc = findHandler->second;
        NOC_ASSERT( handlerFunc );

        return (this->*handlerFunc)(element);
    }

    //-----------------------------------------------
    // There is no custom handler for this element
    return HandleElement( element );
}

/////////////////////////////////////////////////////////////////////////////
// Default Element Handler
//
bool AssetVisitor::HandleElement( Reflect::Element* element )
{
    if ( CheckStopRequested( m_StopRequested ) )
        return false;

    //Console::Indentation indent;
    m_CurrentElement = element;
    element->Host( *this );
    return false;
}

/////////////////////////////////////////////////////////////////////////////
bool AssetVisitor::HandleArtFileAttribute( Reflect::Element* element )
{
    using namespace Asset;
    using namespace Reflect;

    AssetFile* assetFile = GetCurrentAssetFile();
    if ( assetFile )
    {
        // Determine which shaders this asset is currently using
        Asset::ArtFileAttribute* attribute = Reflect::AssertCast< Asset::ArtFileAttribute >(element);

        File::Reference fileRef = attribute->GetFileReference();
        fileRef.Resolve();
        std::string artFile = fileRef.GetPath();

        if ( !artFile.empty() )
        {
            const std::string& attrName = Reflect::Registry::GetInstance()->GetClass( element->GetType() )->m_FullName;
            assetFile->AddAttribute( attrName, artFile, false );

            std::string assetManifestFile = FinderSpecs::Content::MANIFEST_DECORATION.GetExportFile( artFile, attribute->m_FragmentNode );
            if ( FileSystem::Exists(assetManifestFile) )
            {
                Asset::EntityManifestPtr manifest;
                try
                {
                    manifest = Archive::FromFile<Asset::EntityManifest>(assetManifestFile);
                }
                catch ( const Nocturnal::Exception& ex )
                {
                    Console::Warning( "%s\n", ex.what() );
                }

                if (manifest.ReferencesObject())
                {
                    File::V_Reference::iterator newItr = manifest->m_Shaders.begin();
                    File::V_Reference::iterator newEnd = manifest->m_Shaders.end();
                    for ( ; newItr != newEnd; ++newItr )
                    {
                        if ( CheckStopRequested( m_StopRequested ) )
                            return false;

                        File::ReferencePtr shaderRef = (*newItr);

                        M_AssetFiles::iterator found = m_AssetFiles->find( shaderRef->GetId() );
                        if ( found != m_AssetFiles->end() )
                        {
                            // this file is already in our list so add it as a dependency and then move on
                            AddDependency( found->second );
                        }
                        else
                        {
                            Insert<M_AssetFiles>::Result inserted = m_AssetFiles->insert( M_AssetFiles::value_type( shaderRef->GetId(), new AssetFile( *shaderRef ) ) );
                            if ( inserted.second )
                            {
                                AssetFilePtr& shaderAssetFile = inserted.first->second;
                                PreHandleFile( shaderAssetFile );
                                {
                                    // passing NULL in as the field since HandleAssetFile doesn't use the field. That should be changed to be an optional param
                                    HandleAssetFile( element, NULL, shaderAssetFile );
                                }
                                PostHandleFile( shaderAssetFile );
                            }
                        }
                    }

                    return true;
                }
            }
        }
    }

    // we couldn't handle it
    return HandleElement( element );
}

/////////////////////////////////////////////////////////////////////////////
// Visitor class override
//
bool AssetVisitor::VisitField( Reflect::Element* element, const Reflect::Field* field )
{
    S_i32::const_iterator filterItr = m_FieldFilterTypes.begin();
    S_i32::const_iterator filterEnd = m_FieldFilterTypes.end();
    for ( ; filterItr != filterEnd; ++filterItr )
    {
        if ( Nocturnal::HasFlags<i32>( field->m_Flags, (*filterItr ) ) )
            return true;
    }

    if ( Nocturnal::HasFlags<i32>( field->m_Flags, Reflect::FieldFlags::Discard ) )
        //|| !Nocturnal::HasFlags<i32>( field->m_Flags, AssetFlags::ManageField  ) )
    {
        return true;
    }

    //Console::Bullet processBullet( "VisitField: %s\n", field->m_UIName.c_str() );
    {
        //-----------------------------------------------
        // TODO: Add custom field processors
        if ( false )
        {
            return true;
        }

        //-----------------------------------------------
        // There is no custom processor for this field
        return HandleField( element, field );
    }
}

/////////////////////////////////////////////////////////////////////////////
// Default field processor if no override is provided
//
bool AssetVisitor::HandleField( Reflect::Element* element, const Reflect::Field* field )
{
    if ( ( field->m_Flags & Reflect::FieldFlags::FileRef ) != 0 )
    {
        File::ReferencePtr ref;
        Reflect::Serializer::GetValue( field->CreateSerializer( element ), ref );

        HandleFileRef( element, field, ref );
    }
    //-----------------------------------------------
    else if ( field->m_SerializerID == Reflect::GetType< Reflect::ElementArraySerializer >() )
    {
        Reflect::ElementArraySerializerPtr arraySerializer = Reflect::DangerousCast< Reflect::ElementArraySerializer >( field->CreateSerializer( element ) );
        Reflect::V_Element& vals = arraySerializer->m_Data.Ref();

        if ( (int)vals.size() < 1 )
            return true;

        for each ( const Reflect::ElementPtr& memberElem in vals )
        {
            if ( CheckStopRequested( m_StopRequested ) )
                return false;

            m_CurrentElement = memberElem;
            memberElem->Host( *this );
        }

        return false;
    }

    // continue search
    return true;
}

/////////////////////////////////////////////////////////////////////////////
void AssetVisitor::HandleFileRef( Reflect::Element* element, const Reflect::Field* field, File::ReferencePtr& fileRef )
{
    fileRef->Resolve();

    if ( fileRef->GetId() == TUID::Null )
    {
        return;
    }

    if ( *fileRef == *( m_AssetClass->GetAssetFileRef() ) )
    {
        return;
    }

/*
    M_AssetFiles::iterator found = m_AssetFiles->find( id );
    if ( found != m_AssetFiles->end() )
    {
        // this file is already in our list so add it as a dependency and then move on
        AddDependency( found->second );
    }
    else
    {
        File::ManagedFilePtr file = File::GlobalManager().GetManagedFile( id );  
        if ( !file )
            return;

        //Console::Bullet processBullet( "HandleFileID: %s\n", element->GetClass()->m_UIName.c_str() );
        Insert<M_AssetFiles>::Result inserted = m_AssetFiles->insert( M_AssetFiles::value_type( file->m_Id, new AssetFile( file ) ) );
        if ( inserted.second )
        {
            //Console::Print( " o %s\n", file->m_Path.c_str() );

            AssetFilePtr& assetFile = inserted.first->second;
            PreHandleFile( assetFile );
            {
                const Finder::ModifierSpec* extensionSpec = Finder::GetFileExtensionSpec( file->m_Path );

                //-----------------------------------------------
                // Custom file processors
                FileHandlerLookup::iterator findHandler = m_FileHandlerLookup.find( extensionSpec );
                if ( findHandler != m_FileHandlerLookup.end() )
                {
                    FileHandlerFunc handlerFunc = findHandler->second;
                    NOC_ASSERT( handlerFunc );

                    (this->*handlerFunc)( element, field, assetFile );
                }
                //-----------------------------------------------
                // Default Asset file handlers
                else if ( FileSystem::HasExtension( assetFile->GetFilePath(), FinderSpecs::Extension::REFLECT_BINARY.GetExtension() ) )
                {
                    HandleAssetFile( element, field, assetFile );
                }
            }
            PostHandleFile( assetFile );
        }
    }
    */
}

/////////////////////////////////////////////////////////////////////////////
void AssetVisitor::HandleAssetFile( Reflect::Element* element, const Reflect::Field* field, AssetFile* assetFile )
{
    if ( CheckStopRequested( m_StopRequested ) )
        return;

    /*
    Asset::AssetClassPtr assetClass = Asset::AssetClass::FindAssetClass( assetFile->GetFileID(), false );
    if ( assetClass.ReferencesObject() )
    {
        m_CurrentElement = assetClass;
        assetClass->Host( *this );
    }
    */
}

/////////////////////////////////////////////////////////////////////////////
void AssetVisitor::CollectRegionZones( const Content::RegionPtr& region, const Content::V_Zone& zones, Content::V_Zone& regionZones )
{
    const UniqueID::V_TUID& regionZoneID = region->GetZones(); 

    // go through all the zones in the content file 
    Content::V_Zone::const_iterator itrZone = zones.begin();
    Content::V_Zone::const_iterator endZone = zones.end();
    for ( ; itrZone != endZone; ++itrZone )
    {
        if ( CheckStopRequested( m_StopRequested ) )
            return;

        const Content::ZonePtr& zone = (*itrZone);

        if(!zone->m_Active)
            continue; 

        // m_BuilderOptions->m_Zones is basically the list of cinematic zone tuids
        // we could not do this and instead make a fake region for the cinematic (like the default region)
        // 
        //if(  !m_BuilderOptions->m_Zones.empty() && 
        //  std::find( m_BuilderOptions->m_Zones.begin(), m_BuilderOptions->m_Zones.end(), zone->m_FileID ) == m_BuilderOptions->m_Zones.end() )
        //{
        //  continue; 
        //}

        // m_BuilderOptions->m_ZoneList is the list of zone names provided by the user (possibly)
        //if( !m_BuilderOptions->m_ZoneList.empty() && 
        //  std::find( m_BuilderOptions->m_ZoneList.begin(), m_BuilderOptions->m_ZoneList.end(), zone->GetName() ) == m_BuilderOptions->m_ZoneList.end() )
        //{
        //  continue; 
        //}

        if( zone->m_Global )
        {
            regionZones.push_back( zone ); 
            continue; 
        }

        UniqueID::V_TUID::const_iterator itrRZ = regionZoneID.begin();
        UniqueID::V_TUID::const_iterator endRZ = regionZoneID.end();
        for ( ; itrRZ != endRZ; ++itrRZ )
        {
            if ( CheckStopRequested( m_StopRequested ) )
                return;

            if( zone->m_ID == (*itrRZ) )
            {
                regionZones.push_back( zone ); 
                break; 
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
void AssetVisitor::HandleWorldFile( Reflect::Element* element, const Reflect::Field* field, AssetFile* assetFile )
{
    /*
    Content::Scene worldScene( assetFile->GetFilePath() );

    ////////////////////////////////////////
    Content::V_Zone zones;
    worldScene.GetAll< Content::Zone >( zones ); 

    // do zone stuff
    Content::V_Zone::const_iterator itrZone = zones.begin();
    Content::V_Zone::const_iterator endZone = zones.end();
    for ( ; itrZone != endZone; ++itrZone )
    {
        if ( CheckStopRequested( m_StopRequested ) )
            return;

        const Content::ZonePtr& zone = (*itrZone);

        std::string zoneFilePath = zone->GetFilePath();
        File::ManagedFilePtr file = File::GlobalManager().GetManagedFile( zoneFilePath );

        if ( file )
        {
            M_AssetFiles::iterator found = m_AssetFiles->find( file->m_Id );
            if ( found != m_AssetFiles->end() )
            {
                // this file is already in our list so add it as a dependency and then move on
                AddDependency( found->second );
            }
            else
            {
                Insert<M_AssetFiles>::Result inserted = m_AssetFiles->insert( M_AssetFiles::value_type( file->m_Id, new AssetFile( file ) ) );
                if ( inserted.second )
                {
                    AssetFilePtr& zoneAssetFile = inserted.first->second;
                    PreHandleFile( zoneAssetFile );
                    {
                        HandleZoneFile( element, field, zoneAssetFile );
                    }
                    PostHandleFile( zoneAssetFile );
                }
            }
        }
    }


    ////////////////////////////////////////
    Content::V_Region regions; 
    worldScene.GetAll< Content::Region >( regions ); 

    // do region stuff
    typedef std::map< Content::RegionPtr, Content::V_Zone > M_RegionToZone; 
    M_RegionToZone regionToZones;

    Content::V_Region::const_iterator itrRegion = regions.begin();
    Content::V_Region::const_iterator endRegion = regions.end();
    for ( ; itrRegion != endRegion; ++itrRegion )
    {
        if ( CheckStopRequested( m_StopRequested ) )
            return;

        const Content::RegionPtr& region = (*itrRegion);

        //std::string regionName = region->GetName()
        //toLower( regionName );

        Nocturnal::Insert<M_RegionToZone>::Result inserted = regionToZones.insert( M_RegionToZone::value_type( region, Content::V_Zone() ) ); 
        if ( inserted.second )
        {
            const UniqueID::V_TUID& regionZoneIDs = region->GetZones();

            CollectRegionZones( region, zones, inserted.first->second ); 
            //region->GetName();
            //if( m_RegionToZones[ region ].size() > 0)
            //  ...


            //Content::V_Zone::const_iterator itr = m_zones.begin();
            //Content::V_Zone::const_iterator end = m_zones.end();
            //for ( ; itr != end; ++itr )
            //{
            //  const Content::ZonePtr& zone = (*itr);

            //}
        }
    }
    */
}

/////////////////////////////////////////////////////////////////////////////
void AssetVisitor::HandleZoneFile( Reflect::Element* element, const Reflect::Field* field, AssetFile* assetFile )
{
    /*
    Content::Scene zoneScene( assetFile->GetFilePath() );

    Asset::V_Entity entities;
    zoneScene.GetAll< Asset::Entity >( entities );

    for each ( const Asset::EntityPtr& entity in entities )
    {
        if ( CheckStopRequested( m_StopRequested ) )
            return;

        const tuid& entityID = entity->GetEntityAssetID();

        M_AssetFiles::iterator found = m_AssetFiles->find( entityID );
        if ( found != m_AssetFiles->end() )
        {
            // this file is already in our list so add it as a dependency and then move on
            AddDependency( found->second );
        }
        else
        {
            File::ManagedFilePtr file = File::GlobalManager().GetManagedFile( entityID );

            if ( file )
            {
                Insert<M_AssetFiles>::Result inserted = m_AssetFiles->insert( M_AssetFiles::value_type( file->m_Id, new AssetFile( file ) ) );
                if ( inserted.second )
                {
                    AssetFilePtr& entityAssetFile = inserted.first->second;
                    PreHandleFile( entityAssetFile );
                    {
                        HandleAssetFile( element, field, entityAssetFile );
                    }
                    PostHandleFile( entityAssetFile );
                }
            }
        }
    }
    */
}

/////////////////////////////////////////////////////////////////////////////
void AssetVisitor::HandleMayaFile( Reflect::Element* element, const Reflect::Field* field, AssetFile* assetFile )
{
    /*
    using namespace Asset;
    using namespace Reflect;

    // Determine which shaders this asset is currently using
    S_tuid currentShaderIds;

    return;

    AssetClassPtr assetClass = AssetFile::GetAssetClass( assetFile );
    Attribute::AttributeViewer< ArtFileAttribute > model ( assetClass );
    if ( model.Valid() )
    {
        std::string artFile;
        try
        {
            artFile = model->GetFilePath();
        }
        catch( const Nocturnal::Exception& )
        {
        }

        if (!artFile.empty())
        {
            std::string assetManifestFile = FinderSpecs::Content::MANIFEST_DECORATION.GetExportFile( artFile, model->m_FragmentNode );

            if (FileSystem::Exists(assetManifestFile))
            {
                Asset::EntityManifestPtr manifest;
                try
                {
                    manifest = Archive::FromFile<Asset::EntityManifest>(assetManifestFile);
                }
                catch ( const Nocturnal::Exception& ex )
                {
                    Console::Warning( "%s\n", ex.what() );
                }

                if (manifest.ReferencesObject())
                {
                    currentShaderIds.insert( manifest->m_Shaders.begin(), manifest->m_Shaders.end() );
                }
            }
        }

        S_tuid::iterator newItr = currentShaderIds.begin();
        S_tuid::iterator newEnd = currentShaderIds.end();

        for ( ; newItr != newEnd; ++newItr )
        {
            if ( CheckStopRequested( m_StopRequested ) )
                return;

            const tuid& shaderID = (*newItr);

            M_AssetFiles::iterator found = m_AssetFiles->find( shaderID );
            if ( found != m_AssetFiles->end() )
            {
                // this file is already in our list so add it as a dependency and then move on
                AddDependency( found->second );
            }
            else
            {
                File::ManagedFilePtr file = File::GlobalManager().GetManagedFile( shaderID );

                if ( file )
                {
                    Insert<M_AssetFiles>::Result inserted = m_AssetFiles->insert( M_AssetFiles::value_type( file->m_Id, new AssetFile( file ) ) );
                    if ( inserted.second )
                    {
                        AssetFilePtr& shaderAssetFile = inserted.first->second;
                        PreHandleFile( shaderAssetFile );
                        {
                            HandleAssetFile( element, field, shaderAssetFile );
                        }
                        PostHandleFile( shaderAssetFile );
                    }
                }
            }
        }
    }
*/
#pragma TODO ("Get the screen shot")
}

/////////////////////////////////////////////////////////////////////////////
AssetFile* AssetVisitor::GetCurrentAssetFile()
{
    if ( !m_VisitedHashStack.empty() )
    {
        const u64& assetHash = m_VisitedHashStack.back();

        M_AssetFiles::iterator found = m_AssetFiles->find( assetHash );
        NOC_ASSERT( found != m_AssetFiles->end() );

        return found->second;
    }

    return NULL;
}

/////////////////////////////////////////////////////////////////////////////
void AssetVisitor::PreHandleFile( AssetFile* assetFile )
{
    if ( CheckStopRequested( m_StopRequested ) )
        return;

    AddDependency( assetFile );
    PushHash( assetFile->GetFileReference()->GetHash() );
}

/////////////////////////////////////////////////////////////////////////////
void AssetVisitor::PostHandleFile( AssetFile* assetFile )
{
    if ( CheckStopRequested( m_StopRequested ) )
        return;

    PopHash();
}

/////////////////////////////////////////////////////////////////////////////
void AssetVisitor::AddDependency( AssetFile* assetFile )
{
    AssetFile* parentAssetFile = GetCurrentAssetFile();
    if ( parentAssetFile )
    {
        parentAssetFile->AddDependency( assetFile->GetFileReference() );
    }
}

/////////////////////////////////////////////////////////////////////////////
void AssetVisitor::PushHash( const u64& assetHash )
{
    m_VisitedHashStack.push_back( assetHash );
}

/////////////////////////////////////////////////////////////////////////////
void AssetVisitor::PopHash()
{
    if ( m_VisitedHashStack.empty() )
    {
        NOC_BREAK();
    }

    m_VisitedHashStack.pop_back();
}