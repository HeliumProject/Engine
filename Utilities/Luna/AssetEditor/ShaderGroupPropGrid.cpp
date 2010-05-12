#include "Precompile.h"
#include "ShaderGroupPropGrid.h"

#include "AssetEditorIDs.h"
#include "AssetClass.h"
#include "ShaderGroupDlg.h"

#include "Attribute/AttributeHandle.h"
#include "Asset/ColorMapAttribute.h"
#include "Asset/ShaderAsset.h"
#include "File/Manager.h"
#include "FileBrowser/PropertyGridFilePicker.h"
#include "FileSystem/FileSystem.h"
#include "Finder/AssetSpecs.h"
#include "UIToolKit/FileDialog.h"
#include "UIToolKit/PropertyGridAttribs.h"


// Using
using namespace Luna;


// Properties displayed on each page of the property grid
enum Property
{
  PropertyName,
  PropertyDescription,
  PropertyOriginalPath,
  PropertyNewPath,

  PropertyCount
};

static const char* g_PropNames[ PropertyCount ] = 
{
  "Name",
  "Description",
  "Original Path",
  "New Path"
};

// Categories used in this property grid
enum Category
{
  CategorySettings,
  CategoryShaders,

  CategoryCount
};

static const char* g_CategoryNames[ CategoryCount ] = 
{
  "Settings",
  "Shaders"
};


///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ShaderGroupPropGrid::ShaderGroupPropGrid( wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxChar* name )
: wxPropertyGridManager( parent, id, pos, size, style, name )
, m_Dialog( NULL )
{
  Connect( GetId(), wxEVT_PG_CHANGED, wxPropertyGridEventHandler( ShaderGroupPropGrid::OnPropertyGridChange ), NULL, this );
  Connect( GetId(), wxEVT_PG_SELECTED, wxPropertyGridEventHandler( ShaderGroupPropGrid::OnPropertySelected ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ShaderGroupPropGrid::~ShaderGroupPropGrid()
{
  Disconnect( GetId(), wxEVT_PG_CHANGED, wxPropertyGridEventHandler( ShaderGroupPropGrid::OnPropertyGridChange ), NULL, this );
  Disconnect( GetId(), wxEVT_PG_SELECTED, wxPropertyGridEventHandler( ShaderGroupPropGrid::OnPropertySelected ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
// Call this function after creating the grid, and only call it once.
// 
void ShaderGroupPropGrid::Initialize( ShaderGroupDlg* dialog, Asset::ShaderUsagesAttributePtr shaderUsages, Asset::ShaderGroupAttributePtr shaderGroup )
{
  // Only call this function once
  NOC_ASSERT( !m_Dialog );

  // Only call this function with valid attributes
  NOC_ASSERT( dialog );
  NOC_ASSERT( shaderGroup.ReferencesObject() );

  if ( !m_Dialog )
  {
    m_Dialog = dialog;
    m_ShaderUsagesAttribute = shaderUsages;
    m_ShaderGroupAttribute = shaderGroup;

    Asset::V_ShaderGroupSmartPtr::const_iterator groupItr = m_ShaderGroupAttribute->m_ShaderGroups.begin();
    Asset::V_ShaderGroupSmartPtr::const_iterator groupEnd = m_ShaderGroupAttribute->m_ShaderGroups.end();
    for ( ; groupItr != groupEnd; ++groupItr )
    {
      const i32 pageIndex = AddPage( ( *groupItr )->m_Name.c_str() );
      SelectPage( pageIndex );
      MakePage( *groupItr );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Validates that the information that the user has entered is valid.  Returns
// true if everything is ok, otherwise returns false.  If false is returned,
// a description of the problem will be returned in the error parameter.
// 
bool ShaderGroupPropGrid::Validate( std::string& error )
{
  bool isValid = true;
  error.clear();
  const u32 numPages = static_cast< u32 >( GetPageCount() );

  for ( u32 pageIndex = 0; pageIndex < numPages && isValid; ++pageIndex )
  {
    wxPropertyGridPage* page = GetPage( pageIndex );
    if ( page )
    {
      wxPropertyGridIterator itr = page->GetIterator( wxPG_ITERATE_PROPERTIES );
      for ( ; !itr.AtEnd() && isValid; ++itr )
      {
        wxPGProperty* prop = *itr;
        const wxString& label = prop->GetLabel();

        if ( label == g_PropNames[PropertyName] )
        {
          if ( prop->GetValueAsString().empty() )
          {
            error = "Please specify a name for this shader group.";
            isValid = false;
            SelectPage( pageIndex );
            break;
          }
        }
        else if ( label == g_PropNames[PropertyNewPath] )
        {
          std::string path = prop->GetValueAsString().c_str();
          if ( !path.empty() )
          {
            tuid fileID = File::GlobalManager().GetID( path );
            if ( fileID == TUID::Null )
            {
              error = "Unable to create a file ID for '" + path + "'.";
              isValid = false;
              SelectPage( pageIndex );
              break;
            }
          }
        }
      }
    }
  }

  return isValid;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the attribute that stores all data manipulated by this control.
// 
Asset::ShaderGroupAttributePtr ShaderGroupPropGrid::GetShaderGroupAttribute() const
{
  return m_ShaderGroupAttribute;
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to build a property grid page for a specific shader group.
// 
void ShaderGroupPropGrid::MakePage( Asset::ShaderGroupPtr shaderGroup )
{
  NOC_ASSERT( m_ShaderGroupAttribute.ReferencesObject() );
  NOC_ASSERT( shaderGroup.ReferencesObject() );

  // Name
  wxPGProperty* settingsCategory = Append( new wxPropertyCategory( g_CategoryNames[CategorySettings] ) );
  AppendIn( settingsCategory, new wxStringProperty( g_PropNames[PropertyName], wxPG_LABEL, shaderGroup->m_Name.c_str() ) );

  // Description
  AppendIn( settingsCategory, new wxLongStringProperty( g_PropNames[PropertyDescription], wxPG_LABEL, shaderGroup->m_Description.c_str() ) );
  wxPGProperty* shadersCategory = Append( new wxPropertyCategory( g_CategoryNames[CategoryShaders] ) );

  // Shaders
  if ( m_ShaderUsagesAttribute.ReferencesObject() )
  {
    for each ( const Asset::ShaderUsagePtr& shaderUsage in m_ShaderUsagesAttribute->m_ShaderUsages )
    {
      if ( shaderUsage->m_ShaderGroupName.empty() )
      {
        const std::string shaderPath = File::GlobalManager().GetPath( shaderUsage->m_ShaderID );
        std::string shaderName;
        FileSystem::GetLeaf( shaderPath, shaderName );
        FileSystem::StripExtension( shaderName );
        wxPGProperty* category = AppendIn( shadersCategory, new wxPropertyCategory( shaderName.c_str() ) );

        // Original Path
        std::string label;
        label = shaderName + '|' + g_PropNames[PropertyOriginalPath];
        wxPGId origPathProp = AppendIn( category, new wxStringProperty( g_PropNames[PropertyOriginalPath], label, shaderPath.c_str() ) );
        SetPropertyReadOnly( origPathProp );

        // New Path
        label = shaderName + '|' + g_PropNames[PropertyNewPath];
        std::string newPath = shaderPath;
        M_tuid::iterator found = shaderGroup->m_ShaderMapping.find( shaderUsage->m_ShaderID );
        if ( found != shaderGroup->m_ShaderMapping.end() )
        {
          if ( !File::GlobalManager().GetPath( found->second, newPath ) )
          {
            std::string hint( "Check the shader group that references " );
            hint += shaderPath + ".";
            File::MissingFileException noThrow( found->second, "Shader", "", hint );
            wxMessageBox( noThrow.what(), "Error", wxCENTER | wxICON_ERROR | wxOK, GetParent() );
          }
        }
        wxPGProperty* newPathProp = AppendIn( category, new FileBrowser::PropertyGridFilePicker( g_PropNames[PropertyNewPath], label, newPath.c_str() ) );
        SetPropertyAttribute( newPathProp, UIToolKit::PropertyGridAttribs::DialogFlags, UIToolKit::FileDialogStyles::DefaultOpen );
        SetPropertyAttribute( newPathProp, UIToolKit::PropertyGridAttribs::DialogFilter, FinderSpecs::Asset::SHADER_DECORATION.GetDialogFilter().c_str() );
      };
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Updates the thumbnail image on the dialog with the color map texture of
// the specified shader.
// 
void ShaderGroupPropGrid::UpdateThumbnail( const std::string& shaderPath )
{
  std::string imagePath;
  if ( !shaderPath.empty() )
  {
    tuid assetID = File::GlobalManager().GetID( shaderPath );
    if ( assetID != TUID::Null )
    {
      Asset::ShaderAssetPtr shader = Asset::AssetClass::GetAssetClass< Asset::ShaderAsset >( assetID );
      if ( shader.ReferencesObject() )
      {
#pragma TODO( "Fix AttributeViewer so it can be used here instead." )
        Asset::ColorMapAttributePtr colorMap = Reflect::ObjectCast< Asset::ColorMapAttribute >( shader->GetAttribute( Reflect::GetType< Asset::ColorMapAttribute >() ) );
        if ( colorMap.ReferencesObject() && colorMap->m_Enabled )
        {
          if ( colorMap->m_FileID != TUID::Null )
          {
            File::GlobalManager().GetPath( colorMap->m_FileID, imagePath );
          }
        }
      }
    }
  }

  m_Dialog->SetThumbnail( imagePath );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the user chooses to add a new shader group.  Creates a 
// new property page that will contain data for the new shader group.
// 
void ShaderGroupPropGrid::OnAddShaderGroup( wxCommandEvent& args )
{
  NOC_ASSERT( GetPageCount() == m_ShaderGroupAttribute->m_ShaderGroups.size() );

  Asset::ShaderGroupPtr shaderGroup = new Asset::ShaderGroup();
  m_ShaderGroupAttribute->m_ShaderGroups.push_back( shaderGroup );

  std::stringstream stream;
  stream << ( GetPageCount() + 1 );
  std::string pageName( "Shader Group " );
  pageName += stream.str();

  shaderGroup->m_Name = pageName;

  const i32 index = AddPage( pageName.c_str() );
  SelectPage( index );
  MakePage( shaderGroup );
  Refresh();

  m_Dialog->UpdateChoices();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the delete button is pressed.  Deletes the shader group
// that is currently shown.
// 
void ShaderGroupPropGrid::OnDeleteShaderGroup( wxCommandEvent& args )
{
  NOC_ASSERT( GetPageCount() == m_ShaderGroupAttribute->m_ShaderGroups.size() );

  const i32 page = GetSelectedPage();
  if ( page >= 0 && GetPageCount() > 0 )
  {
    RemovePage( page );
    m_ShaderGroupAttribute->m_ShaderGroups.erase( m_ShaderGroupAttribute->m_ShaderGroups.begin() + page );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the user edits a field in the property grid.  Updates the
// internal attribute that stores this data.
// 
void ShaderGroupPropGrid::OnPropertyGridChange( wxPropertyGridEvent& args )
{
  // Ideally, we would like to set the name of the page here (as displayed in
  // the tooltips for the page button), but it looks like that is impossible.

  // Get the selected page and find the shader group that goes with it
  i32 pageIndex = GetSelectedPage();
  if ( pageIndex >= 0 && pageIndex < static_cast< i32 >( m_ShaderGroupAttribute->m_ShaderGroups.size() ) )
  {
    Asset::ShaderGroupPtr shaderGroup = m_ShaderGroupAttribute->m_ShaderGroups.at( pageIndex );

    if ( args.GetPropertyLabel() == g_PropNames[PropertyName] )
    {
      // Update shader group name
      shaderGroup->m_Name = args.GetPropertyValueAsString().c_str();
      m_Dialog->UpdateChoices();
    }
    else if ( args.GetPropertyLabel() == g_PropNames[PropertyDescription] )
    {
      // Update shader group description
      shaderGroup->m_Description = args.GetPropertyValueAsString().c_str();
    }
    else if ( args.GetPropertyLabel() == g_PropNames[PropertyNewPath] )
    {
      // Update one of the shader mappings
      wxPropertyGridIterator propItr = GetPage( pageIndex )->GetIterator( wxPG_ITERATE_PROPERTIES, args.GetProperty() );
      wxPGProperty* orig = *( --propItr );
      NOC_ASSERT( orig );
      NOC_ASSERT( orig->GetLabel() == g_PropNames[PropertyOriginalPath] );

      tuid originalID = File::GlobalManager().GetID( orig->GetValueAsString().c_str() );
      tuid newID = File::GlobalManager().GetID( args.GetPropertyValueAsString().c_str() );
      if ( originalID != TUID::Null )
      {
        if ( newID == TUID::Null || newID == originalID )
        {
          // Removed a shader mapping
          shaderGroup->m_ShaderMapping.erase( originalID );
        }
        else
        {
          // Changed/added a shader mapping
          shaderGroup->m_ShaderMapping[originalID] = newID;
        }
      }

      std::string path;
      if ( newID != TUID::Null )
      {
        File::GlobalManager().GetPath( newID, path );
      }

      UpdateThumbnail( path );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when selection changes in the property grid.  Updates the shader
// preview image to show the selected shader.
// 
void ShaderGroupPropGrid::OnPropertySelected( wxPropertyGridEvent& args )
{
  std::string path;
  if ( args.GetProperty() && 
    ( args.GetPropertyLabel() == g_PropNames[PropertyOriginalPath] ||
    args.GetPropertyLabel() == g_PropNames[PropertyNewPath] ) )
  {
    path = args.GetPropertyValueAsString().c_str();
  }
    
  UpdateThumbnail( path );
}
