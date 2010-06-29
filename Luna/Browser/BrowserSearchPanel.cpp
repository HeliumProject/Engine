#include "Precompile.h"
#include "BrowserSearchPanel.h"

#include "Browser.h"
#include "CollectionManager.h"

#include "Foundation/File/Path.h"
#include "Foundation/String/Tokenize.h"
#include "Foundation/Container/Insert.h" 
#include "Application/UI/ImageManager.h"

using namespace Luna;

static const tchar* s_WordsHelpText = TXT( "This will search for file paths that\ncontain all of these words, in any order." );
static const tchar* s_PhraseHelpText = TXT( "You can do this in standard search by\n\"surrounding your phrase with quotes\"" );
static const tchar* s_FileTypeHelpText = TXT( "Search for files with this extension." );
static const tchar* s_FileIDHelpText = TXT( "Search by the file's ID (or TUID) using\nthe Hex or Decimal value." );
static const tchar* s_LevelHelpText = TXT( "Add a level name here to search for\nassets that are placed in a given level." );
static const tchar* s_ShaderHelpText = TXT( "Add a shader name here to only find\nassets that use the given shader." );
static const tchar* m_ComponentHelpText = TXT( "Here you can search for the value of\na given attribute." );

static const tchar* s_CollectionDefaultText = TXT( "<Select Collection>" );
static const tchar* s_FileTypeDefaultText = TXT( "<All File Types>" );
static const tchar* s_CreatedByDefaultText = TXT( "<Select User>" );
static const tchar* s_AssetTypeDefaultText = TXT( "<Select Type>" );
static const tchar* s_FileIDDefaultText = TXT( "<Hex or Decimal file ID>" );
static const tchar* s_LevelDefaultText = TXT( "<Level Name>" );
static const tchar* s_ShaderDefaultText = TXT( "<Shader Name>") ;
static const tchar* s_ComponentNameDefaultText = TXT( "<Select Component Name>" );
static const tchar* s_ComponentValueDefaultText = TXT( "<Component Value>" );



///////////////////////////////////////////////////////////////////////////// 
Filter::Filter( const tstring& name, const tstring& filters )
: m_Name( name )
, m_Filters( filters )
{
    if ( !m_Filters.empty() )
    {
        Tokenize( GetFilters(), m_Extensions, TXT( ";" ) );

        std::set< tstring >::iterator it = m_Extensions.begin();
        std::set< tstring >::iterator end = m_Extensions.end();
        for ( ; it != end ; ++it )
        {
            tstring& extension = (*it);
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
, m_FieldMRU( new Nocturnal::FieldMRU() )
{
    m_GoButton->SetId( BrowserMenu::AdvancedSearchGo );
    m_CancelButton->SetId( BrowserMenu::AdvancedSearchCancel );

    m_WordsHelpBitmap->SetBitmap( Nocturnal::GlobalImageManager().GetBitmap( TXT( "apps/help-browser.png" ) ) );
    m_PhraseHelpBitmap->SetBitmap( Nocturnal::GlobalImageManager().GetBitmap( TXT( "apps/help-browser.png" ) ) );
    m_FileIDHelpBitmap->SetBitmap( Nocturnal::GlobalImageManager().GetBitmap( TXT( "apps/help-browser.png" ) ) );
    m_ComponentHelpBitmap->SetBitmap( Nocturnal::GlobalImageManager().GetBitmap( TXT( "apps/help-browser.png" ) ) );
    m_LevelHelpBitmap->SetBitmap( Nocturnal::GlobalImageManager().GetBitmap( TXT( "apps/help-browser.png" ) ) );
    m_ShaderHelpBitmap->SetBitmap( Nocturnal::GlobalImageManager().GetBitmap( TXT( "apps/help-browser.png" ) ) );

    m_DefaultFieldText.insert( std::make_pair( m_CollectionChoice->GetId(), s_CollectionDefaultText ) );
    m_DefaultFieldText.insert( std::make_pair( m_FileTypeChoice->GetId(), s_FileTypeDefaultText ) );
    m_DefaultFieldText.insert( std::make_pair( m_CreatedByComboBox->GetId(), s_CreatedByDefaultText ) );
    m_DefaultFieldText.insert( std::make_pair( m_AssetTypeChoice->GetId(), s_AssetTypeDefaultText ) );
    m_DefaultFieldText.insert( std::make_pair( m_FileIDTextCtrl->GetId(), s_FileIDDefaultText ) );
    m_DefaultFieldText.insert( std::make_pair( m_LevelTextCtrl->GetId(), s_LevelDefaultText ) );
    m_DefaultFieldText.insert( std::make_pair( m_ShaderTextCtrl->GetId(), s_ShaderDefaultText ) );
    m_DefaultFieldText.insert( std::make_pair( m_ComponentNameChoice->GetId(), s_ComponentNameDefaultText ) );
    m_DefaultFieldText.insert( std::make_pair( m_ComponentValueTextCtrl->GetId(), s_ComponentValueDefaultText ) );

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
        tstring cleanDirectory = dirDialog.GetPath().c_str();
        Nocturnal::Path::Normalize( cleanDirectory );

        m_FieldMRU->AddItem( (wxControlWithItems*) m_FolderChoice, TXT( "m_FolderChoice" ), cleanDirectory );
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
    //else if ( eventID == m_AssetTypeChoice->GetId() )
    //{
    //  if ( m_AssetTypeChoice->GetValue().empty() )
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
void BrowserSearchPanel::AddFilter( const tchar* filter )
{
    std::vector< tstring > splitFilter;
    Tokenize( filter, splitFilter, TXT( "\\|" ) );

    const size_t numTokens = splitFilter.size();
    if ( numTokens % 2 != 0 )
        return; // error

    for ( size_t i = 0; i < numTokens; i+=2 )
    {
        bool inserted = m_Filters.Append( Filter( splitFilter.at( i ), splitFilter.at( i+1 ) ) ); 
    }
}

/////////////////////////////////////////////////////////////////////////////
void BrowserSearchPanel::SetFilterIndex( const tstring& filter )
{
    size_t index = 0;

    std::vector< tstring > splitFilter;
    Tokenize( filter, splitFilter, TXT( "\\|" ) );

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
tstring BrowserSearchPanel::GetFilter()
{
    tstring filter = TXT( "" );

    // file type filter
    tstring selectedFilter = m_FileTypeChoice->GetStringSelection().c_str();
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
    m_Filters.Append( Filter( TXT( "All files (*.*)" ), TXT( "*.*" ) ) );

    m_FileTypeChoice->Clear();

    // first insert the DefaultString:
    M_DefaultFieldText::const_iterator findDefaultText = m_DefaultFieldText.find( m_FileTypeChoice->GetId() );
    if ( findDefaultText != m_DefaultFieldText.end() )
    {
        m_FileTypeChoice->AppendString( findDefaultText->second.c_str() );
        m_FileTypeChoice->AppendString( TXT( "" ) );
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
const Filter* BrowserSearchPanel::FindFilter( const tstring& name )
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
    bool operator()( const tstring& lhs, const tstring& rhs ) const
    {
        return _tcsicmp( lhs.c_str(), rhs.c_str() ) < 0;
    }
};

///////////////////////////////////////////////////////////////////////////////
void BrowserSearchPanel::PopulateForm()
{
    DisconnectListners();

    Freeze();

#pragma TODO( "reimplement without CacheDB" )
    m_FileTypeChoice->Enable( false );
    m_CreatedByComboBox->Enable( false );
    m_AssetTypeChoice->Enable( false );
    m_ComponentNameChoice->Enable( false );
    m_ComponentValueTextCtrl->Enable( false );

    // populate from MRU: m_FolderChoice
    m_FieldMRU->PopulateControl( (wxControlWithItems*) m_FolderChoice, TXT( "m_FolderChoice" ), TXT( "" ) );

    Thaw();

    ConnectListners();
}

///////////////////////////////////////////////////////////////////////////////
void BrowserSearchPanel::PopulateChoiceControl( wxControlWithItems* control, std::vector< tstring >& contents )
{
    control->Clear();

    wxArrayString items;
    items.reserve( contents.size() + 1 );

    // first insert the DefaultString:
    M_DefaultFieldText::const_iterator findDefaultText = m_DefaultFieldText.find( control->GetId() );
    if ( findDefaultText != m_DefaultFieldText.end() )
    {
        items.push_back( findDefaultText->second.c_str() );
        items.push_back( TXT( "" ) );
    }

    // then insert the list
    std::sort( contents.begin(), contents.end(), SortChoices() );
    std::vector< tstring >::const_iterator itr = contents.begin();
    std::vector< tstring >::const_iterator end = contents.end();
    for ( ; itr != end ; ++itr )
    {
        const tstring& value = (*itr);

        if ( value.empty() )
            continue;

        if ( ( _tcsicmp( value.c_str(), TXT( "null" ) ) == 0 )
            || ( _tcsicmp( value.c_str(), TXT( "unknown" ) ) == 0 ) )
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
void BrowserSearchPanel::PopulateFileTypeChoice( std::vector< tstring >& contents )
{
    // then insert the list
    std::sort( contents.begin(), contents.end(), SortChoices() );
    std::vector< tstring >::const_iterator itr = contents.begin();
    std::vector< tstring >::const_iterator end = contents.end();
    for ( ; itr != end ; ++itr )
    {
        const tstring& value = (*itr);

        if ( value.empty() )
            continue;

        if ( ( _tcsicmp( value.c_str(), TXT( "null" ) ) == 0 )
            || ( _tcsicmp( value.c_str(), TXT( "unknown" ) ) == 0 ) )
        {
            continue;
        }

        AddFilter( tstring( tstring( TXT( "*." ) ) + value ).c_str() );
    }

    UpdateFilters();
    m_FileTypeChoice->SetSelection( 0 );
}

void BrowserSearchPanel::PopulateCollectionsChoice()
{
    u32 numAdded = 0;
    std::vector< tstring > tableData;
    for ( M_AssetCollections::const_iterator itr = m_CollectionManager->GetCollections().begin(),
        end = m_CollectionManager->GetCollections().end(); itr != end; ++itr )
    {    
        AssetCollection* collection = itr->second;
        if ( collection && !collection->GetAssetPaths().empty() )
        {
            tableData.push_back( collection->GetName() );
            ++numAdded;
        }
    }

    if ( numAdded > 0 )
    {
        tableData.push_back( TXT( "" ) );
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

    tstring queryString = TXT( "" );
    AssetCollection* collection = NULL;

    wxString fieldStringValue;
    tstring cleanFieldValue;

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
                queryString += queryString.empty() ? TXT( "" ) : TXT( " " );
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
        std::vector< tstring > splitValue;
        Tokenize( fieldStringValue.c_str(), splitValue, TXT( " " ) );

        std::vector< tstring >::const_iterator itr = splitValue.begin();
        std::vector< tstring >::const_iterator end = splitValue.end();
        for ( ; itr != end ; ++itr )
        {
            const tstring& value = (*itr);
            cleanFieldValue = value;
            Nocturnal::Path::Normalize( cleanFieldValue );
            bool isPath = true; //( cleanFieldValue.find( '/' ) != tstring::npos ) ? true : false; 

            queryString += queryString.empty() ? TXT( "" ) : TXT( " " );
            queryString += isPath ? TXT( "path" ) : TXT( "name" );
            queryString += TXT( ":" );
            queryString += cleanFieldValue;
            queryString += TXT( "" );
        }
    }

    //  wxTextCtrl* m_PhraseTextCtrl;
    fieldStringValue = m_PhraseTextCtrl->GetValue();
    fieldStringValue.Trim(true);  // trim white-space right 
    fieldStringValue.Trim(false); // trim white-space left
    if ( !fieldStringValue.empty() )
    {
        cleanFieldValue = fieldStringValue.c_str();
        Nocturnal::Path::Normalize( cleanFieldValue );
        bool isPath = true; //( cleanFieldValue.find( '/' ) != tstring::npos ) ? true : false;
        bool needsQuotes = ( cleanFieldValue.find( ' ' ) != tstring::npos ) ? true : false;

        queryString += queryString.empty() ? TXT( "" ) : TXT( " " );
        queryString += isPath ? TXT( "path" ) : TXT( "name" );
        queryString += TXT( ":" );
        queryString += needsQuotes ? TXT( "\"" ) : TXT( "" );
        queryString += cleanFieldValue;
        queryString += needsQuotes ? TXT( "\"" ) : TXT( "" );
    }

    // File type filter-------------------------------
    //  wxComboBox* m_FileTypeChoice;
    fieldStringValue = m_FileTypeChoice->GetStringSelection().c_str();
    fieldStringValue.Trim(true);  // trim white-space right 
    fieldStringValue.Trim(false); // trim white-space left
    if ( !fieldStringValue.empty()
        && _tcsicmp( fieldStringValue.c_str(), s_FileTypeDefaultText  ) != 0 
        && _tcsicmp( fieldStringValue.c_str(), TXT( "All files (*.*)" ) ) != 0 )
    {
        const Filter* foundFilter = FindFilter( fieldStringValue.c_str() );
        if ( foundFilter != NULL && !foundFilter->GetExtensions().empty() )
        {
            queryString += queryString.empty() ? TXT( "" ) : TXT( " " );
            queryString += TXT( "fileType:" );
            queryString += TXT( "\"*" );
            queryString += *(foundFilter->GetExtensions().begin());
            queryString += TXT( "\"" );
        }
        else
        {
            wxMessageBox( TXT( "The file extension you specified is not valid." ), TXT( "Error" ), wxCENTER | wxOK | wxICON_ERROR, GetParent() );
            return false;
        }
    }

    //  wxChoice*   m_FolderChoice;
    fieldStringValue = m_FolderChoice->GetStringSelection();
    fieldStringValue.Trim(true);  // trim white-space right 
    fieldStringValue.Trim(false); // trim white-space left
    if ( !fieldStringValue.empty() )
    {
        cleanFieldValue = fieldStringValue.c_str();
        Nocturnal::Path::Normalize( cleanFieldValue );
#pragma TODO( "support getting the asset root from the settings for the project to strip it" )
        //    FileSystem::StripPrefix( File::GlobalManager().GetManagedAssetsRoot(), cleanFieldValue );

        if ( !cleanFieldValue.empty() )
        {
            bool needsQuotes = ( cleanFieldValue.find( ' ' ) != tstring::npos ) ? true : false;

            queryString += queryString.empty() ? TXT( "" ) : TXT( " " );
            queryString += TXT( "path:" );
            queryString += needsQuotes ? TXT( "\"" ) : TXT( "" );
            queryString += cleanFieldValue;
            queryString += TXT( "*\"" );
        }
    }

    // -----------------------------------------
    // Additional search criteria
    //  wxComboBox* m_CreatedByComboBox; 
    fieldStringValue = m_CreatedByComboBox->GetStringSelection();
    fieldStringValue.Trim(true);  // trim white-space right 
    fieldStringValue.Trim(false); // trim white-space left
    if ( !fieldStringValue.empty()
        && _tcsicmp( fieldStringValue.c_str(), s_CreatedByDefaultText  ) != 0 )
    {
        queryString += queryString.empty() ? TXT( "" ) : TXT( " " );
        queryString += TXT( "rcsUser:" );
        queryString += fieldStringValue.c_str();
        queryString += TXT( "" );
    }


    // -----------------------------------------
    //  wxChoice*   m_AssetTypeChoice;
    fieldStringValue = m_AssetTypeChoice->GetStringSelection();
    fieldStringValue.Trim(true);  // trim white-space right 
    fieldStringValue.Trim(false); // trim white-space left
    if ( !fieldStringValue.empty()
        && _tcsicmp( fieldStringValue.c_str(), s_AssetTypeDefaultText  ) != 0 )
    {
        bool needsQuotes = ( fieldStringValue.find( ' ' ) != -1 ) ? true : false;

        queryString += queryString.empty() ? TXT( "" ) : TXT( " " );
        queryString += TXT( "assetType:" );
        queryString += needsQuotes ? TXT( "\"" ) : TXT( "" );
        queryString += fieldStringValue.c_str();
        queryString += needsQuotes ? TXT( "\"" ) : TXT( "" );
    }

    // -----------------------------------------
    //  wxTextCtrl* m_FileIDTextCtrl;
    fieldStringValue = m_FileIDTextCtrl->GetValue();
    fieldStringValue.Trim(true);  // trim white-space right 
    fieldStringValue.Trim(false); // trim white-space left
    if ( !fieldStringValue.empty()
        && _tcsicmp( fieldStringValue.c_str(), s_FileIDDefaultText  ) != 0 )
    {
        queryString += queryString.empty() ? TXT( "" ) : TXT( " " );
        queryString += TXT( "pathHash:" );
        queryString += fieldStringValue.c_str();
        queryString += TXT( "" );
    }

    // -----------------------------------------
    //  wxChoice*   m_ComponentNameChoice;
    //  wxTextCtrl* m_ComponentValueTextCtrl
    fieldStringValue = m_ComponentNameChoice->GetStringSelection();
    fieldStringValue.Trim(true);  // trim white-space right 
    fieldStringValue.Trim(false); // trim white-space left
    if ( !fieldStringValue.empty()
        && _tcsicmp( fieldStringValue.c_str(), s_ComponentNameDefaultText  ) != 0 )
    {
        cleanFieldValue = fieldStringValue.c_str();

        fieldStringValue = m_ComponentValueTextCtrl->GetValue();
        fieldStringValue.Trim(true);  // trim white-space right 
        fieldStringValue.Trim(false); // trim white-space left
        if ( !fieldStringValue.empty()
            && _tcsicmp( fieldStringValue.c_str(), s_ComponentValueDefaultText  ) != 0 )
        {
            bool needsQuotes = ( fieldStringValue.find( ' ' ) != -1 ) ? true : false;

            queryString += queryString.empty() ? TXT( "" ) : TXT( " " );
            queryString += cleanFieldValue + TXT( ":" );
            queryString += needsQuotes ? TXT( "\"" ) : TXT( "" );
            queryString += fieldStringValue.c_str();
            queryString += needsQuotes ? TXT( "\"" ) : TXT( "" );
        }
        else
        {
            wxMessageBox( TXT( "Please specify an attribute value to search for." ), TXT( "Error" ), wxCENTER | wxOK | wxICON_ERROR, GetParent() );
            return false;
        }
    }

    // -----------------------------------------
    //  wxTextCtrl* m_LevelTextCtrl;
    fieldStringValue = m_LevelTextCtrl->GetValue();
    fieldStringValue.Trim(true);  // trim white-space right 
    fieldStringValue.Trim(false); // trim white-space left
    if ( !fieldStringValue.empty()
        && _tcsicmp( fieldStringValue.c_str(), s_LevelDefaultText  ) != 0 )
    {
        cleanFieldValue = fieldStringValue.c_str();
        Nocturnal::Path::Normalize( cleanFieldValue );
        bool needsQuotes = ( cleanFieldValue.find( ' ' ) != tstring::npos ) ? true : false;

        queryString += queryString.empty() ? TXT( "" ) : TXT( " " );
        queryString += TXT( "level:" );
        queryString += needsQuotes ? TXT( "\"" ) : TXT( "" );
        queryString += cleanFieldValue;
        queryString += needsQuotes ? TXT( "\"" ) : TXT( "" );
    }

    // -----------------------------------------
    //  wxTextCtrl* m_ShaderTextCtrl;
    fieldStringValue = m_ShaderTextCtrl->GetValue();
    fieldStringValue.Trim(true);  // trim white-space right 
    fieldStringValue.Trim(false); // trim white-space left
    if ( !fieldStringValue.empty()
        && _tcsicmp( fieldStringValue.c_str(), s_ShaderDefaultText  ) != 0 )
    {
        cleanFieldValue = fieldStringValue.c_str();
        Nocturnal::Path::Normalize( cleanFieldValue );
        bool needsQuotes = ( cleanFieldValue.find( ' ' ) != tstring::npos ) ? true : false;

        queryString += queryString.empty() ? TXT( "" ) : TXT( " " );
        queryString += TXT( "shader:" );
        queryString += needsQuotes ? TXT( "\"" ) : TXT( "" );
        queryString += cleanFieldValue;
        queryString += needsQuotes ? TXT( "\"" ) : TXT( "" );
    }

    m_BrowserFrame->Search( queryString, collection );

    return true;
}
