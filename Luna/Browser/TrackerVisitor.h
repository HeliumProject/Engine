#pragma once

#include "API.h"

#include "AssetDB.h"
#include "AssetFile.h"

#include "Asset/AssetClass.h"
#include "Asset/AssetFlags.h"
#include "Common/Memory/SmartPtr.h"
#include "Common/Types.h"
#include "File/Manager.h"
#include "Reflect/Field.h"
#include "Reflect/Version.h"

namespace Asset
{
  class AssetTrackerVisitor;

  typedef bool (AssetTrackerVisitor::*ElementHandlerFunc)( Reflect::Element* element );
  typedef std::map< i32, ElementHandlerFunc > ElementHandlerLookup;

  typedef void (AssetTrackerVisitor::*FileHandlerFunc)( Reflect::Element* element, const Reflect::Field* field, AssetFile* assetFile );
  typedef std::map< const Finder::ModifierSpec*, FileHandlerFunc > FileHandlerLookup;

  /////////////////////////////////////////////////////////////////////////////
  class AssetTrackerVisitor : public Reflect::Visitor
  {   
  public:
    AssetTrackerVisitor( M_AssetFiles* assetFiles, Asset::AssetClass* assetClass, Asset::AssetDBPtr assetDB );
    virtual ~AssetTrackerVisitor();

    virtual bool VisitElement( Reflect::Element* element ) NOC_OVERRIDE;
    virtual bool VisitField( Reflect::Element* element, const Reflect::Field* field ) NOC_OVERRIDE;

  protected:

    //
    // Members
    //
    Asset::AssetClass*    m_AssetClass;
    Reflect::Element*     m_CurrentElement;
    M_AssetFiles*         m_AssetFiles;
    AssetDBPtr            m_AssetDB;
    
    
    //
    // Default Handlers
    //
    bool HandleElement( Reflect::Element* element );
    bool HandleField( Reflect::Element* element, const Reflect::Field* field );
    void HandleFileID( Reflect::Element* element, const Reflect::Field* field, tuid id );

    
    //
    // Custom Handlers
    //

    // Elements
    S_i32 m_ElementFilterTypes; // TODO: Add more element filters
    ElementHandlerLookup m_ElementHandlerLookup;
    bool HandleUpdateClassAttribute( Reflect::Element* element );
    bool HandleArtFileAttribute( Reflect::Element* element );

    // Fields
    S_i32 m_FieldFilterTypes;   // TODO: Add more field filters

    // Files
    //S_i32 m_FileFilterTypes;  // TODO: Determine if we need to add file filters
    FileHandlerLookup m_FileHandlerLookup;
    void HandleAssetFile( Reflect::Element* element, const Reflect::Field* field, AssetFile* assetFile );
    void HandleWorldFile( Reflect::Element* element, const Reflect::Field* field, AssetFile* assetFile );
    void HandleZoneFile( Reflect::Element* element, const Reflect::Field* field, AssetFile* assetFile );
    void HandleLevelFile( Reflect::Element* element, const Reflect::Field* field, AssetFile* assetFile );
    void HandleMayaFile( Reflect::Element* element, const Reflect::Field* field, AssetFile* assetFile );

    
    //
    // Maintain ID stack & other helpers
    //
    typedef std::vector<tuid> IDStack;
    IDStack  m_VisitedIDStack;

    AssetFile* GetCurrentAssetFile();

    void PreHandleFile( AssetFile* assetFile );
    void PostHandleFile( AssetFile* assetFile );

    void AddFileUsage( tuid id );
    void AddAttribute( Reflect::Element* element );

    void PushID( tuid id );
    void PopID();
  };

} // namespace Asset
