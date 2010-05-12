#include "stdafx.h"
#include "GridPage.h"

#include "WizardPanels.h"
#include "ManagedAsset.h"

#include "Asset/AssetFlags.h"

#include "Common/Flags.h"

#include "FileUI/ManagedFileProperty.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "Finder/AssetSpecs.h"
#include "Finder/ExtensionSpecs.h"

#include "Console/Console.h"
#include "Reflect/Serializers.h"
#include "Reflect/Visitor.h"

#include "UIToolKit/FileDialog.h"
#include "UIToolKit/PropertyGridAttribs.h"


//
// Using
//

using namespace AssetManager;

using namespace Asset;
using namespace Asset::AssetFlags;


///////////////////////////////////////////////////////////////////////////////
// DisplayManagedAssetVisitor
//
// Used by GridPage to populate the property grid with the assets related files.
// 

//
// Constants, statics, etc.
//

// Titles for the file browsers
static const char* s_FileBrowserTitleDupe = "Save to new location";
static const char* s_FileBrowserTitleLink = "Reference an existing file";

// Delmiter for building out a property grid element's name
static const char* s_PropNameDelim = "|";

//////////////////////////////////////
// Property enums
namespace GridProperties
{
  enum GridProperty
  {
    Operation,
    OrigPath,
    NewPath,

    Count,
  };
}


//////////////////////////////////////
class Property
{
public:
  wxPGId          m_PId;
  std::string     m_Prefix;

  Property( wxPGId pid, const std::string& prefix )
    : m_PId( pid )
    , m_Prefix( prefix )
  {}
};
typedef std::stack< Property > PropertyStack;


//////////////////////////////////////
class PropertyName
{
public:
  std::string     m_Name;
  u32             m_Index;

  PropertyName( const std::string& name, const u32 index = 0 )
    : m_Name( name )
    , m_Index( index )
  {}
};
typedef std::vector< PropertyName > PropertyNameStack;


///////////////////////////////////////////////////////////////////////////////
class DisplayManagedAssetVisitor : public Reflect::Visitor
{
public:
  GridPage*               m_GridPage;
  ManagedAsset*           m_ManagedAsset;

  PropertyStack           m_PropertyStack;
  PropertyNameStack       m_PropertyNameStack;

  S_tuid                  m_AlreadyVisited;

  Reflect::ElementPtr     m_CurrentElement;


public:

  //
  // Ctro/Dtro
  //

  DisplayManagedAssetVisitor( GridPage* gridPage, ManagedAsset* managedAsset )
    : m_GridPage( gridPage )
    , m_ManagedAsset( managedAsset )
  {
    m_AlreadyVisited.insert( managedAsset->m_Asset->m_AssetClassID );

    std::string displayName;
    if ( m_GridPage->GetWizard()->ShouldUseNewPath() )
    {
      displayName = m_GridPage->GetWizard()->GetNewFileLocation();
      displayName = FileSystem::GetLeaf( displayName );
    }
    if ( displayName.empty() )
    {
      displayName = m_ManagedAsset->m_Asset->GetShortName();
    }
    std::string prefix = m_ManagedAsset->m_Asset->GetClass()->m_UIName + std::string( ": " ) + displayName;

    wxPGId pid = m_GridPage->m_PropGrid->Append( new wxPropertyCategory( prefix ) );
    m_PropertyStack.push( Property( pid, prefix ) );
    m_CurrentElement = managedAsset->m_Asset;
  }

  ~DisplayManagedAssetVisitor()
  {
    while ( !m_PropertyStack.empty() )
    {
      m_PropertyStack.pop();
    }

    m_PropertyNameStack.clear();

    m_AlreadyVisited.clear();

    m_CurrentElement = NULL;
  }


  /////////////////////////////////////////////////////////////////////////////
  void PushProperty( const std::string& name, const u32 index = 0 )
  {
    m_PropertyNameStack.push_back( PropertyName( name, index ) );
  }


  /////////////////////////////////////////////////////////////////////////////
  void PopProperty()
  {
    if ( !m_PropertyNameStack.empty() )
    {
      m_PropertyNameStack.pop_back();
    }
    else
    {
      if ( m_PropertyStack.empty() )
      {
        NOC_BREAK();
      }

      m_PropertyStack.pop();
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  void AppendInPropertyCategorys()
  {
    if ( m_PropertyNameStack.empty() )
      return;

    for each ( const PropertyName& propName in m_PropertyNameStack )
    {
      std::string prefix = m_PropertyStack.top().m_Prefix + std::string( s_PropNameDelim ) + propName.m_Name;

      if ( propName.m_Index > 0 )
      {
        std::stringstream number;
        number << propName.m_Index;
        prefix += number.str();
      }

      wxPGId pid = m_GridPage->m_PropGrid->AppendIn( m_PropertyStack.top().m_PId, new wxPropertyCategory( propName.m_Name.c_str(), prefix ) );

      m_PropertyStack.push( Property( pid, prefix ) );
    }

    m_PropertyNameStack.clear();
  }


  ///////////////////////////////////////////////////////////////////////////////
  // Helper function to build a unique name to a property grid element.
  inline std::string BuildEnlementPropertyName( const Reflect::ElementPtr& element )
  {
    if ( element->GetTitle().compare( element->GetClass()->m_UIName ) != 0 )
    {
      std::string propertyName = element->GetClass()->m_UIName + std::string( ": " ) + element->GetTitle();
      return propertyName;
    }

    return element->GetTitle();
  }


  /////////////////////////////////////////////////////////////////////////////
  virtual bool VisitElement( Reflect::Element* element ) NOC_OVERRIDE
  {
    if ( !m_CurrentElement || m_CurrentElement.Ptr() != element )
    {
      m_CurrentElement = element;

      PushProperty( BuildEnlementPropertyName( element ) );

      element->Host( *this );

      PopProperty();

      return false;
    }

    return true;
  }


  ///////////////////////////////////////////////////////////////////////////////
  // Helper function to build a unique name to a property grid element.
  inline std::string BuildPropertyName( const std::string& prefix, const std::string& append )
  {
    std::string propertyName = prefix + std::string( s_PropNameDelim ) + append;
    return propertyName;
  }


  /////////////////////////////////////////////////////////////////////////////
  void PopulateFile( const Reflect::Field* field, ManagedAssetFile* managedAssetFile, bool isList = false, u32 index = 0 )
  {
    // make this property name unique if it's in a list of file ids
    if ( isList )
    {
      PushProperty( field->m_UIName, index );
    }

    AppendInPropertyCategorys();

    wxPGId fieldId          = m_PropertyStack.top().m_PId;
    std::string fieldPrefix = m_PropertyStack.top().m_Prefix;

    if ( m_GridPage->m_ManagedAssetFileToProperties.count( managedAssetFile ) > 0 )
    {
      std::stringstream number;
      number << "-" << m_GridPage->m_ManagedAssetFileToProperties.count( managedAssetFile );
      fieldPrefix += number.str();
    }

    int propListSize = ( int ) m_GridPage->m_PropertyLabels.size();
    PropList propList( propListSize );

    // PerformOperation CheckBox
    if ( ( int ) GridProperties::Operation < propListSize )
    {
      std::string prefix = BuildPropertyName( fieldPrefix, m_GridPage->m_PropertyLabels.at( GridProperties::Operation ) );

      wxPGProperty* checkBoxProp = m_GridPage->m_PropGrid->AppendIn( fieldId, new wxBoolProperty( m_GridPage->m_PropertyLabels.at( GridProperties::Operation ).c_str(), prefix, Nocturnal::HasFlags<i32>( managedAssetFile->m_ConfigOptions, AssetFlags::PerformOperation ) ) );

      m_GridPage->m_PropGrid->SetPropertyAttribute( checkBoxProp, wxPG_BOOL_USE_CHECKBOX, (long)1 );
      m_GridPage->m_PropertyToManagedAssetFile.insert( M_PropToRename::value_type( checkBoxProp, managedAssetFile ) );
      propList.m_Property[GridProperties::Operation] = checkBoxProp;

      if ( Nocturnal::HasFlags<i32>( managedAssetFile->m_ConfigOptions, AssetFlags::CannotOverridePO ) )
      {
        m_GridPage->m_PropGrid->SetPropertyReadOnly( checkBoxProp );
      }
    }

    // File path
    if ( ( int ) GridProperties::OrigPath < propListSize )
    {
      std::string prefix = BuildPropertyName( fieldPrefix, m_GridPage->m_PropertyLabels.at( GridProperties::OrigPath ) );

      wxPGProperty* origPathProp = m_GridPage->m_PropGrid->AppendIn( fieldId, new wxStringProperty( m_GridPage->m_PropertyLabels.at( GridProperties::OrigPath ).c_str(), prefix, managedAssetFile->m_File->m_Path.c_str() ) );

      m_GridPage->m_PropGrid->SetPropertyReadOnly( origPathProp );
      m_GridPage->m_PropertyToManagedAssetFile.insert( M_PropToRename::value_type( origPathProp, managedAssetFile ) );
      propList.m_Property[GridProperties::OrigPath] = origPathProp;
    }

    // New file path
    if ( ( int ) GridProperties::NewPath < propListSize )
    {
      m_GridPage->m_ManagedAssetFilePathFields.insert( M_ManagedAssetFilePathField::value_type( managedAssetFile, ManagedAssetFilePathField( managedAssetFile->m_NewFile->m_Path ) ) );

      std::string prefix = BuildPropertyName( fieldPrefix, m_GridPage->m_PropertyLabels.at( GridProperties::NewPath ) );
      wxPGProperty* newPathProp = m_GridPage->m_PropGrid->AppendIn( fieldId, new File::ManagedFileProperty( m_GridPage->m_PropertyLabels.at( GridProperties::NewPath ).c_str(), prefix, managedAssetFile->m_NewFile->m_Path.c_str() ) );

      if ( Nocturnal::HasFlags<i32>( managedAssetFile->m_ConfigOptions, AssetFlags::PerformOperation ) )
      {
        m_GridPage->m_PropGrid->SetPropertyAttribute( newPathProp, UIToolKit::PropertyGridAttribs::DialogTitle, s_FileBrowserTitleDupe );
        m_GridPage->m_PropGrid->SetPropertyAttribute( newPathProp, UIToolKit::PropertyGridAttribs::DialogFlags, UIToolKit::FileDialogStyles::DefaultSave );

        m_GridPage->m_PropGrid->SetPropertyValue( newPathProp, managedAssetFile->m_NewFile->m_Path.c_str() );
      }
      else
      {
        m_GridPage->m_PropGrid->SetPropertyAttribute( newPathProp, UIToolKit::PropertyGridAttribs::DialogTitle, s_FileBrowserTitleLink );
        m_GridPage->m_PropGrid->SetPropertyAttribute( newPathProp, UIToolKit::PropertyGridAttribs::DialogFlags, UIToolKit::FileDialogStyles::DefaultOpen );

        managedAssetFile->m_NewFile->m_Path = managedAssetFile->m_File->m_Path;
        m_GridPage->m_PropGrid->SetPropertyValue( newPathProp, managedAssetFile->m_NewFile->m_Path.c_str() );
      }

      const Finder::FinderSpec* spec = NULL;
      std::string specName;
      if ( field->GetProperty( Asset::AssetProperties::FilterSpec, specName ) )
      {
        spec = Finder::GetFinderSpec( specName );
      }
      else if ( field->GetProperty( Asset::AssetProperties::ModifierSpec, specName ) )
      {
        spec = Finder::GetFinderSpec( specName );
      }

      if ( spec )
      {
        if ( !managedAssetFile->m_File->m_Path.empty() && spec )
        {
          std::stringstream filter;
          std::string ext;
          FileSystem::GetExtension( managedAssetFile->m_File->m_Path, ext );
          if ( !ext.empty() )
          {
            filter << spec->GetUIName() << " (" << ext << ")" << "|*" << ext << "|";
          }
          filter << spec->GetDialogFilter();
          m_GridPage->m_PropGrid->SetPropertyAttribute( newPathProp, UIToolKit::PropertyGridAttribs::DialogFilter, filter.str().c_str() );
        }
      }

      if ( Nocturnal::HasFlags<i32>( managedAssetFile->m_ConfigOptions, AssetFlags::CannotOverridePO ) )
      {
        m_GridPage->m_PropGrid->SetPropertyReadOnly( newPathProp );
      }

      m_GridPage->m_PropertyToManagedAssetFile.insert( M_PropToRename::value_type( newPathProp, managedAssetFile ) );
      propList.m_Property[GridProperties::NewPath] = newPathProp;
    }

    m_GridPage->m_ManagedAssetFileToProperties.insert( M_RenameToProp::value_type( managedAssetFile, propList ) );

    if ( isList )
    {
      PopProperty();
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  void PopulateField( const Reflect::ElementPtr& element, const Reflect::Field* field, tuid id, bool isList = false, u32 index = 0 )
  {
    NOC_ASSERT ( id != TUID::Null );
    if ( id == TUID::Null )
      return;

    M_ManagedAssetFiles::iterator findFile = m_ManagedAsset->m_ManagedAssetFiles.find( id );
    NOC_ASSERT ( findFile != m_ManagedAsset->m_ManagedAssetFiles.end() );
    if ( findFile == m_ManagedAsset->m_ManagedAssetFiles.end() )
      return;

    const ManagedAssetFilePtr& managedAssetFile = findFile->second;

    NOC_ASSERT ( managedAssetFile->m_File->m_Id != TUID::Null );
    if ( managedAssetFile->m_File->m_Id == TUID::Null )
      return;

    // this tuid may represent an asset file, but only explore it if we haven't already visited it
    Nocturnal::Insert<S_tuid>::Result insertedAssetId = m_AlreadyVisited.insert( managedAssetFile->m_File->m_Id );
    if ( insertedAssetId.second )
    {
      if ( FileSystem::HasExtension( managedAssetFile->m_File->m_Path, FinderSpecs::Extension::REFLECT_BINARY.GetExtension() ) )
      {
        Asset::AssetClassPtr assetClass = NULL;

          assetClass = Asset::AssetClass::FindAssetClass( managedAssetFile->m_File->m_Id );
          if ( assetClass.ReferencesObject() )
          {
            PushProperty( BuildEnlementPropertyName( assetClass ), index );

            PopulateFile( field, managedAssetFile, isList, index );

            m_CurrentElement = assetClass;
            assetClass->Host( *this );

            PopProperty();

            return;
          }
        }
    }

    return PopulateFile( field, managedAssetFile, isList, index );
  }

  /////////////////////////////////////////////////////////////////////////////
  virtual bool VisitField( Reflect::Element* element, const Reflect::Field* field ) NOC_OVERRIDE
  {
    if ( Nocturnal::HasFlags<i32>( field->m_Flags, Reflect::FieldFlags::Discard )
      || !Nocturnal::HasFlags<i32>( field->m_Flags, AssetFlags::ManageField  ) )
    {
      return true;
    }

    if ( ( field->m_Flags & Reflect::FieldFlags::FileID ) != 0 )
    {
      //-----------------------------------------------
      if ( field->m_SerializerID == Reflect::GetType< Reflect::U64Serializer >() )
      {
        tuid id;
        Reflect::Serializer::GetValue( field->CreateSerializer( element ), id );

        if ( id == TUID::Null )
          return true;

        if ( id == m_ManagedAsset->m_Asset->m_AssetClassID )
          return true;

        PopulateField( element, field, id );
      }
      //-----------------------------------------------
      else if ( field->m_SerializerID == Reflect::GetType< Reflect::U64ArraySerializer >() )
      {
        Reflect::SerializerPtr serializer = field->CreateSerializer( element );

        V_tuid vals;
        Reflect::Serializer::GetValue( serializer, vals );

        if ( (int)vals.size() < 1 )
          return true;

        bool isList = ( (int)vals.size() > 1 );

        u32 index = 0;
        for each ( const tuid& id in vals )
        {
          PopulateField( element, field, id, isList, index );
          ++index;
        }
      }
      //-----------------------------------------------
      else if ( field->m_SerializerID == Reflect::GetType< Reflect::U64SetSerializer >() )
      {
        Reflect::SerializerPtr serializer = field->CreateSerializer( element );

        S_tuid vals;
        Reflect::Serializer::GetValue( serializer, vals );

        if ( (int)vals.size() < 1 )
          return true;

        bool isList = ( (int)vals.size() > 1 );

        u32 index = 0;
        for each ( const tuid& id in vals )
        {
          PopulateField( element, field, id, isList, index );
          ++index;
        }
      }
      else
      {
        NOC_BREAK();
        throw File::Exception( "This field type '%s::%s' has no support for the file manager",
          field->m_Type->m_ShortName.c_str(), field->m_Name.c_str() );
      }
    }
    //-----------------------------------------------
    else if ( field->m_SerializerID == Reflect::GetType< Reflect::ElementArraySerializer >() )
    {
      Reflect::ElementArraySerializerPtr arraySerializer = Reflect::DangerousCast< Reflect::ElementArraySerializer >( field->CreateSerializer( element ) );
      Reflect::V_Element& vals = arraySerializer->m_Data.Ref();

      if ( (int)vals.size() < 1 )
        return true;

      bool isList = ( (int)vals.size() > 1 );

      u32 index = 0;
      for each ( const Reflect::ElementPtr& memberElem in vals )
      {
        PushProperty( memberElem->GetClass()->m_UIName, index );

        m_CurrentElement = memberElem;
        memberElem->Host( *this );

        PopProperty();

        ++index;
      }

      return false;
    }

    // continue search
    return true;
  }
};


///////////////////////////////////////////////////////////////////////////////
// GridPage
// 

//
// Constants, statics, etc.
//

// Title of the dialog
static const char* s_PageTitle = "Associated Files";
static const char* s_PageDescription = "Below are the files associated with this asset. You can choose whether or not to include any of these files in the operation.";

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
GridPage::GridPage( Wizard* wizard )
: WizardPage< GridPanel >( wizard )
, m_PropGrid( m_Panel->m_PropertyGrid )
, m_ManagedAsset( GetWizard()->GetManagedAsset() )
{
  // Set up the title & description
  m_Panel->m_Title->SetLabel( s_PageTitle );

  m_Panel->m_Description->SetLabel( s_PageDescription );
  m_Panel->m_Description->Wrap( m_Panel->GetMinWidth() - 10 );
  m_Panel->Layout();

  ConnectListeners();
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
GridPage::~GridPage()
{
  DisconnectListeners();
}


///////////////////////////////////////////////////////////////////////////////
// Static creation function.  Returns a new (heap-allocated) instance of this 
// class.
// 
UIToolKit::WizardPage* GridPage::Create( Wizard* wizard )
{
  return new GridPage( wizard );
}

void GridPage::ConnectListeners()
{
  Connect( m_PropGrid->GetId(), wxEVT_PG_CHANGED, wxPropertyGridEventHandler( GridPage::OnPropertyGridChange ), NULL, this );
  Connect( m_Panel->m_SelectAllButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GridPage::OnSelectAllButtonClick ), NULL, this );
  Connect( m_Panel->m_UnselectAllButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GridPage::OnUnselectAllButtonClick ), NULL, this );	
}

void GridPage::DisconnectListeners()
{
  Disconnect( m_PropGrid->GetId(), wxEVT_PG_CHANGED, wxPropertyGridEventHandler( GridPage::OnPropertyGridChange ), NULL, this );
  Disconnect( m_Panel->m_SelectAllButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GridPage::OnSelectAllButtonClick ), NULL, this );
  Disconnect( m_Panel->m_UnselectAllButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GridPage::OnUnselectAllButtonClick ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
// Called just before the window is shown.  Populates the UI.
// 
bool GridPage::TransferDataToWindow()
{
  // Only do validation if traversing to the next wizard page
  if ( GetWizard()->GetDirection() != UIToolKit::PageDirections::Forward )
  {
    return true;
  }

  wxBusyCursor bc;

  DisconnectListeners();

  // Clear the property grid control so that it can be recreated.
  if ( m_PropGrid->GetPageCount() > 0 )
  {
    m_PropGrid->RemovePage( 0 );
    m_PropertyToManagedAssetFile.clear();
    m_ManagedAssetFileToProperties.clear();
  }

  // Populate the grid
  // Only get the rename asset if traversing to the next wizard page
  if ( GetWizard()->GetDirection() == UIToolKit::PageDirections::Forward )
  {
    AssetManager::GetManagedAsset( GetWizard()->GetManagedAsset(), false );

    m_PropGrid->AddPage( m_DlgTitle.c_str() );

    DisplayManagedAssetVisitor displayManagedAssetVisitor( this, m_ManagedAsset );
    m_ManagedAsset->m_Asset->Host( displayManagedAssetVisitor );

    m_PropGrid->SetSplitterLeft();
  }

  ConnectListeners();

  return true;
}


///////////////////////////////////////////////////////////////////////////////
// Called when switching pages.  Transfers data from the UI to the data managed
// by the wizard class.
// 
bool GridPage::TransferDataFromWindow()
{
  wxBusyCursor bc;

  DisconnectListeners();

  // Only do validation if traversing to the next wizard page
  if ( GetWizard()->GetDirection() != UIToolKit::PageDirections::Forward )
  {
    ConnectListeners();
    return true;
  }

  GetManagedAssetRelatedFiles( m_ManagedAsset );

  std::string errorMessage;
  if ( !ValidateAssetOperation( m_ManagedAsset, errorMessage ) )
  {
    wxMessageBox( errorMessage.c_str(), "Error", wxCENTER | wxOK | wxICON_ERROR, GetParent() );

    ConnectListeners();
    return false;
  }


  ///////////////////////////////////////

  // Set the summary page info and list of files (if any)
  m_PageSummary.clear();
  m_P4FileList.clear();

  V_string relatedFiles;

  bool useNewPath = GetWizard()->ShouldUseNewPath();

  for each ( const M_ManagedAssetFiles::value_type& fileIndex in GetWizard()->GetManagedAsset()->m_ManagedAssetFiles )
  {
    const ManagedAssetFilePtr& managedAssetFile = fileIndex.second;
    // early out if it's not duplicating the old file
    if ( !Nocturnal::HasFlags<i32>( managedAssetFile->m_ConfigOptions, AssetFlags::PerformOperation ) )
    {
      continue;
    }

    if ( useNewPath )
    {
      m_P4FileList.push_back( managedAssetFile->m_NewFile->m_Path );
    }
    else
    {
      m_P4FileList.push_back( managedAssetFile->m_File->m_Path );
    }

    for each ( const M_string::value_type& relatedFilePair in managedAssetFile->m_RelatedFiles )
    {
      if ( useNewPath )
      {
        relatedFiles.push_back( relatedFilePair.second );
      }
      else
      {
        relatedFiles.push_back( relatedFilePair.first );
      }
    }
  }

  FileSystem::SortFiles( m_P4FileList );

  std::string newFilesStr;
  for each ( const std::string& newFile in m_P4FileList )
  {
    newFilesStr += "  o " + newFile + "\n";
  }


  FileSystem::SortFiles( relatedFiles );

  std::string relatedFilesStr;
  for each ( const std::string& relatedFile in relatedFiles )
  {
    relatedFilesStr += "  o " + relatedFile + "\n";
  }

  // Additional Assets
  m_PageSummary.push_back( StyledText( &m_TitleTextAttr, "Additional assets: \n" ) );
  m_PageSummary.push_back( StyledText( &m_DefaultTextAttr, 
    ( !newFilesStr.empty() ? newFilesStr : "  o No additional assets will be affected.\n" ) ) );

  // Related files:
  m_PageSummary.push_back( StyledText( &m_TitleTextAttr, "\nRelated files: \n" ) );
  m_PageSummary.push_back( StyledText( &m_DefaultTextAttr, 
    ( !relatedFilesStr.empty() ? relatedFilesStr : "  o No related files will be affected.\n" ) ) );

  // append the related files
  m_P4FileList.insert( m_P4FileList.end(), relatedFiles.begin(), relatedFiles.end() );

  ConnectListeners();

  return true;
}




///////////////////////////////////////////////////////////////////////////////
// Callback for when an item in the property grid is changed.
// 
void GridPage::OnPropertyGridChange( wxPropertyGridEvent& args )
{
  M_PropToRename::iterator foundFile = m_PropertyToManagedAssetFile.find( args.GetProperty() );
  if ( foundFile == m_PropertyToManagedAssetFile.end() )
  {
    return;
  }


  DisconnectListeners();

  std::string label( args.GetPropertyLabel().c_str() );

  //------------------------------------------------------------------------
  if ( label == m_PropertyLabels.at( GridProperties::Operation ) )
  {
    ManagedAssetFile* managedAssetFile = foundFile->second;
    bool performOperation = m_PropGrid->GetPropertyValueAsBool( args.GetProperty() );

    if ( ( m_ManagedAssetFileToProperties.count( managedAssetFile ) == 1 )
      || ( ( m_ManagedAssetFileToProperties.count( managedAssetFile ) > 1 )
      && ( wxMessageBox( "More than one attribute references this file, this change will be applied to all references to this file, would you still like to make this change?",
      "Modify all references?", wxYES_NO | wxCENTER | wxICON_QUESTION, this ) == wxYES ) ) )
    {
      UpdatePerformOperation( managedAssetFile, performOperation );
    }
    else
    {
      m_PropGrid->SetPropertyValue( args.GetProperty(), !performOperation );
    }
  }
  //------------------------------------------------------------------------
  else if ( label == m_PropertyLabels.at( GridProperties::OrigPath ) )
  {
    // This property should NOT be editable by the user.
    NOC_BREAK();
  }
  //------------------------------------------------------------------------
  else if ( label == m_PropertyLabels.at( GridProperties::NewPath ) )
  {
    ManagedAssetFile* managedAssetFile = foundFile->second;

    M_ManagedAssetFilePathField::iterator fieldItr = m_ManagedAssetFilePathFields.find( managedAssetFile );
    NOC_ASSERT( fieldItr != m_ManagedAssetFilePathFields.end() );

    ManagedAssetFilePathField& managedAssetFilePathField = fieldItr->second;

    if ( ( m_ManagedAssetFileToProperties.count( managedAssetFile ) == 1 )
      || ( ( m_ManagedAssetFileToProperties.count( managedAssetFile ) > 1 )
      && ( wxMessageBox( "More than one attribute references this file, this change will be applied to all references to this file, would you still like to make this change?",
      "Modify all references?", wxYES_NO | wxCENTER | wxICON_QUESTION, this ) == wxYES ) ) )
    {
      bool performOperation = Nocturnal::HasFlags<i32>( managedAssetFile->m_ConfigOptions, AssetFlags::PerformOperation );

      // get the user specified path and clean it
      managedAssetFile->m_NewFile->m_Path = args.GetPropertyValueAsString().c_str();
      FileSystem::CleanName( managedAssetFile->m_NewFile->m_Path );

      bool reset = managedAssetFile->m_NewFile->m_Path.empty();

      // the user entered an empty file path, reset it to the default value
      if ( reset )
      {
        managedAssetFile->m_NewFile->m_Path = performOperation ? managedAssetFilePathField.m_SuggestPath : managedAssetFile->m_File->m_Path ;
      }

      // save that the file path was edited, but only if we didn't reset it
      if ( performOperation )
      {
        managedAssetFilePathField.m_NewPathEdited = !reset;
      }
      else
      {
        managedAssetFilePathField.m_ReferencePathEdited = !reset;
      }

      M_RenameToProp::const_iterator itr = m_ManagedAssetFileToProperties.lower_bound( managedAssetFile );
      M_RenameToProp::const_iterator end = m_ManagedAssetFileToProperties.upper_bound( managedAssetFile );

      for ( ; itr != end ; ++itr )
      {    
        const PropList& propList = itr->second;

        M_Property::const_iterator findNewPathPGProperty = propList.m_Property.find( GridProperties::NewPath );
        if ( findNewPathPGProperty != propList.m_Property.end() )
        {
          wxPGProperty* prop( findNewPathPGProperty->second );
          m_PropGrid->SetPropertyValue( prop, managedAssetFile->m_NewFile->m_Path.c_str() );
        }
      }
    }
  }

  ConnectListeners();
}


///////////////////////////////////////////////////////////////////////////////
void GridPage::UpdatePerformOperation( ManagedAssetFile* managedAssetFile, bool performOperation )
{ 
  M_ManagedAssetFilePathField::iterator fieldItr = m_ManagedAssetFilePathFields.find( managedAssetFile );
  NOC_ASSERT( fieldItr != m_ManagedAssetFilePathFields.end() );

  M_RenameToProp::const_iterator itr = m_ManagedAssetFileToProperties.lower_bound( managedAssetFile );
  M_RenameToProp::const_iterator end = m_ManagedAssetFileToProperties.upper_bound( managedAssetFile );

  bool useNewPath = GetWizard()->ShouldUseNewPath();

  for ( ; itr != end ; ++itr )
  {    
    const PropList& propList = itr->second;

    M_Property::const_iterator findOperationPGProperty = propList.m_Property.find( GridProperties::Operation );
    if ( findOperationPGProperty != propList.m_Property.end() )
    {
      wxPGId opID( findOperationPGProperty->second );

      if ( performOperation )
      {
        m_PropGrid->SetPropertyValue( opID, performOperation );
      }
      else
      {
        m_PropGrid->SetPropertyValue( opID, performOperation );
      }
    }

    if ( useNewPath )
    {
      M_ManagedAssetFilePathField::iterator fieldItr = m_ManagedAssetFilePathFields.find( managedAssetFile );
      NOC_ASSERT( fieldItr != m_ManagedAssetFilePathFields.end() );

      ManagedAssetFilePathField& managedAssetFilePathField = fieldItr->second;

      M_Property::const_iterator findNewPathPGProperty = propList.m_Property.find( GridProperties::NewPath );
      if ( findNewPathPGProperty != propList.m_Property.end() )
      {
        wxPGId newPathPId( findNewPathPGProperty->second );

        if ( performOperation )
        {
          m_PropGrid->SetPropertyAttribute( newPathPId, UIToolKit::PropertyGridAttribs::DialogTitle, s_FileBrowserTitleDupe );
          m_PropGrid->SetPropertyAttribute( newPathPId, UIToolKit::PropertyGridAttribs::DialogFlags, UIToolKit::FileDialogStyles::DefaultSave );

          if ( !managedAssetFilePathField.m_NewPathEdited )
          {
            managedAssetFile->m_NewFile->m_Path = managedAssetFilePathField.m_SuggestPath;
          }
          m_PropGrid->SetPropertyValue( newPathPId, managedAssetFile->m_NewFile->m_Path.c_str() );
        }
        else
        {
          m_PropGrid->SetPropertyAttribute( newPathPId, UIToolKit::PropertyGridAttribs::DialogTitle, s_FileBrowserTitleLink );
          m_PropGrid->SetPropertyAttribute( newPathPId, UIToolKit::PropertyGridAttribs::DialogFlags, UIToolKit::FileDialogStyles::DefaultOpen );
          m_PropGrid->SetPropertyValue( newPathPId, managedAssetFile->m_File->m_Path.c_str() );

          if ( !managedAssetFilePathField.m_ReferencePathEdited )
          {
            managedAssetFile->m_NewFile->m_Path = managedAssetFile->m_File->m_Path;
          }
          m_PropGrid->SetPropertyValue( newPathPId, managedAssetFile->m_NewFile->m_Path.c_str() );
        }
      }
    }
  }

  Nocturnal::SetFlag<i32>( managedAssetFile->m_ConfigOptions, AssetFlags::PerformOperation, performOperation );
}


///////////////////////////////////////////////////////////////////////////////
void GridPage::SelectAllFiles( bool select )
{ 
  DisconnectListeners();

  bool useNewPath = GetWizard()->ShouldUseNewPath();

  for each ( const M_ManagedAssetFiles::value_type& fileIndex in GetWizard()->GetManagedAsset()->m_ManagedAssetFiles )
  {
    const ManagedAssetFilePtr& managedAssetFile = fileIndex.second;

    UpdatePerformOperation( managedAssetFile, select );
  }

  ConnectListeners();
}

///////////////////////////////////////////////////////////////////////////////

void GridPage::OnSelectAllButtonClick( wxCommandEvent& event )
{ 
  SelectAllFiles( true );
}

void GridPage::OnUnselectAllButtonClick( wxCommandEvent& event )
{ 
  SelectAllFiles( false );
}
