#include "stdafx.h"

#include "AssetTrackerVisitor.h"


#include "Asset/DependenciesAttribute.h"
#include "Asset/Exceptions.h"
#include "Asset/Entity.h"
#include "Asset/EntityAsset.h"
#include "Asset/ShaderUsagesAttribute.h"
#include "Asset/UpdateClassAttribute.h"

#include "Asset/ArtFileAttribute.h"
#include "Attribute/AttributeHandle.h"
#include "Common/Container/Insert.h" 
#include "Common/Flags.h"
#include "Common/Types.h"
#include "Content/Region.h"
#include "Content/Scene.h"
#include "Content/Zone.h"
#include "File/Manager.h"
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
AssetTrackerVisitor::AssetTrackerVisitor( M_AssetFiles* assetFiles, Asset::AssetClass* assetClass, Asset::AssetDBPtr assetDB )
 : m_AssetFiles( assetFiles )
 , m_AssetClass( assetClass )
{
  if ( !assetFiles || !assetClass )
  {
    NOC_BREAK();
  }

  m_AssetDB = assetDB;
  //m_AssetDB->Initialize();

  PushID( m_AssetClass->GetFileID() );
  m_CurrentElement = m_AssetClass;


  //
  // Register custom handlers
  //

  // Elements
  m_ElementFilterTypes.insert( Reflect::GetType<Asset::ShaderUsagesAttribute>() );
  m_ElementFilterTypes.insert( Reflect::GetType<Asset::ShaderUsage>() );

  m_ElementHandlerLookup.insert( ElementHandlerLookup::value_type( 
    Reflect::GetType<Asset::UpdateClassAttribute>(),
    &AssetTrackerVisitor::HandleUpdateClassAttribute ) );

  m_ElementHandlerLookup.insert( ElementHandlerLookup::value_type( 
    Reflect::GetType<Asset::ArtFileAttribute>(),
    &AssetTrackerVisitor::HandleArtFileAttribute ) );
  
  /*m_ElementHandlerLookup.insert( ElementHandlerLookup::value_type( 
    Reflect::GetType<Asset::AnimationAttribute>(),
    &AssetTrackerVisitor::HandleAnimationAttribute ) );

  m_ElementHandlerLookup.insert( ElementHandlerLookup::value_type( 
    Reflect::GetType<Asset::ColorMapAttribute>(),
    &AssetTrackerVisitor::HandleColorMapAttribute ) );

  m_ElementHandlerLookup.insert( ElementHandlerLookup::value_type( 
    Reflect::GetType<Asset::CubeMapAttribute>(),
    &AssetTrackerVisitor::HandleCubeMapAttribute ) );

  m_ElementHandlerLookup.insert( ElementHandlerLookup::value_type( 
    Reflect::GetType<Asset::FoliageAttribute>(),
    &AssetTrackerVisitor::HandleFoliageAttribute ) );*/

  // Field
  m_FieldFilterTypes.insert( (i32)Reflect::FieldFlags::Discard );

  // Files
  m_FileHandlerLookup.insert( FileHandlerLookup::value_type( 
    &FinderSpecs::Asset::WORLD_DECORATION,
    &AssetTrackerVisitor::HandleWorldFile ) );

  m_FileHandlerLookup.insert( FileHandlerLookup::value_type( 
    &FinderSpecs::Asset::ZONE_DECORATION,
    &AssetTrackerVisitor::HandleZoneFile ) );

  m_FileHandlerLookup.insert( FileHandlerLookup::value_type( 
    &FinderSpecs::Asset::LEVEL_DECORATION,
    &AssetTrackerVisitor::HandleLevelFile ) );

  m_FileHandlerLookup.insert( FileHandlerLookup::value_type( 
    &FinderSpecs::Extension::REFLECT_BINARY,
    &AssetTrackerVisitor::HandleAssetFile ) );

  
}

/////////////////////////////////////////////////////////////////////////////
AssetTrackerVisitor::~AssetTrackerVisitor()
{
  while ( !m_VisitedIDStack.empty() )
  {
    PopID();
  }

  m_CurrentElement = NULL;
  m_ElementFilterTypes.clear();
  m_ElementHandlerLookup.clear();
  m_FileHandlerLookup.clear();
}

/////////////////////////////////////////////////////////////////////////////
// Visitor class override
//
bool AssetTrackerVisitor::VisitElement( Reflect::Element* element )
{
  if ( m_ElementFilterTypes.find( element->GetType() ) != m_ElementFilterTypes.end() )
  {
    return false;
  }

  if ( m_CurrentElement && m_CurrentElement == element )
  {
    return true;
  }

  Console::Bullet processBullet( "\nVisitElement: %s (%s)\n",
    element->GetClass()->m_UIName.c_str(),
    Reflect::Registry::GetInstance()->GetClass( element->GetType() )->m_FullName.c_str() );

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
bool AssetTrackerVisitor::HandleElement( Reflect::Element* element )
{
  Console::Indentation indent;
  m_CurrentElement = element;
  element->Host( *this );
  return false;
}

/////////////////////////////////////////////////////////////////////////////
bool AssetTrackerVisitor::HandleArtFileAttribute( Reflect::Element* element )
{
  using namespace Asset;
  using namespace Reflect;

  AssetFile* assetFile = GetCurrentAssetFile();
  if ( assetFile )
  {
    // Determine which shaders this asset is currently using
    Asset::ArtFileAttribute* attribute = Reflect::AssertCast< Asset::ArtFileAttribute >(element);

    std::string artFile = attribute->GetFilePath();;

    if ( !artFile.empty() )
    {
      std::string assetManifestFile = FinderSpecs::Content::MANIFEST_DECORATION.GetExportFile( artFile, attribute->m_FragmentNode );
      if ( FileSystem::Exists(assetManifestFile) )
      {
        Asset::EntityManifestPtr manifest;
        try
        {
          manifest = Archive::FromFile<Asset::EntityManifest>(assetManifestFile);
        }
        catch ( const Reflect::Exception& )
        {
        }

        if (manifest.ReferencesObject())
        {
          V_tuid::iterator newItr = manifest->m_Shaders.begin();
          V_tuid::iterator newEnd = manifest->m_Shaders.end();
          for ( ; newItr != newEnd; ++newItr )
          {
            const tuid& shaderID = (*newItr);
            File::ManagedFilePtr shaderFile = File::GlobalManager().GetManagedFile( shaderID );

            // add to assetFile
            if ( shaderFile )
            {
              Insert<M_AssetFiles>::Result inserted = m_AssetFiles->insert( M_AssetFiles::value_type( shaderFile->m_Id, new AssetFile( shaderFile ) ) );
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

          AddAttribute( element );
          return true;
        }
      }
    }
  }

  // we couldn't handle it
  return HandleElement( element );
}

/////////////////////////////////////////////////////////////////////////////
bool AssetTrackerVisitor::HandleUpdateClassAttribute( Reflect::Element* element )
{
  AssetFile* assetFile = GetCurrentAssetFile();
  if ( assetFile )
  {
    Asset::UpdateClassAttribute* attribute = Reflect::AssertCast< Asset::UpdateClassAttribute >(element);
    V_string updateClasses = attribute->GetClassNames(); 

    if ( updateClasses.size() > 0 )
    {
      std::string updateClassesStr;

      V_string::iterator itr = updateClasses.begin(); 
      V_string::iterator end = updateClasses.end(); 
      for ( ; itr != end; ++itr )
      {
        // append to the list of valid IDs
        if ( !updateClassesStr.empty() )
        {
          updateClassesStr += ", ";
        }
        updateClassesStr += (*itr);
      }

      const std::string& attrName = Reflect::Registry::GetInstance()->GetClass( element->GetType() )->m_FullName;
      assetFile->AddAttribute( attrName, updateClassesStr );
      return true;
    }
  }

  // we couldn't handle it
  return HandleElement( element );
}

/////////////////////////////////////////////////////////////////////////////
// Visitor class override
//
bool AssetTrackerVisitor::VisitField( Reflect::Element* element, const Reflect::Field* field )
{
  S_i32::const_iterator filterItr = m_FieldFilterTypes.begin();
  S_i32::const_iterator filterEnd = m_FieldFilterTypes.end();
  for ( ; filterItr != filterEnd; ++filterItr )
  {
    if ( Nocturnal::HasFlags<i32>( field->m_Flags, (*filterItr ) ) )
      return true;
  }

  //if ( Nocturnal::HasFlags<i32>( field->m_Flags, Reflect::FieldFlags::Discard ) )
  //  //|| !Nocturnal::HasFlags<i32>( field->m_Flags, AssetFlags::ManageField  ) )
  //{
  //  return true;
  //}

  Console::Bullet processBullet( "VisitField: %s\n", field->m_UIName.c_str() );
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
bool AssetTrackerVisitor::HandleField( Reflect::Element* element, const Reflect::Field* field )
{
  if ( ( field->m_Flags & Reflect::FieldFlags::FileID ) != 0 )
  {
    //-----------------------------------------------
    if ( field->m_SerializerID == Reflect::GetType< Reflect::U64Serializer >() )
    {
      tuid id;
      Reflect::Serializer::GetValue( field->CreateSerializer( element ), id );

      HandleFileID( element, field, id );
    }
    //-----------------------------------------------
    else if ( field->m_SerializerID == Reflect::GetType< Reflect::U64ArraySerializer >() )
    {
      Reflect::SerializerPtr serializer = field->CreateSerializer( element );

      V_tuid vals;
      Reflect::Serializer::GetValue( serializer, vals );

      for each ( const tuid& id in vals )
      {
        HandleFileID( element, field, id );
      }
    }
    //-----------------------------------------------
    else if ( field->m_SerializerID == Reflect::GetType< Reflect::U64SetSerializer >() )
    {
      Reflect::SerializerPtr serializer = field->CreateSerializer( element );

      S_tuid vals;
      Reflect::Serializer::GetValue( serializer, vals );

      for each ( const tuid& id in vals )
      {
        HandleFileID( element, field, id );
      }
    }
    else
    {
      NOC_BREAK();
      throw File::Exception( "This field type '%s::%s' has no support for the file manager",
        field->m_Type->m_ShortName.c_str(),
        field->m_Name.c_str() );
    }
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
      m_CurrentElement = memberElem;
      memberElem->Host( *this );
    }

    return false;
  }

  // continue search
  return true;
}

/////////////////////////////////////////////////////////////////////////////
void AssetTrackerVisitor::HandleFileID( Reflect::Element* element, const Reflect::Field* field, tuid id )
{
  if ( ( id == TUID::Null )
    || ( id == m_AssetClass->GetFileID() ) )
    return;

  File::ManagedFilePtr file = File::GlobalManager().GetManagedFile( id );  
  if ( !file )
    return;

  Insert<M_AssetFiles>::Result inserted = m_AssetFiles->insert( M_AssetFiles::value_type( file->m_Id, new AssetFile( file ) ) );

  Console::Bullet processBullet( "HandleFileID: %s\n", element->GetClass()->m_UIName.c_str() );
  if ( inserted.second )
  {
    Console::Print( " o %s\n", file->m_Path.c_str() );

    AssetFilePtr& assetFile = inserted.first->second;
    PreHandleFile( assetFile );
    {
      const Finder::ModifierSpec* extensionSpec = assetFile->GetModifierSpec();
      
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

/////////////////////////////////////////////////////////////////////////////
void AssetTrackerVisitor::HandleAssetFile( Reflect::Element* element, const Reflect::Field* field, AssetFile* assetFile )
{
  Asset::AssetClassPtr assetClass = Asset::AssetClass::FindAssetClass( assetFile->GetFileID(), false );
  if ( assetClass.ReferencesObject() )
  {
    assetFile->SetAssetClass( assetClass );
    m_CurrentElement = assetClass;
    assetClass->Host( *this );
  }
}

/////////////////////////////////////////////////////////////////////////////
void CollectRegionZones( const Content::RegionPtr& region, const Content::V_Zone& zones, Content::V_Zone& regionZones )
{
  const UniqueID::V_TUID& regionZoneID = region->GetZones(); 

  // go through all the zones in the content file 
  Content::V_Zone::const_iterator itrZone = zones.begin();
  Content::V_Zone::const_iterator endZone = zones.end();
  for ( ; itrZone != endZone; ++itrZone )
  {
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
    for( ; itrRZ != endRZ; ++itrRZ )
    {
      if( zone->m_ID == (*itrRZ) )
      {
        regionZones.push_back( zone ); 
        break; 
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
void AssetTrackerVisitor::HandleWorldFile( Reflect::Element* element, const Reflect::Field* field, AssetFile* assetFile )
{
  Content::Scene worldScene( assetFile->GetFilePath() );

  ////////////////////////////////////////
  Content::V_Zone zones;
  worldScene.GetAll< Content::Zone >( zones ); 

  // do zone stuff
  Content::V_Zone::const_iterator itrZone = zones.begin();
  Content::V_Zone::const_iterator endZone = zones.end();
  for ( ; itrZone != endZone; ++itrZone )
  {
    const Content::ZonePtr& zone = (*itrZone);

    std::string zoneFilePath = zone->GetFilePath();
    File::ManagedFilePtr file = File::GlobalManager().GetManagedFile( zoneFilePath );

    if ( file )
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
}

/////////////////////////////////////////////////////////////////////////////
void AssetTrackerVisitor::HandleZoneFile( Reflect::Element* element, const Reflect::Field* field, AssetFile* assetFile )
{
  Content::Scene zoneScene( assetFile->GetFilePath() );

  Asset::V_Entity entities;
  zoneScene.GetAll< Asset::Entity >( entities );

  for each ( const Asset::EntityPtr& entity in entities )
  {
    const tuid& entityID = entity->GetEntityAssetID();
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

/////////////////////////////////////////////////////////////////////////////
void AssetTrackerVisitor::HandleLevelFile( Reflect::Element* element, const Reflect::Field* field, AssetFile* assetFile )
{
  Content::Scene levelScene( assetFile->GetFilePath() );

  ////////////////////////////////////////
  Content::V_Zone zones;
  levelScene.GetAll< Content::Zone >( zones ); 

  Content::Scene zoneScene( assetFile->GetFilePath() );

  Asset::V_Entity entities;
  zoneScene.GetAll< Asset::Entity >( entities );

  for each ( const Asset::EntityPtr& entity in entities )
  {
    const tuid& entityID = entity->GetEntityAssetID();
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

/////////////////////////////////////////////////////////////////////////////
void AssetTrackerVisitor::HandleMayaFile( Reflect::Element* element, const Reflect::Field* field, AssetFile* assetFile )
{
  using namespace Asset;
  using namespace Reflect;

  // Determine which shaders this asset is currently using
  S_tuid currentShaderIds;

  return;

  AssetClassPtr assetClass = assetFile->GetAssetClass();
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
        catch ( const Reflect::Exception& )
        {
        }

        if (manifest.ReferencesObject())
        {
          currentShaderIds.insert( manifest->m_Shaders.begin(), manifest->m_Shaders.end() );
        }
      }
    }

    
    ShaderUsagesAttributePtr shaderUsages = DangerousCast< ShaderUsagesAttribute >( assetClass->GetAttribute( Reflect::GetType< ShaderUsagesAttribute >() ) );
    if ( !shaderUsages.ReferencesObject() )
    {
      // add the shader usages attribute
      shaderUsages = new Asset::ShaderUsagesAttribute();
    }

    // now modify the shader usage attribute to have an entry per shader
    // adding and removing entries as necessary

    // first remove the shader usages that are no longer valid
    V_ShaderUsage validUsages;
    V_ShaderUsage::iterator itr = shaderUsages->m_ShaderUsages.begin();
    V_ShaderUsage::iterator end = shaderUsages->m_ShaderUsages.end();

    for ( ; itr != end; ++itr )
    {
      if ( currentShaderIds.find( (*itr)->m_ShaderID ) != currentShaderIds.end() )
      {
        currentShaderIds.erase( (*itr)->m_ShaderID );
        validUsages.push_back( (*itr) );
      }
    }

    S_tuid::iterator newItr = currentShaderIds.begin();
    S_tuid::iterator newEnd = currentShaderIds.end();

    for ( ; newItr != newEnd; ++newItr )
    {
      const tuid& shaderID = (*newItr);
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

#pragma TODO ("Get the screen shot")
}

/////////////////////////////////////////////////////////////////////////////
AssetFile* AssetTrackerVisitor::GetCurrentAssetFile()
{
  if ( !m_VisitedIDStack.empty() )
  {
    const tuid& assetID = m_VisitedIDStack.back();

    M_AssetFiles::iterator found = m_AssetFiles->find( assetID );
    NOC_ASSERT( found != m_AssetFiles->end() );

    return found->second;
  }

  return NULL;
}

/////////////////////////////////////////////////////////////////////////////
void AssetTrackerVisitor::PreHandleFile( AssetFile* assetFile )
{
  AddFileUsage( assetFile->GetFileID() );
  PushID( assetFile->GetFileID() );
}

/////////////////////////////////////////////////////////////////////////////
void AssetTrackerVisitor::PostHandleFile( AssetFile* assetFile )
{
  PopID();

  if ( m_AssetDB->AssetHasChangedOnDisc( assetFile->GetManagedFile() ) )
  {
    m_AssetDB->InsertAssetFile( assetFile, m_AssetFiles );
  }
}

/////////////////////////////////////////////////////////////////////////////
void AssetTrackerVisitor::AddFileUsage( tuid dependencyID )
{
  AssetFile* assetFile = GetCurrentAssetFile();
  if ( assetFile )
  {
    assetFile->AddDependencyID( dependencyID );
  }
}

/////////////////////////////////////////////////////////////////////////////
void AssetTrackerVisitor::AddAttribute( Reflect::Element* element )
{
  AssetFile* assetFile = GetCurrentAssetFile();
  if ( assetFile )
  {
    const std::string& attrName = Reflect::Registry::GetInstance()->GetClass( element->GetType() )->m_FullName;
    
    std::stringstream attrValueStream;
    Reflect::Archive::ToStream( element, attrValueStream, Reflect::ArchiveTypes::XML );
    
    assetFile->AddAttribute( attrName, attrValueStream.str() );
  }
}

/////////////////////////////////////////////////////////////////////////////
void AssetTrackerVisitor::PushID( tuid id )
{
  m_VisitedIDStack.push_back( id );
}

/////////////////////////////////////////////////////////////////////////////
void AssetTrackerVisitor::PopID()
{
  if ( m_VisitedIDStack.empty() )
  {
    NOC_BREAK();
  }

  m_VisitedIDStack.pop_back();
}