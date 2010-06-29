#pragma once

#include "Pipeline/API.h"

//#include "CacheDB.h"

#include "Pipeline/Asset/AssetClass.h"
#include "Pipeline/Asset/AssetFile.h"
#include "Pipeline/Asset/AssetFlags.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Platform/Types.h"
#include "Foundation/Reflect/Field.h"
#include "Foundation/Reflect/Version.h"

//
// Forwards
//
namespace Content
{
  class Region;
  typedef Nocturnal::SmartPtr<Region> RegionPtr; 

  class Zone;
  typedef Nocturnal::SmartPtr< Zone > ZonePtr;
  typedef std::vector< ZonePtr > V_Zone;
}

namespace Asset
{
  class AssetVisitor;

  typedef bool (AssetVisitor::*ElementHandlerFunc)( Reflect::Element* element );
  typedef std::map< i32, ElementHandlerFunc > ElementHandlerLookup;

  typedef void (AssetVisitor::*FileHandlerFunc)( Reflect::Element* element, const Reflect::Field* field, AssetFile* assetFile );
  typedef std::map< tstring, FileHandlerFunc > FileHandlerLookup;

  /////////////////////////////////////////////////////////////////////////////
  class PIPELINE_API AssetVisitor : public Reflect::Visitor
  {   
  public:
    AssetVisitor( M_AssetFiles* assetFiles, Asset::AssetClass* assetClass, bool* stopRequested = NULL );
    virtual ~AssetVisitor();

    virtual bool VisitElement( Reflect::Element* element ) NOC_OVERRIDE;
    virtual bool VisitField( Reflect::Element* element, const Reflect::Field* field ) NOC_OVERRIDE;

  protected:

    //
    // Members
    //
    Asset::AssetClass*    m_AssetClass;
    Reflect::Element*     m_CurrentElement;
    M_AssetFiles*         m_AssetFiles;
    bool*                 m_StopRequested;
    
    
    
    //
    // Default Handlers
    //
    bool HandleElement( Reflect::Element* element );
    bool HandleField( Reflect::Element* element, const Reflect::Field* field );
    void HandlePath( Reflect::Element* element, const Reflect::Field* field, Nocturnal::Path& path );

    
    //
    // Custom Handlers
    //

    // Elements
    std::set< i32 > m_ElementFilterTypes; // TODO: Add more element filters
    ElementHandlerLookup m_ElementHandlerLookup;
    bool HandleArtFileComponent( Reflect::Element* element );

    // Fields
    std::set< i32 > m_FieldFilterTypes;   // TODO: Add more field filters

    // Files
    //std::set< i32 > m_FileFilterTypes;  // TODO: Determine if we need to add file filters
    FileHandlerLookup m_FileHandlerLookup;
    void HandleAssetFile( Reflect::Element* element, const Reflect::Field* field, AssetFile* assetFile );
    void HandleWorldFile( Reflect::Element* element, const Reflect::Field* field, AssetFile* assetFile );
    void HandleZoneFile( Reflect::Element* element, const Reflect::Field* field, AssetFile* assetFile );
    void HandleMayaFile( Reflect::Element* element, const Reflect::Field* field, AssetFile* assetFile );

    
    //
    // Maintain ID stack & other helpers
    //
    typedef std::vector< u64 > HashStack;
    HashStack  m_VisitedHashStack;

    AssetFile* GetCurrentAssetFile();

    void PreHandleFile( AssetFile* assetFile );
    void PostHandleFile( AssetFile* assetFile );

    void AddDependency( AssetFile* assetFile );

    void PushHash( const u64& assetHash );
    void PopHash();
  };

} // namespace Asset
