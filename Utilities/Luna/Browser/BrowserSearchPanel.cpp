#include "Precompile.h"
#include "BrowserSearchPanel.h"

#include "Browser.h"
#include "BrowserSearchDatabase.h"
#include "CollectionManager.h"

#include "Asset/CacheDB.h"
#include "Common/String/Tokenize.h"
#include "Common/Container/Insert.h" 
#include "File/Manager.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "Finder/AssetSpecs.h"
#include "Finder/ExtensionSpecs.h"
#include "Finder/FinderSpec.h"
#include "Finder/LunaSpecs.h"
#include "Finder/ProjectSpecs.h"
#include "UIToolKit/ImageManager.h"

using namespace Luna;

static const char* s_WordsHelpText = "This will search for file paths that\ncontain all of these words, in any order.";
static const char* s_PhraseHelpText = "You can do this in standard search by\n\"surrounding your phrase with quotes\"";
static const char* s_FileTypeHelpText = "Search for files with this extension.";
static const char* s_FileIDHelpText = "Search by the file's ID (or TUID) using\nthe Hex or Decimal value.";
static const char* s_LevelHelpText = "Add a level name here to search for\nassets that are placed in a given level.";
static const char* s_ShaderHelpText = "Add a shader name here to only find\nassets that use the given shader.";
static const char* m_AttributeHelpText = "Here you can search for the value of\na given attribute.";

static const char* s_CollectionDefaultText = "<Select Collection>";
static const char* s_FileTypeDefaultText = "<All File Types>";
static const char* s_CreatedByDefaultText = "<Select User>";
static const char* s_EngineTypeDefaultText = "<Select Type>";
static const char* s_FileIDDefaultText = "<Hex or Decimal file ID>";
static const char* s_LevelDefaultText = "<Level Name>";
static const char* s_ShaderDefaultText = "<Shader Name>";
static const char* s_AttributeNameDefaultText = "<Select Attribute Name>";
static const char* s_AttributeValueDefaultText = "<Attribute Value>";



///////////////////////////////////////////////////////////////////////////// 
Filter::Filter( const std::string& name, const std::string& filters )
  : m_Name( name )
  , m_Filters( filters )
{
  if ( !m_Filters.empty() )
  {
    Tokenize( GetFilters(), m_Extensions, ";" );

    S_string::iterator it = m_Extensions.begin();
    S_string::iterator end = m_Extensions.end();
    for ( ; it != end ; ++it )
    {
      std::string& extension = (*it);
      if ( !extension.empty() && *extension.begin() == '*' )
      {
        extension.erase( 0, 1 );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
BrowserSearchPanel::BrowserSearchPanel( BrowserFrame* browserFrame, wxWindow* parent )
: SearchPanelGenerated( parent )
, m_BrowserFrame( browserFrame )
, m_CollectionManager( NULL )
, m_FieldMRU( new UIToolKit::FieldMRU() )
{
  m_GoButton->SetId( BrowserMenu::AdvancedSearchGo );
  m_CancelButton->SetId( BrowserMenu::AdvancedSearchCancel );

  m_WordsHelpBitmap->SetBitmap( UIToolKit::GlobalImageManager().GetBitmap( "help_16.png" ) );
  m_PhraseHelpBitmap->SetBitmap( UIToolKit::GlobalImageManager().GetBitmap( "help_16.png" ) );
  m_FileIDHelpBitmap->SetBitmap( UIToolKit::GlobalImageManager().GetBitmap( "help_16.png" ) );
  m_AttributeHelpBitmap->SetBitmap( UIToolKit::GlobalImageManager().GetBitmap( "help_16.png" ) );
  m_LevelHelpBitmap->SetBitmap( UIToolKit::GlobalImageManager().GetBitmap( "help_16.png" ) );
  m_ShaderHelpBitmap->SetBitmap( UIToolKit::GlobalImageManager().GetBitmap( "help_16.png" ) );

  m_DefaultFieldText.insert( std::make_pair( m_CollectionChoice->GetId(), s_CollectionDefaultText ) );
  m_DefaultFieldText.insert( std::make_pair( m_FileTypeChoice->GetId(), s_FileTypeDefaultText ) );
  m_DefaultFieldText.insert( std::make_pair( m_CreatedByComboBox->GetId(), s_CreatedByDefaultText ) );
  m_DefaultFieldText.insert( std::make_pair( m_EngineTypeChoice->GetId(), s_EngineTypeDefaultText ) );
  m_DefaultFieldText.insert( std::make_pair( m_FileIDTextCtrl->GetId(), s_FileIDDefaultText ) );
  m_DefaultFieldText.insert( std::make_pair( m_LevelTextCtrl->GetId(), s_LevelDefaultText ) );
  m_DefaultFieldText.insert( std::make_pair( m_ShaderTextCtrl->GetId(), s_ShaderDefaultText ) );
  m_DefaultFieldText.insert( std::make_pair( m_AttributeNameChoice->GetId(), s_AttributeNameDefaultText ) );
  m_DefaultFieldText.insert( std::make_pair( m_AttributeValueTextCtrl->GetId(), s_AttributeValueDefaultText ) );

  ConnectListners();

  PopulateForm();
}

BrowserSearchPanel::~BrowserSearchPanel()
{
  DisconnectListners();
}

void BrowserSearchPanel::ConnectListners()
{
  m_CollectionManager = m_BrowserFrame->GetBrowser()->GetBrowserPreferences()->GetCollectionManager();
  //m_CollectionManager->AddClearAllListener( CollectionManagerSignature::Delegate( this, &BrowserSearchPanel::OnCollectionManagerChanged ) );
  //m_CollectionManager->AddCollectionAddedListener( CollectionManagerSignature::Delegate( this, &BrowserSearchPanel::OnCollectionManagerChanged ) );
  //m_CollectionManager->AddCollectionRemovingListener( CollectionManagerSignature::Delegate( this, &BrowserSearchPanel::OnCollectionManagerChanged ) );
  m_CollectionManager->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &BrowserSearchPanel::OnCollectionManagerChanged ) );
}

void BrowserSearchPanel::DisconnectListners()
{
  //m_CollectionManager->RemoveClearAllListener( CollectionManagerSignature::Delegate( this, &BrowserSearchPanel::OnCollectionManagerChanged ) );
  //m_CollectionManager->RemoveCollectionAddedListener( CollectionManagerSignature::Delegate( this, &BrowserSearchPanel::OnCollectionManagerChanged ) );
  //m_CollectionManager->RemoveCollectionRemovingListener( CollectionManagerSignature::Delegate( this, &BrowserSearchPanel::OnCollectionManagerChanged ) );
  m_CollectionManager->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &BrowserSearchPanel::OnCollectionManagerChanged ) );
}

/////////////////////////////////////////////////////////////////////////////////
bool BrowserSearchPanel::Show( bool show )
{
  if ( show )
  {
    PopulateCollectionsChoice();
  }

  return __super::Show( show );
}

///////////////////////////////////////////////////////////////////////////////
void BrowserSearchPanel::OnFieldTextEnter( wxCommandEvent& event )
{
  // TODO: submit and hide the form
  event.Skip();
}

/////////////////////////////////////////////////////////////////////////////
// Called when the "Browse..." button is clicked. Opens the Directory browser
// modal dialog and sets the value of the folder.
void BrowserSearchPanel::OnFolderBrowseButtonClick( wxCommandEvent& event )
{
  // pass some initial dir to wxDirDialog
  wxString currentLookIn = m_FolderChoice->GetStringSelection();

  wxDirDialog dirDialog( this, wxT( "Select the directory to search in:" ), currentLookIn, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST );
  if ( dirDialog.ShowModal() == wxID_OK )
  {
    // clean the user input
    std::string cleanDirectory;
    FileSystem::CleanName( dirDialog.GetPath().c_str(), cleanDirectory );

    m_FieldMRU->AddItem( (wxControlWithItems*) m_FolderChoice, "m_FolderChoice", cleanDirectory );
  }
}

///////////////////////////////////////////////////////////////////////////////
void BrowserSearchPanel::OnFieldText( wxCommandEvent& event )
{
  //M_DefaultFieldText::const_iterator findDefaultText = m_DefaultFieldText.find( event.GetId() );
  //if ( findDefaultText != m_DefaultFieldText.end() )
  //int eventID = event.GetId();
  //if ( eventID == m_CreatedByComboBox->GetId() )
  //{
  //  if ( m_CreatedByComboBox->GetValue().empty() )
  //  {
  //  }
  //}
  //else if ( eventID == m_EngineTypeChoice->GetId() )
  //{
  //  if ( m_EngineTypeChoice->GetValue().empty() )
  //  {
  //  }
  //}
  //else if ( eventID == m_FileIDTextCtrl->GetId() )
  //{
  //  if ( m_FileIDTextCtrl->GetValue().empty() )
  //  {
  //  }
  //}
  //else if ( eventID == m_LevelTextCtrl->GetId() )
  //{
  //  if ( m_LevelTextCtrl->GetValue().empty() )
  //  {
  //  }
  //}
  //else if ( eventID == m_ShaderTextCtrl->GetId() )
  //{
  //  if ( m_ShaderTextCtrl->GetValue().empty() )
  //  {
  //  }
  //}
}

///////////////////////////////////////////////////////////////////////////////
void BrowserSearchPanel::OnSearchButtonClick( wxCommandEvent& event )
{
  // TODO: submit and hide the form
  if ( ProcessForm() )
  {
    
    event.Skip();
  }
  else
  {
    //event.Veto();
  }
}

///////////////////////////////////////////////////////////////////////////////
void BrowserSearchPanel::OnCancelButtonClick( wxCommandEvent& event )
{
  // TODO: Cancel and hide the form
  event.Skip();
}

/////////////////////////////////////////////////////////////////////////////
void BrowserSearchPanel::OnCollectionManagerChanged( const Reflect::ElementChangeArgs& args )
{
  PopulateCollectionsChoice();
}

/////////////////////////////////////////////////////////////////////////////
// Adds a FinderSpec filter to the current list of filters.
//
void BrowserSearchPanel::AddFilter( const Finder::FinderSpec& filterSpec )
{
  AddFilter( filterSpec.GetDialogFilter().c_str() );
}

/////////////////////////////////////////////////////////////////////////////
// Adds a string filter with the following format to the current list of filters.
//   "<Display String>|<Extension Mask>"
//
// For example, this input:
//  "BMP and GIF files (*.bmp;*.gif)|*.bmp;*.gif|PNG files (*.png)|*.png"
//
// Becomes this map
//  "BMP and GIF files (*.bmp;*.gif)" -> "*.bmp;*.gif"
//  "PNG files (*.png)" -> "*.png"
//
void BrowserSearchPanel::AddFilter( const char* filter )
{
  V_string splitFilter;
  Tokenize( filter, splitFilter, "\\|" );

  const size_t numTokens = splitFilter.size();
  if ( numTokens % 2 != 0 )
    return; // error

  for ( size_t i = 0; i < numTokens; i+=2 )
  {
    bool inserted = m_Filters.Append( Filter( splitFilter.at( i ), splitFilter.at( i+1 ) ) ); 
  }
}

/////////////////////////////////////////////////////////////////////////////
void BrowserSearchPanel::SetFilterIndex( const Finder::FinderSpec& spec )
{
  size_t index = 0;

  V_string splitFilter;
  Tokenize( spec.GetDialogFilter(), splitFilter, "\\|" );

  if ( (int)splitFilter.size() % 2 != 0 )
    return; // error

  OS_Filter::Iterator itr = m_Filters.Begin();
  OS_Filter::Iterator end = m_Filters.End();
  for ( size_t count = 0; itr != end; ++itr, ++count )
  {
    const Filter& filter = *itr;
    if ( filter.GetName() == splitFilter.at( 0 ) )
    {
      index = count;
      break;
    }
  }

  SetFilterIndex( static_cast< int >( index ) );
}

/////////////////////////////////////////////////////////////////////////////
void BrowserSearchPanel::SetFilterIndex( int filterIndex )
{
  m_FileTypeChoice->SetSelection( filterIndex );
}

/////////////////////////////////////////////////////////////////////////////
// returns the currently selected filter eg: "*.*", "*.png;*.jpg", etc...
//
std::string BrowserSearchPanel::GetFilter()
{
  std::string filter = "";

  // file type filter
  std::string selectedFilter = m_FileTypeChoice->GetStringSelection().c_str();
  const Filter* foundFilter = FindFilter( selectedFilter );
  if ( foundFilter )
  {
    filter = foundFilter->GetFilters();
  }

  return filter;
}

/////////////////////////////////////////////////////////////////////////////
// Updates the current display list of filters and adds "All files (*.*)|*.*" filter
// if the current list is empty.
//
void BrowserSearchPanel::UpdateFilters()
{
  m_Filters.Append( Filter( "All files (*.*)", "*.*" ) );

  m_FileTypeChoice->Clear();

  // first insert the DefaultString:
  M_DefaultFieldText::const_iterator findDefaultText = m_DefaultFieldText.find( m_FileTypeChoice->GetId() );
  if ( findDefaultText != m_DefaultFieldText.end() )
  {
    m_FileTypeChoice->AppendString( findDefaultText->second.c_str() );
    m_FileTypeChoice->AppendString( "" );
  }

  OS_Filter::Iterator it     = m_Filters.Begin();
  OS_Filter::Iterator itEnd  = m_Filters.End();
  for ( ; it != itEnd ; ++it )
  {
    m_FileTypeChoice->Append( ( *it ).GetName().c_str() );
  }

  m_FileTypeChoice->SetSelection( 0 );
}

/////////////////////////////////////////////////////////////////////////////
// Helper function to find a filter by name
const Filter* BrowserSearchPanel::FindFilter( const std::string& name )
{
  OS_Filter::Iterator itr = m_Filters.Begin();
  OS_Filter::Iterator end = m_Filters.End();
  for ( ; itr != end; ++itr )
  {
    const Filter& filter = *itr;
    if ( filter.GetName() == name )
    {
      return &filter;
    }
  }
  return NULL;
}


struct SortChoices
{
  bool operator()( const std::string& lhs, const std::string& rhs ) const
  {
    return _stricmp( lhs.c_str(), rhs.c_str() ) < 0;
  }
};

///////////////////////////////////////////////////////////////////////////////
void BrowserSearchPanel::PopulateForm()
{
  DisconnectListners();

  Freeze();

  // populate combo boxes: m_CreatedByComboBox, m_EngineTypeChoice
  V_string tableData;
  u32 numAdded = m_BrowserFrame->GetBrowser()->GetCacheDB()->GetPopulateTableData( (u32)Asset::CacheDBColumnIDs::FileType, tableData );
  if ( numAdded > 0 )
  {
    PopulateFileTypeChoice( tableData );
  }
  else
  {
    m_FileTypeChoice->Enable( false );
  }

  tableData.clear();
  numAdded = m_BrowserFrame->GetBrowser()->GetCacheDB()->GetPopulateTableData( (u32)Asset::CacheDBColumnIDs::P4User, tableData );
  if ( numAdded > 0 )
  {
    PopulateChoiceControl( (wxControlWithItems*) m_CreatedByComboBox, tableData );
  }
  else
  {
    m_CreatedByComboBox->Enable( false );
  }

  tableData.clear();
  numAdded = m_BrowserFrame->GetBrowser()->GetCacheDB()->GetPopulateTableData( (u32)Asset::CacheDBColumnIDs::EngineType, tableData );
  if ( numAdded > 0 )
  {
    PopulateChoiceControl( (wxControlWithItems*) m_EngineTypeChoice, tableData );
  }
  else
  {
    m_EngineTypeChoice->Enable( false );
  }


  tableData.clear();
  numAdded = m_BrowserFrame->GetBrowser()->GetCacheDB()->GetAttributesTableData( tableData );
  if ( numAdded > 0 )
  {
    PopulateChoiceControl( (wxControlWithItems*) m_AttributeNameChoice, tableData );
  }
  else
  {
    m_AttributeNameChoice->Enable( false );
    m_AttributeValueTextCtrl->Enable( false );
  }

  // populate from MRU: m_FolderChoice
  m_FieldMRU->PopulateControl( (wxControlWithItems*) m_FolderChoice, "m_FolderChoice", Finder::ProjectAssets() );

  Thaw();

  ConnectListners();
}

///////////////////////////////////////////////////////////////////////////////
void BrowserSearchPanel::PopulateChoiceControl( wxControlWithItems* control, V_string& contents )
{
  control->Clear();

  wxArrayString items;
  items.reserve( contents.size() + 1 );

  // first insert the DefaultString:
  M_DefaultFieldText::const_iterator findDefaultText = m_DefaultFieldText.find( control->GetId() );
  if ( findDefaultText != m_DefaultFieldText.end() )
  {
    items.push_back( findDefaultText->second.c_str() );
    items.push_back( "" );
  }

  // then insert the list
  std::sort( contents.begin(), contents.end(), SortChoices() );
  V_string::const_iterator itr = contents.begin();
  V_string::const_iterator end = contents.end();
  for ( ; itr != end ; ++itr )
  {
    const std::string& value = (*itr);

    if ( value.empty() )
      continue;

    if ( ( _stricmp( value.c_str(), "null" ) == 0 )
      || ( _stricmp( value.c_str(), "unknown" ) == 0 ) )
    {
      continue;
    }

    items.push_back( value.c_str() );
  }

  control->Freeze();
  control->Append( items );
  control->SetSelection( 0 );
  control->Thaw();
}

///////////////////////////////////////////////////////////////////////////////
void BrowserSearchPanel::PopulateFileTypeChoice( V_string& contents )
{
  // then insert the list
  std::sort( contents.begin(), contents.end(), SortChoices() );
  V_string::const_iterator itr = contents.begin();
  V_string::const_iterator end = contents.end();
  for ( ; itr != end ; ++itr )
  {
    const std::string& value = (*itr);

    if ( value.empty() )
      continue;

    if ( ( _stricmp( value.c_str(), "null" ) == 0 )
      || ( _stricmp( value.c_str(), "unknown" ) == 0 ) )
    {
      continue;
    }

    try
    {
      const Finder::ModifierSpec* modifierSpec = Finder::GetFileExtensionSpec( value );
      if ( modifierSpec )
      {
        AddFilter( *modifierSpec );
      }
    }
    catch ( const Nocturnal::Exception& )
    {
      // do nothing
    }
  }

  UpdateFilters();
  m_FileTypeChoice->SetSelection( 0 );
}

void BrowserSearchPanel::PopulateCollectionsChoice()
{
  u32 numAdded = 0;
  V_string tableData;
  for ( M_AssetCollections::const_iterator itr = m_CollectionManager->GetCollections().begin(),
    end = m_CollectionManager->GetCollections().end(); itr != end; ++itr )
  {    
    AssetCollection* collection = itr->second;
    if ( collection && !collection->GetAssetIDs().empty() )
    {
      tableData.push_back( collection->GetName() );
      ++numAdded;
    }
  }

  if ( numAdded > 0 )
  {
    tableData.push_back( "" );
    PopulateChoiceControl( (wxControlWithItems*) m_CollectionChoice, tableData );
    m_SearchCollectionRadio->Enable( true );
    m_CollectionChoice->Enable( true );
  }
  else
  {
    m_SearchVaultRadio->SetValue( true );
    m_SearchCollectionRadio->SetValue( false );
    m_SearchCollectionRadio->Enable( false );
    m_CollectionChoice->Enable( false );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Collects and error checks the search criteria from the form, and populates
// the struct m_SearchCriteria. Returns true if no errors were found. Also
// stores the MRU for combo box form fields
// 
// Parse: Fields
// Update: MRUs
// Set: m_GeneratedQueryTextCtrl
//
bool BrowserSearchPanel::ProcessForm()
{
  wxBusyCursor bc;
  
#pragma TODO( "Rachel: Need more error checking in BrowserSearchPanel::GetQueryString" )

  std::string queryString = "";
  AssetCollection* collection = NULL;

  wxString fieldStringValue;
  std::string cleanFieldValue;

  // -----------------------------------------
  if ( m_SearchCollectionRadio->GetValue() )
  {
    fieldStringValue = m_CollectionChoice->GetStringSelection();
    fieldStringValue.Trim(true);  // trim white-space right 
    fieldStringValue.Trim(false); // trim white-space left
    if ( !fieldStringValue.empty() )
    {
      collection = m_CollectionManager->FindCollection( fieldStringValue.c_str() );
      if ( collection )
      {
        queryString += queryString.empty() ? "" : " ";
        queryString += collection->GetQueryString();
      }
      else
      {
        return false;
      }
    }
  }

  // -----------------------------------------
  // Asset File Path/Name
  //  wxTextCtrl* m_WordsTextCtrl;
  fieldStringValue = m_WordsTextCtrl->GetValue();
  fieldStringValue.Trim(true);  // trim white-space right 
  fieldStringValue.Trim(false); // trim white-space left
  if ( !fieldStringValue.empty() )
  {
    V_string splitValue;
    Tokenize( fieldStringValue.c_str(), splitValue, " " );

    V_string::const_iterator itr = splitValue.begin();
    V_string::const_iterator end = splitValue.end();
    for ( ; itr != end ; ++itr )
    {
      const std::string& value = (*itr);
      FileSystem::CleanName( value, cleanFieldValue );
      bool isPath = true; //( cleanFieldValue.find( '/' ) != std::string::npos ) ? true : false; 

      queryString += queryString.empty() ? "" : " ";
      queryString += isPath 
        ? Asset::CacheDBColumnIDs::Column( Asset::CacheDBColumnIDs::Path )
        : Asset::CacheDBColumnIDs::Column( Asset::CacheDBColumnIDs::Name );
      queryString += ":";
      queryString += cleanFieldValue;
      queryString += "";
    }
  }

  //  wxTextCtrl* m_PhraseTextCtrl;
  fieldStringValue = m_PhraseTextCtrl->GetValue();
  fieldStringValue.Trim(true);  // trim white-space right 
  fieldStringValue.Trim(false); // trim white-space left
  if ( !fieldStringValue.empty() )
  {
    FileSystem::CleanName( fieldStringValue.c_str(), cleanFieldValue );
    bool isPath = true; //( cleanFieldValue.find( '/' ) != std::string::npos ) ? true : false;
    bool needsQuotes = ( cleanFieldValue.find( ' ' ) != std::string::npos ) ? true : false;

    queryString += queryString.empty() ? "" : " ";
    queryString += isPath 
      ? Asset::CacheDBColumnIDs::Column( Asset::CacheDBColumnIDs::Path )
      : Asset::CacheDBColumnIDs::Column( Asset::CacheDBColumnIDs::Name );
    queryString += ":";
    queryString += needsQuotes ? "\"" : "";
    queryString += cleanFieldValue;
    queryString += needsQuotes ? "\"" : "";
  }

  // File type filter-------------------------------
  //  wxComboBox* m_FileTypeChoice;
  fieldStringValue = m_FileTypeChoice->GetStringSelection().c_str();
  fieldStringValue.Trim(true);  // trim white-space right 
  fieldStringValue.Trim(false); // trim white-space left
  if ( !fieldStringValue.empty()
    && _stricmp( fieldStringValue.c_str(), s_FileTypeDefaultText  ) != 0 
    && _stricmp( fieldStringValue.c_str(), "All files (*.*)"  ) != 0 )
  {
    const Filter* foundFilter = FindFilter( fieldStringValue.c_str() );
    if ( foundFilter != NULL && !foundFilter->GetExtensions().empty() )
    {
      queryString += queryString.empty() ? "" : " ";
      queryString += Asset::CacheDBColumnIDs::Column( Asset::CacheDBColumnIDs::FileType ) + ":";
      queryString += "\"*";
      queryString += *(foundFilter->GetExtensions().begin());
      queryString += "\"";
    }
    else
    {
      wxMessageBox( "The file extension you specified is not valid.", "Error", wxCENTER | wxOK | wxICON_ERROR, GetParent() );
      return false;
    }
  }

  //  wxChoice*   m_FolderChoice;
  fieldStringValue = m_FolderChoice->GetStringSelection();
  fieldStringValue.Trim(true);  // trim white-space right 
  fieldStringValue.Trim(false); // trim white-space left
  if ( !fieldStringValue.empty() )
  {
    FileSystem::CleanName( fieldStringValue.c_str(), cleanFieldValue );
    FileSystem::StripPrefix( File::GlobalManager().GetManagedAssetsRoot(), cleanFieldValue );

    if ( !cleanFieldValue.empty() )
    {
      bool needsQuotes = ( cleanFieldValue.find( ' ' ) != std::string::npos ) ? true : false;
    
      queryString += queryString.empty() ? "" : " ";
      queryString += Asset::CacheDBColumnIDs::Column( Asset::CacheDBColumnIDs::Path ) + ":";
      queryString += needsQuotes ? "\"" : "";
      queryString += cleanFieldValue;
      queryString += "*\"";
    }
  }

  // -----------------------------------------
  // Additional search criteria
  //  wxComboBox* m_CreatedByComboBox; 
  fieldStringValue = m_CreatedByComboBox->GetStringSelection();
  fieldStringValue.Trim(true);  // trim white-space right 
  fieldStringValue.Trim(false); // trim white-space left
  if ( !fieldStringValue.empty()
    && _stricmp( fieldStringValue.c_str(), s_CreatedByDefaultText  ) != 0 )
  {
    queryString += queryString.empty() ? "" : " ";
    queryString += Asset::CacheDBColumnIDs::Column( Asset::CacheDBColumnIDs::P4User ) + ":";
    queryString += fieldStringValue.c_str();
    queryString += "";
  }


  // -----------------------------------------
  //  wxChoice*   m_EngineTypeChoice;
  fieldStringValue = m_EngineTypeChoice->GetStringSelection();
  fieldStringValue.Trim(true);  // trim white-space right 
  fieldStringValue.Trim(false); // trim white-space left
  if ( !fieldStringValue.empty()
    && _stricmp( fieldStringValue.c_str(), s_EngineTypeDefaultText  ) != 0 )
  {
    bool needsQuotes = ( fieldStringValue.find( ' ' ) != -1 ) ? true : false;

    queryString += queryString.empty() ? "" : " ";
    queryString += Asset::CacheDBColumnIDs::Column( Asset::CacheDBColumnIDs::EngineType ) + ":";
    queryString += needsQuotes ? "\"" : "";
    queryString += fieldStringValue.c_str();
    queryString += needsQuotes ? "\"" : "";
  }

  // -----------------------------------------
  //  wxTextCtrl* m_FileIDTextCtrl;
  fieldStringValue = m_FileIDTextCtrl->GetValue();
  fieldStringValue.Trim(true);  // trim white-space right 
  fieldStringValue.Trim(false); // trim white-space left
  if ( !fieldStringValue.empty()
    && _stricmp( fieldStringValue.c_str(), s_FileIDDefaultText  ) != 0 )
  {
    queryString += queryString.empty() ? "" : " ";
    queryString += Asset::CacheDBColumnIDs::Column( Asset::CacheDBColumnIDs::FileID ) + ":";
    queryString += fieldStringValue.c_str();
    queryString += "";
  }

  // -----------------------------------------
  //  wxChoice*   m_AttributeNameChoice;
  //  wxTextCtrl* m_AttributeValueTextCtrl
  fieldStringValue = m_AttributeNameChoice->GetStringSelection();
  fieldStringValue.Trim(true);  // trim white-space right 
  fieldStringValue.Trim(false); // trim white-space left
  if ( !fieldStringValue.empty()
    && _stricmp( fieldStringValue.c_str(), s_AttributeNameDefaultText  ) != 0 )
  {
    cleanFieldValue = fieldStringValue.c_str();
    
    fieldStringValue = m_AttributeValueTextCtrl->GetValue();
    fieldStringValue.Trim(true);  // trim white-space right 
    fieldStringValue.Trim(false); // trim white-space left
    if ( !fieldStringValue.empty()
      && _stricmp( fieldStringValue.c_str(), s_AttributeValueDefaultText  ) != 0 )
    {
      bool needsQuotes = ( fieldStringValue.find( ' ' ) != -1 ) ? true : false;

      queryString += queryString.empty() ? "" : " ";
      queryString += cleanFieldValue + ":";
      queryString += needsQuotes ? "\"" : "";
      queryString += fieldStringValue.c_str();
      queryString += needsQuotes ? "\"" : "";
    }
    else
    {
      wxMessageBox( "Please specify an attribute value to search for.", "Error", wxCENTER | wxOK | wxICON_ERROR, GetParent() );
      return false;
    }
  }

  // -----------------------------------------
  //  wxTextCtrl* m_LevelTextCtrl;
  fieldStringValue = m_LevelTextCtrl->GetValue();
  fieldStringValue.Trim(true);  // trim white-space right 
  fieldStringValue.Trim(false); // trim white-space left
  if ( !fieldStringValue.empty()
    && _stricmp( fieldStringValue.c_str(), s_LevelDefaultText  ) != 0 )
  {
    FileSystem::CleanName( fieldStringValue.c_str(), cleanFieldValue );
    bool needsQuotes = ( cleanFieldValue.find( ' ' ) != std::string::npos ) ? true : false;

    queryString += queryString.empty() ? "" : " ";
    queryString += Asset::CacheDBColumnIDs::Column( Asset::CacheDBColumnIDs::Level ) + ":";
    queryString += needsQuotes ? "\"" : "";
    queryString += cleanFieldValue;
    queryString += needsQuotes ? "\"" : "";
  }

  // -----------------------------------------
  //  wxTextCtrl* m_ShaderTextCtrl;
  fieldStringValue = m_ShaderTextCtrl->GetValue();
  fieldStringValue.Trim(true);  // trim white-space right 
  fieldStringValue.Trim(false); // trim white-space left
  if ( !fieldStringValue.empty()
    && _stricmp( fieldStringValue.c_str(), s_ShaderDefaultText  ) != 0 )
  {
    FileSystem::CleanName( fieldStringValue.c_str(), cleanFieldValue );
    bool needsQuotes = ( cleanFieldValue.find( ' ' ) != std::string::npos ) ? true : false;

    queryString += queryString.empty() ? "" : " ";
    queryString += Asset::CacheDBColumnIDs::Column( Asset::CacheDBColumnIDs::Shader ) + ":";
    queryString += needsQuotes ? "\"" : "" ;
    queryString += cleanFieldValue;
    queryString += needsQuotes ? "\"" : "" ;
  }

  m_BrowserFrame->Search( queryString, collection );

  // save a query that is entered through the advanced search wizard
  if( !queryString.empty() )
  {
    BrowserSearchDatabase::UpdateSearchEvents( queryString );
  }

  return true;
}

  //m_SearchCriteria->clear();

  //if ( m_checkBoxDiskFiles->IsChecked() )
  //{
  //  m_SearchCriteria->m_SearchMode = m_SearchCriteria->m_SearchMode | SearchModes::FilesOnDisk;
  //}

  //// File History Data --------------------------
  //m_SearchCriteria->m_SearchHistoryData = m_checkBoxSeachHistoryData->IsChecked();

  //// Level Tracker-------------------------------
  //wxString comboBoxLevel = m_comboBoxLevel->GetValue();
  //if ( !comboBoxLevel.empty() )
  //{
  //  m_SearchCriteria->m_SearchMode = m_SearchCriteria->m_SearchMode | SearchModes::Level;
  //  m_SearchCriteria->m_Level = comboBoxLevel.c_str();
  //}

  //// Update Class-------------------------------
  //wxString comboBoxUpdateClass = m_comboBoxUpdateClass->GetValue();
  //if ( !comboBoxUpdateClass.empty() )
  //{
  //  m_SearchCriteria->m_SearchMode = m_SearchCriteria->m_SearchMode | SearchModes::UpdateClass;
  //  m_SearchCriteria->m_UpdateClass = comboBoxUpdateClass.c_str();
  //}

  //// Created by-------------------------------
  //wxString comboBoxUserCreatedBy = m_comboBoxUserCreatedBy->GetValue();
  //if ( !comboBoxUserCreatedBy.empty() )
  //{
  //  m_SearchCriteria->m_SearchMode = m_SearchCriteria->m_SearchMode | SearchModes::CreatedBy;
  //  m_SearchCriteria->m_UserCreatedBy = comboBoxUserCreatedBy.c_str();
  //}
  //g_FieldMRU->AddItem( (wxControlWithItems*) m_comboBoxUserCreatedBy, "m_comboBoxUserCreatedBy", m_SearchCriteria->m_UserCreatedBy );
  // 

  //// TUID-------------------------------
  //std::string strTUID = m_comboBoxTUID->GetValue().c_str();
  //if ( !strTUID.empty() )
  //{
  //  std::istringstream idStream (strTUID);
  //  tuid id;
  //  
  //  // if the TUID is in HEX
  //  if ( strTUID.size() > 2 && ( strTUID[0] == '0' && (strTUID[1] == 'x' || strTUID[1] == 'X' )))
  //  {
  //    idStream >> std::hex >> id;
  //  }
  //  else
  //  {
  //    idStream >> id;
  //  }

  //  if ( id == 0 || id == _UI64_MAX )
  //  {
  //    wxMessageBox( "The asset ID you specified is not valid.", "Error", wxCENTER | wxOK | wxICON_ERROR, GetParent() );
  //    return false;
  //  }

  //  m_SearchCriteria->m_SearchMode = m_SearchCriteria->m_SearchMode | SearchModes::AssetID;
  //  m_SearchCriteria->m_TUID = id;
  //}
  //g_FieldMRU->AddItem( (wxControlWithItems*) m_comboBoxTUID, "m_comboBoxTUID", strTUID );


  //// Legacy asset class-------------------------------
  //std::string legacyID =  m_comboBoxLegacyID->GetValue().c_str();
  //if ( !legacyID.empty() )
  //{
  //  m_SearchCriteria->m_LegacyID = atoi( legacyID.c_str() );
  //  if ( m_SearchCriteria->m_LegacyID < 0 )
  //  {
  //    wxMessageBox( "Invalid legacy asset ID specified.", "Error", wxCENTER | wxOK | wxICON_ERROR, GetParent() );
  //    return false;
  //  }
  //  
  //  m_SearchCriteria->m_LegacyType =  m_choiceLegacyType->GetStringSelection().c_str();
  //  if ( ( m_SearchCriteria->m_LegacyType != "level" )
  //    && ( m_SearchCriteria->m_LegacyType != "moby" )
  //    && ( m_SearchCriteria->m_LegacyType != "tie" )
  //    && ( m_SearchCriteria->m_LegacyType != "shader" )
  //    && ( m_SearchCriteria->m_LegacyType != "foliage" )
  //    && ( m_SearchCriteria->m_LegacyType != "shrub" ) 
  //    && ( m_SearchCriteria->m_LegacyType != "sky" ) )
  //  {
  //    wxMessageBox( "Please specify a legacy asset class.", "Error", wxCENTER | wxOK | wxICON_ERROR, GetParent() );
  //    return false;
  //  }
  //  
  //  m_SearchCriteria->m_SearchMode = m_SearchCriteria->m_SearchMode | SearchModes::LegacyInfo;
  //}
  //g_FieldMRU->AddItem( (wxControlWithItems*) m_comboBoxLegacyID, "m_comboBoxLegacyID", legacyID );


  //// Look in folder-------------------------------
  //wxString comboBoxLookIn = m_FolderChoice->GetValue();
  //if ( !comboBoxLookIn.empty() )
  //{
  //  m_SearchCriteria->m_LookIn = comboBoxLookIn.c_str();

  //  // clean user input
  //  FileSystem::CleanName( m_SearchCriteria->m_LookIn );
  //  FileSystem::GuaranteeSlash( m_SearchCriteria->m_LookIn ); 
  //  g_FieldMRU->AddItem( (wxControlWithItems*) m_FolderChoice, "m_FolderChoice", m_SearchCriteria->m_LookIn );  
  //}
  //

  //// Search Query-------------------------------
  //wxString comboBoxSearch = m_comboBoxSearch->GetValue();
  //comboBoxSearch.Trim(true);  // trim white-space right 
  //comboBoxSearch.Trim(false); // trim white-space left
  //if ( !comboBoxSearch.empty() )
  //{
  //  m_SearchCriteria->m_SearchMode = m_SearchCriteria->m_SearchMode | SearchModes::SearchQuery;
  //  m_SearchCriteria->m_SearchQuery = comboBoxSearch.c_str();
  //}
  //g_FieldMRU->AddItem( (wxControlWithItems*) m_comboBoxSearch, "m_comboBoxSearch", m_SearchCriteria->m_SearchQuery );


  //// File type filter-------------------------------
  //std::string filter = m_choiceFiletype->GetStringSelection().c_str();
  //const Filter* foundFilter = FindFilter( filter );
  //if ( foundFilter != NULL )
  //{
  //  m_SearchCriteria->m_Filter = foundFilter->GetExtensions();
  //}
  //else
  //{
  //  wxMessageBox( "The file extension you specified is not valid.", "Error", wxCENTER | wxOK | wxICON_ERROR, GetParent() );
  //  return false;
  //}


  //// get the search queries-------------------------------
  //std::string searchQuery = "";
  //if ( m_SearchCriteria->m_SearchMode & SearchModes::SearchQuery )
  //{
  //  searchQuery = !m_SearchCriteria->m_SearchQuery.empty() ? m_SearchCriteria->m_SearchQuery : "*";
  //}
  //
  //if ( !FileSystem::HasPrefix( m_SearchCriteria->m_LookIn, searchQuery ) )
  //{
  //  searchQuery = m_SearchCriteria->m_LookIn + std::string( "*" ) + searchQuery;
  //}
  //

  //// if they provided an extension that's part of this filter, add it specifically
  //std::string queryExtension = FileSystem::GetExtension( searchQuery );
  //for each ( const std::string& itFilter in m_SearchCriteria->m_Filter )
  //{
  //  // rat.en => [.entity.irb].find( [.en] )
  //  if ( !queryExtension.empty() && itFilter.find( queryExtension ) == 0 )
  //  {
  //    std::string addQuery = searchQuery;
  //    FileSystem::StripExtension( addQuery );
  //    addQuery += itFilter;
  //    m_SearchCriteria->m_SearchQueries.insert( m_SearchCriteria->m_SearchQueries.begin(), 1, addQuery );
  //}

  //  m_SearchCriteria->m_SearchQueries.push_back( searchQuery + std::string("*") + itFilter );
  //}
