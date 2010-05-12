#include "stdafx.h"

#include "FileBrowser.h"
#include "FileBrowserInit.h"
#include "Inspect/Container.h"

#include "AppUtils/AppUtils.h"
#include "Asset/AssetInit.h"
#include "Asset/AssetClass.h"
#include "Asset/Exceptions.h"

#include "Reflect/Class.h"
#include "Reflect/Version.h"

#include "File/History.h"
#include "File/Manager.h"
#include "File/PatchOperations.h"
#include "FileUI/ManagedFileDialog.h"

#include "Finder/Finder.h"
#include "Finder/AssetSpecs.h"
#include "Finder/ExtensionSpecs.h"
#include "Finder/FinderSpec.h"
#include "Finder/LunaSpecs.h"
#include "Finder/ProjectSpecs.h"

//#include "File/Manager.h"
//#include "File/ManagedFile.h"
#include "TUID/TUID.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/FileIterator.h"

#include "Common/Types.h"

#include "Console/Console.h"

#include "Common/String/Tokenize.h"
#include "Common/Container/Insert.h" 
using Nocturnal::Insert; 

#include <Windows.h>
#include "Windows/Thread.h"

#include "wx/artprov.h"
#include "wx/generic/dirctrlg.h" // for wxTheFileIconsTable
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/clipbrd.h>

#include "UIToolKit/AutoCompleteComboBox.h"
#include "UIToolKit/FieldMRU.h"
#include "UIToolKit/ImageManager.h"
#include "UIToolKit/SortableListView.h"

using namespace UIToolKit;

/////////////////////////////////////////////////////////////////////////////
// Declare custom event types:
//
extern const wxEventType wxEVT_BEGIN_SEARCH;
const wxEventType wxEVT_BEGIN_SEARCH = wxNewEventType();
#define EVT_BEGIN_SEARCH(id, func) \
  DECLARE_EVENT_TABLE_ENTRY( \
  wxEVT_BEGIN_SEARCH, id, wxID_ANY, \
  (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &func ), \
  (wxObject *) NULL \
  ),

extern const wxEventType wxEVT_END_SEARCH;
const wxEventType wxEVT_END_SEARCH = wxNewEventType();
#define EVT_END_SEARCH(id, func) \
  DECLARE_EVENT_TABLE_ENTRY( \
  wxEVT_END_SEARCH, id, wxID_ANY, \
  (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, &func ), \
  (wxObject *) NULL \
  ),


/////////////////////////////////////////////////////////////////////////////


namespace File
{

  UIToolKit::FieldMRUPtr g_FieldMRU = NULL;
  V_FileInfoPtr g_FileInfos;

  // used bu the ThrobberTimer
  static const wxString s_DotDotDot[] = { wxT(""), wxT("."), wxT(".."), wxT("...") };

  /////////////////////////////////////////////////////////////////////////////
  // Events table
  //
  BEGIN_EVENT_TABLE( FileBrowser, wxWindow )
    EVT_BUTTON( wxID_OK, FileBrowser::OnOK )
    EVT_BUTTON( wxID_CANCEL, FileBrowser::OnCancel )
    EVT_CLOSE( FileBrowser::OnClose )

    EVT_BUTTON( ID_SearchButton, FileBrowser::OnSearch )
    EVT_BUTTON( ID_ClearButton, FileBrowser::OnClear )
    EVT_BEGIN_SEARCH( wxID_ANY, FileBrowser::OnBeginSearch )
    EVT_END_SEARCH( wxID_ANY, FileBrowser::OnEndSearch )

    EVT_TEXT_ENTER( ID_SearchQuery, FileBrowser::OnSearch ) 
    EVT_TEXT_ENTER( ID_LookInPath, FileBrowser::OnSearch )
    EVT_TEXT_ENTER( ID_AssetTUID, FileBrowser::OnSearch ) 
    EVT_KEY_UP( FileBrowser::OnChar )

    EVT_CHOICE( ID_FileType, FileBrowser::OnFileType )

    EVT_BUTTON( ID_BrowseButton, FileBrowser::OnBrowseButton )

    EVT_SIZE( FileBrowser::OnSize )

    EVT_LIST_END_LABEL_EDIT( ID_ListCtrlResults, FileBrowser::OnItemEndEdit ) 	    // Finish editing a label. This can be prevented by calling Veto().
    
    EVT_LIST_ITEM_ACTIVATED( ID_ListCtrlResults, FileBrowser::OnItemActivated )     // The item has been double-clicked.
    EVT_LIST_ITEM_SELECTED( ID_ListCtrlResults, FileBrowser::OnItemSelection ) 	    // The item has been selected.
    EVT_LIST_ITEM_DESELECTED( ID_ListCtrlResults, FileBrowser::OnItemDeselection )  // The item has been deselected.
    EVT_LIST_ITEM_FOCUSED( ID_ListCtrlResults, FileBrowser::OnItemSelection ) 	    // The currently focused item has changed.
    EVT_LIST_COL_CLICK( ID_ListCtrlResults, FileBrowser::OnColClick ) 	            // A column (m_col) has been left-clicked.

  END_EVENT_TABLE()



  /////////////////////////////////////////////////////////////////////////////  

  namespace SearchModes
  {
    enum SearchMode
    {
      SearchQuery       = 1 << 1,
      AssetID           = 1 << 2,
    };
  }
  typedef SearchModes::SearchMode SearchMode;

  /////////////////////////////////////////////
  // Search criteria struct
  class SearchCriteria
  {
  public:
    std::string                   m_SearchQuery;
    bool                          m_SearchHistoryData;
    S_string                      m_Filter;
    i32                           m_SearchMode;
    std::string                   m_LookIn;
    V_string                      m_SearchQueries;
    tuid                          m_TUID;

    SearchCriteria() { clear(); }

    void clear()
    {
      m_SearchQuery             = "";
      m_SearchHistoryData       = false;
      m_Filter.clear();
      m_Filter.insert( "*.*" );
      m_LookIn                  = Finder::ProjectAssets();
      m_SearchQueries.clear();
      m_SearchMode               = 0;
      m_TUID                    = TUID::Null;
    }
  };


  //////////////////////////////////////// 
  // Used to update the UI at a regular interval
  //
  class UpdateTimer : public wxTimer
  {
  public:
    FileBrowser* m_FileBrowser;

    UpdateTimer( FileBrowser* fileBrowser ) : m_FileBrowser( fileBrowser ) {}
    void Notify()
    {
      m_FileBrowser->UpdateTimerTic();
    }
  };


  /////////////////////////////////////////
  // Used to update the UI at a regular interval
  //
  class ThrobberTimer : public wxTimer
  {
  public:
    FileBrowser*    m_FileBrowser;

    ThrobberTimer( FileBrowser* fileBrowser ) : m_FileBrowser( fileBrowser ){}

    void Notify()
    {
      m_FileBrowser->ThrobberTimerTic();
    }
  };


  /////////////////////////////////////////
  // Used to update the UI at a regular interval
  //
  class ShowDetailsTimer : public wxTimer
  {
  public:
    FileBrowser*    m_FileBrowser;

    ShowDetailsTimer( FileBrowser* fileBrowser ) : m_FileBrowser( fileBrowser ){}

    void Notify()
    {
      m_FileBrowser->ShowDetailsTimerTic();
    }
  };


  /////////////////////////////////////////////////////////////////////////////
  // 
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


  /////////////////////////////////////////////////////////////////////////////
  // Ctor
  //
  FileBrowser::FileBrowser
    ( 
    wxWindow* parent,
    int id,
    wxString title,
    wxPoint pos,
    wxSize size, 
    int style
    )
    : FileBrowserGenerated( parent, id, title, pos, size, style )
  {
    Init();

    Create( parent, id, title, pos, size, style );
  }



  /////////////////////////////////////////////////////////////////////////////
  // Initialize the dialog
  //
  void FileBrowser::Init()
  {
    ::FileBrowser::Initialize();

    if ( !g_FieldMRU )
    {
      g_FieldMRU = new UIToolKit::FieldMRU(); 
    }

    if ( m_DisplayColumns.empty() )
    {
      m_DisplayColumns[DisplayColumnTypes::Name]       = DisplayColumn( DisplayColumnTypes::Name,       "Name",           200, FileInfo::GetName );
      m_DisplayColumns[DisplayColumnTypes::FileType]   = DisplayColumn( DisplayColumnTypes::FileType,   "File Type",      100, FileInfo::GetFileType );
      m_DisplayColumns[DisplayColumnTypes::Folder]     = DisplayColumn( DisplayColumnTypes::Folder,     "In Folder",      250, FileInfo::GetFolder );
      m_DisplayColumns[DisplayColumnTypes::CreatedBy]  = DisplayColumn( DisplayColumnTypes::CreatedBy,  "Created By",     75,  FileInfo::GetCreatedBy );
      m_DisplayColumns[DisplayColumnTypes::Size]       = DisplayColumn( DisplayColumnTypes::Size,       "Size",           50,  FileInfo::GetSize );
      m_DisplayColumns[DisplayColumnTypes::Id]         = DisplayColumn( DisplayColumnTypes::Id,         "File ID",        200, FileInfo::GetId );
    }

    m_DisplayColumnList.clear();
    m_DisplayColumnList.push_back( DisplayColumnTypes::Name );
    m_DisplayColumnList.push_back( DisplayColumnTypes::FileType );
    m_DisplayColumnList.push_back( DisplayColumnTypes::Folder );
    m_DisplayColumnList.push_back( DisplayColumnTypes::Size );
    m_DisplayColumnList.push_back( DisplayColumnTypes::Id );


    m_DisplayDetailsList.clear();
    m_DisplayDetailsList.push_back( DisplayColumnTypes::Name );
    m_DisplayDetailsList.push_back( DisplayColumnTypes::Folder );
    m_DisplayDetailsList.push_back( DisplayColumnTypes::FileType );
    m_DisplayDetailsList.push_back( DisplayColumnTypes::Size );
    m_DisplayDetailsList.push_back( DisplayColumnTypes::CreatedBy );
    m_DisplayDetailsList.push_back( DisplayColumnTypes::Id );

    // For displaying images in this dialog.
    UIToolKit::ImageManagerInit( FinderSpecs::Luna::DEFAULT_THEME_FOLDER.GetFolder(),
      FinderSpecs::Luna::GAME_THEME_FOLDER.GetFolder() );
    
    m_ReturnCode            = wxID_CANCEL;
    m_IsTuidRequired        = false;

    m_IsLookInUserDefined   = true;

    // displaying results
    m_CurFileInfoIndex      = 0;
    m_FileInfoIndexTable.clear();
    m_FileInfos.clear();
    
    // searching
    m_SearchCriteria        = new SearchCriteria();

    m_ContinueSearching     = false;

    m_UpdateTimer           = NULL;
    m_ThrobberTimer         = NULL;
    m_DotIndex              = 0;

    m_LookingIn             = "";
    m_FoundFiles.clear();

    m_RequestedFileID       = TUID::Null;

    ::InitializeCriticalSection( &m_FoundFilesCriticalSection );
    ::InitializeCriticalSection( &m_LookingInCriticalSection );

    m_EventEndSearch        = ::CreateEvent( NULL, TRUE, TRUE,  "FileBrowserEndSearchEvent" );

    m_ShowDetails           = false;
    m_EventEndShowDetails   = ::CreateEvent( NULL, TRUE, TRUE,  "FileBrowserEndShowDetailsEvent" );
    ::InitializeCriticalSection( &m_ShowDetailsCriticalSection );
    m_ShowDetailsTimer      = NULL;
    

    UpdateFilter();
  }


  /////////////////////////////////////////////////////////////////////////////
  // Called by the Ctor to build the dialog
  //
  void FileBrowser::Create
    ( 
    wxWindow* parent,
    int id, 
    wxString title, 
    wxPoint pos, 
    wxSize size, 
    int style
    )
  {
    // Set the task bar icon -- Not used currently
    wxIcon appIcon;
    appIcon.CopyFromBitmap( UIToolKit::GlobalImageManager().GetBitmap( "magnify_16.png", wxBITMAP_TYPE_PNG ) );
    this->SetIcon( appIcon );

    m_staticTextSearching->SetLabel( "" );

    m_buttonSearch->SetDefault();

    // Prepopulate the combo boxes MRU:
    g_FieldMRU->PopulateControl( (wxControlWithItems*) m_comboBoxSearch, "m_comboBoxSearch" );
    g_FieldMRU->PopulateControl( (wxControlWithItems*) m_comboBoxLookIn, "m_comboBoxLookIn", Finder::ProjectAssets() );
    g_FieldMRU->PopulateControl( (wxControlWithItems*) m_comboBoxTUID, "m_comboBoxTUID" );

    wxImageList *imageList = wxTheFileIconsTable->GetSmallImageList();
    m_listCtrlResults->SetImageList( imageList, wxIMAGE_LIST_SMALL );

    Layout();
    SetFocus();

    m_comboBoxSearch->SetFocus();
    m_bitmapThumbnail->Hide();
  }


  /////////////////////////////////////////////////////////////////////////////
  // Dtor, deletes the wxWidget objects
  // 
  FileBrowser::~FileBrowser()
  {    
    // wait for searching thread to complete
    ::WaitForSingleObject( m_EventEndSearch, INFINITE );
    ::CloseHandle( m_EventEndSearch );

    ::WaitForSingleObject( m_EventEndShowDetails, INFINITE );
    ::CloseHandle( m_EventEndShowDetails );

    ::DeleteCriticalSection( &m_ShowDetailsCriticalSection );
    delete m_ShowDetailsTimer;
    m_ShowDetailsTimer = NULL;

    UIToolKit::ImageManagerCleanup();

    m_DisplayColumns.clear();

    delete m_ThrobberTimer;
    m_ThrobberTimer = NULL;

    delete m_UpdateTimer;
    m_UpdateTimer = NULL;

    delete m_SearchCriteria;
    m_SearchCriteria = NULL;

    ::DeleteCriticalSection( &m_FoundFilesCriticalSection );
    ::DeleteCriticalSection( &m_LookingInCriticalSection );

    ::FileBrowser::Cleanup();
  }




  //*************************************************************************//
  //
  //  wxDialog OVERLOADS
  //
  //*************************************************************************//



  /////////////////////////////////////////////////////////////////////////////
  // show the dialog modally and return the value passed to EndModal()
  //
  int FileBrowser::ShowModal()
  {
    int result = wxID_CANCEL;
    bool isDone = false;
    ClearPaths();
    m_FileIDs.clear();

    // clear the results list and load the last found files
    InitResultsDisplay();
    RepopulateResults();

    while ( !isDone && ( ( result = __super::ShowModal() ) == wxID_OK ) )
    {
      m_FileIDs.clear();
      isDone = true;

      std::string error;

      for each ( const std::string& path in m_Paths )
      {
        if ( IsTuidRequired() )
        {
          tuid fileID = TUID::Null;
          if ( ManagedFileDialog::ValidateFileID( path, m_RequestedFileID, fileID, error ) )
          {
            m_FileIDs.insert( fileID );
          }
          else
          {
            // Error
            break;
          }
        }

        if ( !ManagedFileDialog::ValidateFinderSpec( path, error ) )
        {
          // Error
          break;
        }

        if ( !ValidatePath( path, error ) )
        {
          // Error
          break;
        }
      }

      if ( !error.empty() )
      {
        isDone = false;
        wxMessageBox( error.c_str(), "Error", wxCENTER | wxOK | wxICON_ERROR, GetParent() );
      }
    }

    return result;
  }


  /////////////////////////////////////////////////////////////////////////////
  // may be called to terminate the dialog with the given return code
  //
  void FileBrowser::EndModal( int retCode )
  {
    // wait for all threads to complete

    m_ContinueSearching = false;
    ::WaitForSingleObject( m_EventEndSearch, INFINITE );

    m_ShowDetails = false;
    ::WaitForSingleObject( m_EventEndShowDetails, INFINITE );

    m_ReturnCode = retCode;

    __super::EndModal( m_ReturnCode );
  }





  //*************************************************************************//
  //
  //  API FUNCTIONS
  //
  //*************************************************************************//



  /////////////////////////////////////////////////////////////////////////////
  // gets the currently selected file path (for single selection mode)
  //
  std::string FileBrowser::GetPath() const
  {
    // You should only call this function on a control that allows single
    // selection only (otherwise use GetPaths).
    NOC_ASSERT( !IsMultipleSelectionEnabled() );

    static const std::string empty;
    if ( !m_Paths.empty() )
    {
      std::string path = *m_Paths.begin();
      return path;
    }

    return empty;
  }

  /////////////////////////////////////////////////////////////////////////////
  // gets all the currently selected file paths (for multiple selection)
  //
  const S_string& FileBrowser::GetPaths() const
  {
    // You should only call this function if multiple selection is enabled
    // (otherwise use GetPath).
    NOC_ASSERT( IsMultipleSelectionEnabled() );

    return m_Paths;
  }


  /////////////////////////////////////////////////////////////////////////////
  // returns the currently selected filter eg: "*.*", "*.png;*.jpg", etc...
  //
  std::string FileBrowser::GetFilter()
  {
    std::string filter = "";

    // file type filter
    std::string selectedFilter = m_choiceFiletype->GetStringSelection().c_str();
    const Filter* foundFilter = FindFilter( selectedFilter );
    if ( foundFilter )
    {
      filter = foundFilter->GetFilters();
    }

    return filter;
  }


  /////////////////////////////////////////////////////////////////////////////
  // Sets the "Look in" directory
  //
  void FileBrowser::SetDirectory( const std::string& directory, bool userSpecified )
  {
    m_IsLookInUserDefined = userSpecified;

    if ( directory.empty() )
    {
      g_FieldMRU->AddItem( (wxControlWithItems*) m_comboBoxLookIn, "m_comboBoxLookIn", Finder::ProjectAssets() );
    }
    else
    {
      std::string cleanDirectory = directory;
      if ( FileSystem::HasExtension( cleanDirectory ) )
      {
        FileSystem::StripLeaf( cleanDirectory );
      }

      g_FieldMRU->AddItem( (wxControlWithItems*) m_comboBoxLookIn, "m_comboBoxLookIn", cleanDirectory );
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  // Clears the current list of filters and adds a FinderSpec filter to the
  // filters list.
  //
  void FileBrowser::SetFilter( const Finder::FinderSpec& filterSpec )
  {
    m_Filters.Clear();
    AddFilter( filterSpec.GetDialogFilter().c_str() );
  }


  /////////////////////////////////////////////////////////////////////////////
  // Clears the current list of filters and adds a filter passed in with the
  // following format:
  //   "<Display String>|<Extension Mask>"
  //
  // Examples:
  //   "All files (*.*)|*.*"
  //   "Maya files (*.*)|*.mb"
  //   "All files (*.*)|*.*|Maya files (*.*)|*.mb"
  //
  void FileBrowser::SetFilter( const char* filter )
  {
    m_Filters.Clear();
    AddFilter( filter );
  }


  /////////////////////////////////////////////////////////////////////////////
  // Adds a FinderSpec filter to the current list of filters.
  //
  void FileBrowser::AddFilter( const Finder::FinderSpec& filterSpec )
  {
    AddFilter( filterSpec.GetDialogFilter().c_str() );
    UpdateFilter();
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
  void FileBrowser::AddFilter( const char* filter )
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

    UpdateFilter();
  }


  /////////////////////////////////////////////////////////////////////////////
  // Updates the current display list of filters and adds "All files (*.*)|*.*" filter
  // if the current list is empty.
  //
  void FileBrowser::UpdateFilter()
  {
    if ( m_Filters.Empty() ) //|| m_Style & FileDialogStyles::ShowAllFilesFilter )
    {
      bool inserted = m_Filters.Append( Filter( "All files (*.*)", "*.*" ) );
    }

    m_choiceFiletype->Clear();

    OS_Filter::Iterator it     = m_Filters.Begin();
    OS_Filter::Iterator itEnd  = m_Filters.End();
    for ( ; it != itEnd ; ++it )
    {
      m_choiceFiletype->Append( ( *it ).GetName().c_str() );
    }

    m_choiceFiletype->SetSelection( 0 );
  }



  /////////////////////////////////////////////////////////////////////////////
  void FileBrowser::SetFilterIndex( const Finder::FinderSpec& spec )
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
  void FileBrowser::SetFilterIndex( int filterIndex )
  {
    m_choiceFiletype->SetSelection( filterIndex );
  }



  /////////////////////////////////////////////////////////////////////////////
  bool FileBrowser::IsTuidRequired() const
  {
    return m_IsTuidRequired;
  }

  
  /////////////////////////////////////////////////////////////////////////////
  void FileBrowser::SetTuidRequired( bool isRequired )
  {
    m_IsTuidRequired = isRequired;
  }

  /////////////////////////////////////////////////////////////////////////////
  // This option only has meaning for single file select dialogs that are required
  // to return a file with a TUID.  If the file that is selected does not have
  // a TUID, this is the TUID that should be used (it will be checked to make
  // sure it does not already belong to something).
  // 
  void FileBrowser::SetRequestedFileID( const tuid& request )
  {
    // Only call this function if you required the dialog to use TUIDs.
    NOC_ASSERT( IsTuidRequired() );

    // Only call this function when working with a dialog in single-select mode
    NOC_ASSERT( !IsMultipleSelectionEnabled() );

    m_RequestedFileID = request;
  }

  /////////////////////////////////////////////////////////////////////////////
  void FileBrowser::EnableMultipleSelection( bool enable )
  {
    m_listCtrlResults->SetSingleStyle( wxLC_SINGLE_SEL, !enable );
  }

  /////////////////////////////////////////////////////////////////////////////
  bool FileBrowser::IsMultipleSelectionEnabled() const
  {
    return ( m_listCtrlResults->GetWindowStyle() & wxLC_SINGLE_SEL ) != wxLC_SINGLE_SEL;
  }
  
  /////////////////////////////////////////////////////////////////////////////
  // Returns the TUID for the selected file.  This function should only be called
  // for dialogs in single-select mode.  The return value is only valid if 
  // ShowModal returned wxID_OK.
  // 
  tuid FileBrowser::GetFileID() const
  {
    // Only call this function if you required the dialog to use TUIDs.
    NOC_ASSERT( IsTuidRequired() );

    // You should only call this while in single select mode.
    NOC_ASSERT( !IsMultipleSelectionEnabled() );

    tuid result = TUID::Null;
    if ( m_FileIDs.size() > 0 )
    {
      result = *m_FileIDs.begin();
    }
    return result;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Returns the TUIDs for the selected files.  This function should only be called
  // for dialogs in multi-select mode.  The return value is only valid if 
  // ShowModal returned wxID_OK.
  // 
  const S_tuid& FileBrowser::GetFileIDs() const
  {
    // Only call this function if you required the dialog to use TUIDs.
    NOC_ASSERT( IsTuidRequired() );

    // You should only call this while in multi-select mode.
    NOC_ASSERT( IsMultipleSelectionEnabled() );

    return m_FileIDs;
  }








  //*************************************************************************//
  //
  //  EVENT FUNCTIONS
  //
  //*************************************************************************//




  ///////////////////////////////////////////////////////////////////////////////
  // Called when the window is resized.  Resizes the browser panel to fill the 
  // available space.
  // 
  void FileBrowser::OnSize( wxSizeEvent& evt )
  {
    this->GetSizer()->SetDimension( 0, 0, GetClientSize().x , GetClientSize().y );    
    this->GetSizer()->Layout();
    evt.Skip();
  }

  /////////////////////////////////////////////////////////////////////////////
  // Caleld when the "OK" button is clicked. Does some error checking and the 
  // ends the modal and returns the wxID_OK
  //
  void FileBrowser::OnOK( wxCommandEvent& evt )
  {
    EndModal( evt.GetId() );
  }


  /////////////////////////////////////////////////////////////////////////////
  // Called when the "Cancel" button is clicked. Ends the modal and
  // returns the default value of wxID_CANCEL
  //
  void FileBrowser::OnCancel( wxCommandEvent& evt )
  {
    EndModal( evt.GetId() );
  }


  /////////////////////////////////////////////////////////////////////////////
  // Called when the |X| window close button is clicked. Ends the modal and
  // returns the default value of wxID_CANCEL
  //
  void FileBrowser::OnClose( wxCloseEvent& evt )
  {
    EndModal();
  }


  /////////////////////////////////////////////////////////////////////////////
  // Called when the "Search" button is clicked - collects the search criteria
  // and creates and starts the searching thread;
  // OR
  // when the "Stop" button is clicked - Stops the search thread
  // currently in process by setting the m_ContinueSearching to false.
  //
  void FileBrowser::StartSearch()
  {
    wxBusyCursor bc;

    if ( m_ContinueSearching )
    {
      m_ContinueSearching = false;

      m_buttonSearch->Enable( false );
      m_buttonClear->Enable( false );

      SetStatusText( "Stopping" );
    }
    else
    {
      m_ContinueSearching = true;
      m_buttonSearch->Enable( false );
      m_buttonClear->Enable( false );

      BeginShowDetails( NULL );

      SetStatusText( "Initializing search criteria" );

      // clear the results list and recreate the result display
      m_FileInfos.clear();
      g_FileInfos.clear();
      InitResultsDisplay();

      // start the UI throbbers
      if ( !m_ThrobberTimer )
      {
        m_ThrobberTimer = new ThrobberTimer( this );
      }
      m_ThrobberTimer->Start( 100 );

      if ( !CollectSearchCriteria() )
      {
        SetStatusText( "Please correct the errors above." );
        delete m_ThrobberTimer;
        m_ThrobberTimer = NULL;

        m_ContinueSearching = false;
        m_buttonSearch->Enable( true );
        m_buttonClear->Enable( true );
        return;
      }

      // create and start the searching thread
      class SearchThread : public wxThread
      {
      private:
        FileBrowser* m_FileBrowser;

      public:
        SearchThread( FileBrowser* fileBrowser )
          : wxThread( wxTHREAD_DETACHED )
          , m_FileBrowser( fileBrowser )
        {}

        virtual wxThread::ExitCode Entry() NOC_OVERRIDE
        {
          m_FileBrowser->Search();
          return NULL;
        }
      };   

      // Detached threads delete themselves once they have completed,
      // and thus must be created on the heap
      SearchThread* searchThread = new SearchThread( this );
      searchThread->Create();
      searchThread->Run();
    }
  }


  ///////////////////////////////////////////////////////////////////////////////
  // Called by the wxEVT_BEGIN_SEARCH event.
  // Changes the display to the searching mode, disabling some UI and swapping out
  // some visuals.
  // 
  void FileBrowser::OnBeginSearch( wxCommandEvent& WXUNUSED(evt) )
  {    
    // start the UI throbbers
    if ( !m_ThrobberTimer )
    {
      m_ThrobberTimer = new ThrobberTimer( this );
    }
    m_ThrobberTimer->Start( 100 );

    // swap the search and stop button, and disable the clear button
    m_buttonSearch->SetLabel( "Stop" );
    m_buttonSearch->Enable( true );
    m_buttonSearch->SetFocus();
    m_buttonClear->Enable( false );

    Layout();
    Refresh();

    // change the text
    SetStatusText( "Searching" );

    // start the UI throbbers
    if ( !m_UpdateTimer )
    {
      m_UpdateTimer = new UpdateTimer( this );
    }
    m_UpdateTimer->Start( 50 );
  }


  ///////////////////////////////////////////////////////////////////////////////
  // Called by the wxEVT_END_SEARCH event.
  // Hide the searching UI, stops the UI update timers and displays the rest of
  // the search results.
  // 
  void FileBrowser::OnEndSearch( wxCommandEvent& WXUNUSED(evt) )
  {
    SetStatusText( "Stopping" );
    
    m_buttonSearch->Enable( false );

    ::WaitForSingleObject( m_EventEndSearch, INFINITE );

    // if there are any files there, add them to the view now
    ::EnterCriticalSection( &m_FoundFilesCriticalSection );
    
    if ( !m_FoundFiles.empty() )
      PopulateResults( m_FoundFiles );

    ::LeaveCriticalSection( &m_FoundFilesCriticalSection );

    int numFilesFound = m_listCtrlResults->GetItemCount();
    if ( numFilesFound > 0 )
    {
      std::stringstream foundFiles;
      foundFiles << numFilesFound << ( numFilesFound == 1 ? " file found." : " files found" );
      SetStatusText( foundFiles.str() ); 
    }
    else
    {
      SetStatusText( "No files found." );
    }

    // swap the search and stop button, and enable the clear button
    m_buttonSearch->SetLabel( "Search" );
    m_buttonSearch->Enable( true );
    m_buttonClear->Enable( true );

    Layout();
    Refresh();

    // stop and delete the UI update timers
    delete m_UpdateTimer;
    m_UpdateTimer = NULL;

    delete m_ThrobberTimer;
    m_ThrobberTimer = NULL;
  }


  ///////////////////////////////////////////////////////////////////////////////
  // Update the UI during the search, adding newly found files to m_listCtrlResults,
  // and updating the m_staticTextLookingIn value.
  // 
  void FileBrowser::UpdateTimerTic()
  {
    // only updated the Looking In" text if we are still searching
    if ( !m_ContinueSearching )
      return;

    ::EnterCriticalSection( &m_LookingInCriticalSection );
    m_staticTextLookingIn->SetLabel( m_LookingIn.c_str() );
    ::LeaveCriticalSection( &m_LookingInCriticalSection );

    // if there are any files there, add them to the view now
    ::EnterCriticalSection( &m_FoundFilesCriticalSection );
    
    if ( !m_FoundFiles.empty() )
    {
      PopulateResults( m_FoundFiles );
    }

    ::LeaveCriticalSection( &m_FoundFilesCriticalSection );
  }


  ///////////////////////////////////////////////////////////////////////////////
  // 
  void FileBrowser::PopulateResults( V_FileInfoPtr& foundFiles )
  {
    wxBusyCursor bc;
    
    m_buttonSearch->Enable( false );

    m_listCtrlResults->Freeze();
    m_listCtrlResults->EnableSorting( false );
    
    for each ( const FileInfoPtr& file in foundFiles )
    {
      AddResult( file );
    }
    foundFiles.clear();

    m_listCtrlResults->EnableSorting( true );
    m_listCtrlResults->Thaw();

    m_buttonSearch->Enable( true );
  }


  ///////////////////////////////////////////////////////////////////////////////
  // 
  void FileBrowser::RepopulateResults()
  {
    if ( !g_FileInfos.empty() )
    {
      V_FileInfoPtr deepCopy = g_FileInfos;      
      g_FileInfos.clear();
      PopulateResults( deepCopy );
    }
  }

  ///////////////////////////////////////////////////////////////////////////////
  // Updates the throbbers with each call to ThrobberTimerTic
  // 
  void FileBrowser::ThrobberTimerTic()
  {
    if ( m_DotIndex > 3 )
      m_DotIndex = 0;
    
    wxString buf;
    buf.Printf( wxT( "%s%s" ), m_SearchingText.c_str(), s_DotDotDot[m_DotIndex++].c_str() );
    m_staticTextSearching->SetLabel( buf );
  }


  void FileBrowser::SetStatusText( const std::string& searchingText )
  {
    m_SearchingText = searchingText;
    m_staticTextSearching->SetLabel( m_SearchingText.c_str() );
    
    m_LookingIn = "";
    m_staticTextLookingIn->SetLabel( m_LookingIn.c_str() );
  }


  /////////////////////////////////////////////////////////////////////////////
  // Called when the "Browse..." button is clicked. Clears all of the form's
  // fields.
  //
  void FileBrowser::OnClear( wxCommandEvent& evt )
  {
    wxBusyCursor bc;
    
    m_checkBoxSeachHistoryData->SetValue( false );
    //m_checkBoxEntityTracker->SetValue( false );

    m_comboBoxSearch->SetValue( wxEmptyString );
    m_comboBoxTUID->SetValue( wxEmptyString );

    if ( m_IsLookInUserDefined )
    {
      g_FieldMRU->AddItem( (wxControlWithItems*) m_comboBoxLookIn, "m_comboBoxLookIn", Finder::ProjectAssets() );
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  // Called when the "Browse..." button is clicked. Opens the Directory browser
  // modal dialog and sets the value of the "Look In" folder.
  //
  void FileBrowser::OnBrowseButton( wxCommandEvent& evt )
  {
    // pass some initial dir to wxDirDialog
    wxString currentLookIn = m_comboBoxLookIn->GetValue();

    wxDirDialog dirDialog( this, wxT( "Select the directory to search in:" ), currentLookIn, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST );

    if ( dirDialog.ShowModal() == wxID_OK )
    {
      // clean the user input
      std::string cleanDir;
      FileSystem::CleanName( dirDialog.GetPath().c_str(), cleanDir );
      SetDirectory( cleanDir, true );
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // Called when a list control's column is clicked
  //
  void FileBrowser::OnColClick( wxListEvent& evt )
  {
    wxBusyCursor bc;

    m_listCtrlResults->SortItems( evt.GetColumn() );
  }


  /////////////////////////////////////////////////////////////////////////////
  // Called when an item is double-clicked.  Ends the dialog as if the OK
  // button had been pressed.
  // 
  void FileBrowser::OnItemActivated( wxListEvent& evt )
  {
    EndModal( wxID_OK );
  }

  /////////////////////////////////////////////////////////////////////////////
  // Called when an item in the list is selected.
  //
  void FileBrowser::OnItemSelection( wxListEvent& evt )
  {
    // early out if we are already have that file selected
    if ( m_SelectedFileInfo == m_FileInfoIndexTable[ evt.GetData() ] )
    {
      return;
    }

    m_SelectedFileInfo = m_FileInfoIndexTable[ evt.GetData() ];

    if ( m_SelectedFileInfo )
    {
      if ( FileSystem::Exists( m_SelectedFileInfo->GetPath() ) )
      {
        AddPath( m_SelectedFileInfo->GetPath() );
        BeginShowDetails( m_SelectedFileInfo );
      }
      else
      {
        std::string error = "The asset you selected does not exist on disk. When was the last time you got assets? \n";
        error += m_SelectedFileInfo->GetPath();
        wxMessageBox( error.c_str(), "Error", wxCENTER | wxOK | wxICON_ERROR, GetParent() );

        BeginShowDetails( NULL );
      }
    }    
  }

  /////////////////////////////////////////////////////////////////////////////
  // Called when an item in the list is deselected
  // 
  void FileBrowser::OnItemDeselection( wxListEvent& evt )
  {
    m_SelectedFileInfo = m_FileInfoIndexTable[ evt.GetData() ];

    if ( m_SelectedFileInfo )
    {
      RemovePath( m_SelectedFileInfo->GetPath() );
    }

    BeginShowDetails( NULL );
  }

  ////////////////////////////////////////////
  // create and start the show details thread
  void FileBrowser::BeginShowDetails( FileInfo* fileInfo )
  {
    m_ShowDetails = false;
    ::WaitForSingleObject( m_EventEndShowDetails, INFINITE );
    
    // start the UI throbbers
    if ( !m_ShowDetailsTimer )
    {
      m_ShowDetailsTimer = new ShowDetailsTimer( this );
    }
    m_ShowDetailsTimer->Start( 50 );


    class ShowDetailsThread : public wxThread
    {
    private:
      FileBrowser* m_FileBrowser;
      FileInfoPtr  m_FileInfo;

    public:
      ShowDetailsThread( FileBrowser* fileBrowser, FileInfo* fileInfo )
        : wxThread( wxTHREAD_DETACHED )
        , m_FileBrowser( fileBrowser )
        , m_FileInfo( fileInfo )
      {}

      virtual ~ShowDetailsThread()
      {}

      virtual wxThread::ExitCode Entry() NOC_OVERRIDE
      {
        m_FileBrowser->ShowDetails( m_FileInfo );
        return NULL;
      }
    }; 

    m_ShowDetails = true;

    // Detached threads delete themselves once they have completed,
    // and thus must be created on the heap
    ShowDetailsThread* showDetailsThread = new ShowDetailsThread( this, fileInfo );
    showDetailsThread->Create();
    showDetailsThread->Run();
  }


  ///////////////////////////////////////////////////////////////////////////////
  void FileBrowser::ShowDetails( FileInfo* fileInfo )
  {
    ::ResetEvent( m_EventEndShowDetails );

    Windows::TakeSection critSection( m_ShowDetailsCriticalSection );
    m_ShowDetailsChanged = true;

    m_ShowDetailsText.clear();
    m_ShowDetailsThumbnailPath.clear();

    // early out if there is no file to show
    if ( !fileInfo )
    {
      ::SetEvent( m_EventEndShowDetails );
      return;
    }

    if ( m_ShowDetails ) 
    {
      m_ShowDetailsThumbnailPath = fileInfo->GetThumbnailPath( );
    }

    if ( m_ShowDetails ) 
    {
      wxString buffer;
      for each ( const DisplayColumnType& displayType in m_DisplayDetailsList )
      {
        m_ShowDetailsText += m_DisplayColumns[displayType].Name();
        m_ShowDetailsText += ": ";
        m_ShowDetailsText += m_DisplayColumns[displayType].Data( fileInfo );
        m_ShowDetailsText += "\n\n";

        if ( !m_ShowDetails ) 
          break;
      }
    }

    m_ShowDetails = false;
    ::SetEvent( m_EventEndShowDetails );
  }


  ///////////////////////////////////////////////////////////////////////////////
  // Update the UI during the search, adding newly found files to m_listCtrlResults,
  // and updating the m_staticTextLookingIn value.
  // 
  void FileBrowser::ShowDetailsTimerTic()
  {
    if ( !m_ShowDetailsChanged )
    {
      return;
    }

    Windows::TakeSection critSection( m_ShowDetailsCriticalSection );
    m_ShowDetailsChanged = false;

    // clear details
    m_textCtrlDetails->Clear();
    m_bitmapThumbnail->Hide();

    
    m_textCtrlDetails->AppendText( m_ShowDetailsText );
    m_textCtrlDetails->ShowPosition( 0 );
    m_textCtrlDetails->Show();

    // show m_bitmapThumbnail
    if ( !m_ShowDetailsThumbnailPath.empty() && FileSystem::Exists( m_ShowDetailsThumbnailPath ) )
    {
      m_bitmapThumbnail->SetBitmap( UIToolKit::GlobalImageManager().GetScaledBitmap( m_ShowDetailsThumbnailPath, 150, UIToolKit::GlobalImageManager().GetBitmapLoadType( m_ShowDetailsThumbnailPath ) ) );
      wxBitmap bmp = m_bitmapThumbnail->GetBitmap();
      m_bitmapThumbnail->SetSize( wxSize( bmp.GetWidth(),bmp.GetHeight() ) ); 
      m_bitmapThumbnail->SetMaxSize( wxSize( bmp.GetWidth(),bmp.GetHeight() ) ); 
      m_bitmapThumbnail->Show();
    }
  }



  /////////////////////////////////////////////////////////////////////////////
  // 
  void FileBrowser::OnItemEndEdit( wxListEvent& evt )
  {
    if ( !evt.IsEditCancelled() )
      evt.Veto();
  }


  /////////////////////////////////////////////////////////////////////////////
  // Catch the RETURN OnChar event and simulate the search button being clicked
  //
  void FileBrowser::OnChar( wxKeyEvent& evt )
  { 
    wxString key;
    long keycode = evt.GetKeyCode();
    if ( keycode == WXK_RETURN )
    {
      return;
    }

    evt.Skip();
  }

  /////////////////////////////////////////////////////////////////////////////
  // Updates the "Look in" field if the chosen file type can narrow down the
  // search.
  //
  void FileBrowser::OnFileType( wxCommandEvent& evt )
  {
    evt.Skip();
  }





  //*************************************************************************//
  //
  //  DISPLAY FUNCTIONS
  //
  //*************************************************************************//



  /////////////////////////////////////////////////////////////////////////////
  //
  //
  void FileBrowser::HideDialogButtons()
  {
    ShowDialogButtons( false );
  }

  /////////////////////////////////////////////////////////////////////////////
  //
  //
  void FileBrowser::ShowDialogButtons( bool show )
  {
    m_panelDialogButtons->Show( show );

    Layout();
    this->Refresh();
  }

  /////////////////////////////////////////////////////////////////////////////
  //
  //
  void FileBrowser::InitResultsDisplay( )
  {
    wxBusyCursor bc;
    
    m_listCtrlResults->Freeze();

    m_listCtrlResults->ClearAll();
    m_CurFileInfoIndex = 0;
    m_FileInfoIndexTable.clear();

    // unselect the path (hidding the OK button again)
    ClearPaths();

    // create the table columns
    int colIndex = 0;

    for each ( const DisplayColumnType& displayType in m_DisplayColumnList )
    {
      m_listCtrlResults->InsertColumn( colIndex, m_DisplayColumns[displayType].Name().c_str(), wxLC_ALIGN_LEFT );
      m_listCtrlResults->SetColumnWidth( colIndex, m_DisplayColumns[displayType].Width() );
      ++colIndex;
    }

    m_listCtrlResults->Thaw();
  }


  /////////////////////////////////////////////////////////////////////////////
  // Inserts the new file into the m_FileInfos set as well as the 
  // m_FileInfoIndexTable
  //
  void FileBrowser::AddResult( FileInfo* fileInfo )
  {
    //////////////////////
    // insert the file into the set
    Insert<S_FileInfoPtr>::Result insertSet = m_FileInfos.insert( fileInfo );
    if ( !insertSet.second )
    {
      // FIXME: report error?
      return;
    }

    g_FileInfos.push_back( fileInfo );

    // skip files with wrong extensions
    std::string filter = m_choiceFiletype->GetStringSelection().c_str();
    const Filter* foundFilter = FindFilter( filter );
    if ( foundFilter != NULL )
    {
      const S_string& foundExtensions = foundFilter->GetExtensions();

      if ( ( foundExtensions.find( ".*" ) == foundExtensions.end() )
        && ( foundExtensions.find( fileInfo->GetExtension() ) == foundExtensions.end() ) )
      {
        return;
      }
    }

    

    //////////////////////
    // Insert the new item into the list control

    // File image icon
    i32 imageIndex = wxFileIconsTable::file;
    std::string fileExtension = fileInfo->GetExtension();
    if ( !fileExtension.empty() )
    {
      imageIndex = wxTheFileIconsTable->GetIconID( fileExtension.c_str() );
    }

    // File name
    wxString buf;
    buf.Printf( wxT( "%s" ), fileInfo->GetName().c_str() );
    i32 rowIndex = m_listCtrlResults->InsertItem( m_CurFileInfoIndex, buf, imageIndex );
    if ( rowIndex == -1 )
    {
      // FIXME: report error?
      return;
    }
    m_listCtrlResults->SetItemData( rowIndex, m_CurFileInfoIndex );

    //////////////////////
    // insert the data index and file info pointer into the m_FileInfoIndexTable
    m_FileInfoIndexTable[m_CurFileInfoIndex] = (*insertSet.first);

    // advance the index for the next call to AddResult
    ++m_CurFileInfoIndex;


    //////////////////////
    // populate the rest of the row
    int colIndex = 0;

    for each ( const DisplayColumnType& displayType in m_DisplayColumnList )
    {
      m_listCtrlResults->SetItem( rowIndex, colIndex, m_DisplayColumns[displayType].Data( fileInfo ).c_str() );
      ++colIndex;
    }
  }




  void FileBrowser::EnableOperationButton( bool enable )
  {
    m_buttonOperation->Enable( enable );
  }


  //*************************************************************************//
  //
  //  MEMBER FUNCTIONS
  //
  //*************************************************************************//
  
  /////////////////////////////////////////////////////////////////////////////
  // Adds a path to the list of currently selected files.
  // 
  void FileBrowser::AddPath( const std::string& path )
  {
    m_Paths.insert( path );

    EnableOperationButton( !m_Paths.empty() );
  }

  /////////////////////////////////////////////////////////////////////////////
  // Removes the specified path from the list of selected files.
  // 
  void FileBrowser::RemovePath( const std::string& path )
  {
    m_Paths.erase( path );

    EnableOperationButton( !m_Paths.empty() );
  }

  /////////////////////////////////////////////////////////////////////////////
  // Removes all paths from the list of selected files.
  // 
  void FileBrowser::ClearPaths()
  {
    m_Paths.clear();

    EnableOperationButton( false );
  }

  ///////////////////////////////////////////////////////////////////////////////
  // Collects and error checks the search criteria from the form, and populates
  // the struct m_SearchCriteria. Returns true if no errors were found. Also
  // stores the MRU for combo box form fields
  // 
  bool FileBrowser::CollectSearchCriteria()
  {
    wxBusyCursor bc;
    
    m_SearchCriteria->clear();

    // File History Data --------------------------
    m_SearchCriteria->m_SearchHistoryData = m_checkBoxSeachHistoryData->IsChecked();

    // TUID-------------------------------
    std::string strTUID = m_comboBoxTUID->GetValue().c_str();
    if ( !strTUID.empty() )
    {
      std::istringstream idStream (strTUID);
      tuid id;
      
      // if the TUID is in HEX
      if ( strTUID.size() > 2 && ( strTUID[0] == '0' && (strTUID[1] == 'x' || strTUID[1] == 'X' )))
      {
        idStream >> std::hex >> id;
      }
      else
      {
        idStream >> id;
      }

      if ( id == 0 || id == _UI64_MAX )
      {
        wxMessageBox( "The asset ID you specified is not valid.", "Error", wxCENTER | wxOK | wxICON_ERROR, GetParent() );
        return false;
      }

      m_SearchCriteria->m_SearchMode = m_SearchCriteria->m_SearchMode | SearchModes::AssetID;
      m_SearchCriteria->m_TUID = id;
    }
    g_FieldMRU->AddItem( (wxControlWithItems*) m_comboBoxTUID, "m_comboBoxTUID", strTUID );


    // Look in folder-------------------------------
    wxString comboBoxLookIn = m_comboBoxLookIn->GetValue();
    if ( !comboBoxLookIn.empty() )
    {
      m_SearchCriteria->m_LookIn = comboBoxLookIn.c_str();

      // clean user input
      FileSystem::CleanName( m_SearchCriteria->m_LookIn );
      FileSystem::GuaranteeSlash( m_SearchCriteria->m_LookIn ); 
      g_FieldMRU->AddItem( (wxControlWithItems*) m_comboBoxLookIn, "m_comboBoxLookIn", m_SearchCriteria->m_LookIn );  
    }
    

    // Search Query-------------------------------
    wxString comboBoxSearch = m_comboBoxSearch->GetValue();
    comboBoxSearch.Trim(true);  // trim white-space right 
    comboBoxSearch.Trim(false); // trim white-space left
    if ( !comboBoxSearch.empty() )
    {
      m_SearchCriteria->m_SearchMode = m_SearchCriteria->m_SearchMode | SearchModes::SearchQuery;
      m_SearchCriteria->m_SearchQuery = comboBoxSearch.c_str();
    }
    g_FieldMRU->AddItem( (wxControlWithItems*) m_comboBoxSearch, "m_comboBoxSearch", m_SearchCriteria->m_SearchQuery );


    // File type filter-------------------------------
    std::string filter = m_choiceFiletype->GetStringSelection().c_str();
    const Filter* foundFilter = FindFilter( filter );
    if ( foundFilter != NULL )
    {
      m_SearchCriteria->m_Filter = foundFilter->GetExtensions();
    }
    else
    {
      wxMessageBox( "The file extension you specified is not valid.", "Error", wxCENTER | wxOK | wxICON_ERROR, GetParent() );
      return false;
    }


    // get the search queries-------------------------------
    std::string searchQuery = "";
    if ( m_SearchCriteria->m_SearchMode & SearchModes::SearchQuery )
    {
      searchQuery = !m_SearchCriteria->m_SearchQuery.empty() ? m_SearchCriteria->m_SearchQuery : "*";
    }
    
    if ( !FileSystem::HasPrefix( m_SearchCriteria->m_LookIn, searchQuery ) )
    {
      searchQuery = m_SearchCriteria->m_LookIn + std::string( "*" ) + searchQuery;
    }
    

    // if they provided an extension that's part of this filter, add it specifically
    std::string queryExtension = FileSystem::GetExtension( searchQuery );
    for each ( const std::string& itFilter in m_SearchCriteria->m_Filter )
    {
      // rat.en => [.entity.irb].find( [.en] )
      if ( !queryExtension.empty() && itFilter.find( queryExtension ) == 0 )
      {
        std::string addQuery = searchQuery;
        FileSystem::StripExtension( addQuery );
        addQuery += itFilter;
        m_SearchCriteria->m_SearchQueries.insert( m_SearchCriteria->m_SearchQueries.begin(), 1, addQuery );
    }

      m_SearchCriteria->m_SearchQueries.push_back( searchQuery + std::string("*") + itFilter );
    }

    return true;
  }


  ///////////////////////////////////////////////////////////////////////////////
  // Used in Search() to create begin and end search events
  inline void FileBrowser::BeginSearchEvent()
  {
    ::ResetEvent( m_EventEndSearch );

    wxCommandEvent evtBeginSearch( wxEVT_BEGIN_SEARCH );
    wxPostEvent( this, evtBeginSearch );
  }

  inline void FileBrowser::EndSearchEvent()
  {
    wxCommandEvent evtEndSearch( wxEVT_END_SEARCH );
    wxPostEvent( this, evtEndSearch );

    m_ContinueSearching = false;

    ::SetEvent( m_EventEndSearch );
  }



  ///////////////////////////////////////////////////////////////////////////////
  // Search for files metting the given criteria
  // 
  void FileBrowser::Search()
  {
    wxBusyCursor bc;
    
    BeginSearchEvent();

    // search managed files only options
    if ( m_SearchCriteria->m_SearchMode & SearchModes::AssetID )
    {
      SearchByTUID();
    } 

    if ( !m_ContinueSearching )
    {
      EndSearchEvent();
      return;
    } 

    // search FileManager
    if ( m_SearchCriteria->m_SearchMode & SearchModes::SearchQuery )
    {
      SearchManagedFiles();
    }

    if ( !m_ContinueSearching )
    {
      EndSearchEvent();
      return;
    } 
    
    // search File HistoryData - files that have been renamed
    if ( m_SearchCriteria->m_SearchHistoryData )
    {
      SearchByHistoryData();
    }

    if ( !m_ContinueSearching )
    {
      EndSearchEvent();
      return;
    } 

    EndSearchEvent();
    return;
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  void FileBrowser::SearchByTUID()
  {
    if ( m_SearchCriteria->m_TUID > 0 )
    {
      char buf[200];
      sprintf( buf, "Asset TUID: %I64u...", m_SearchCriteria->m_TUID );
      ::EnterCriticalSection( &m_LookingInCriticalSection );
      m_LookingIn = buf;
      ::LeaveCriticalSection( &m_LookingInCriticalSection );

      File::ManagedFilePtr managedFile = File::GlobalManager().GetManagedFile( m_SearchCriteria->m_TUID );
      if ( managedFile.ReferencesObject() )
      {
        FileInfoPtr fileInfo = new FileInfo();
        managedFile->CopyTo( fileInfo );

        ::EnterCriticalSection( &m_FoundFilesCriticalSection );
        m_FoundFiles.push_back( fileInfo );
        ::LeaveCriticalSection( &m_FoundFilesCriticalSection );
      }
    }
  }


  ///////////////////////////////////////////////////////////////////////////////
  // Search for files metting the given criteria
  //
  void FileBrowser::SearchByHistoryData()
  {
    // search with the basic criteria
    for each ( const std::string& itSearchQuery in m_SearchCriteria->m_SearchQueries )
    {
      if ( !m_ContinueSearching )
        return;

      char buf[200];
      sprintf( buf, "Asset named %s...", itSearchQuery.c_str() );
      ::EnterCriticalSection( &m_LookingInCriticalSection );
      m_LookingIn = buf;
      ::LeaveCriticalSection( &m_LookingInCriticalSection );

      FindManagedFilesByHistory( itSearchQuery, "", File::GetPatchOperationString( PatchOperations::Update ), true );
    }
  }

  ///////////////////////////////////////////////////////////////////////////////
  // Search for files metting the given criteria
  //
  void FileBrowser::FindManagedFilesByHistory( const std::string& searchQuery, const std::string& modifiedBy, const std::string& operation, bool searchHistoryData )
  {
    if ( !m_ContinueSearching )
      return;

    try
    {
      File::V_ManagedFilePtr listOfFiles;
      File::GlobalManager().FindFilesByHistory( searchQuery, modifiedBy, listOfFiles, operation, searchHistoryData );

      ::EnterCriticalSection( &m_FoundFilesCriticalSection );

      File::V_ManagedFilePtr::iterator it = listOfFiles.begin();
      File::V_ManagedFilePtr::iterator itEnd = listOfFiles.end();
      for ( ; it != itEnd; ++it )
      {
        FileInfoPtr fileInfo = new FileInfo();
        (*it)->CopyTo( fileInfo );
        m_FoundFiles.push_back( fileInfo );
      }

      ::LeaveCriticalSection( &m_FoundFilesCriticalSection );
    }
    catch( const Nocturnal::Exception& )
    {
      //SetStatusMsg( StatusMsgLevels::Error, "%s", ex.what() );
    } 
  }


  ///////////////////////////////////////////////////////////////////////////////
  // Search for files metting the given criteria
  //
  void FileBrowser::SearchManagedFiles()
  {
    // search with the basic criteria
    for each ( const std::string& itSearchQuery in m_SearchCriteria->m_SearchQueries )
    {
      if ( !m_ContinueSearching )
        return;

      FindManagedFiles( itSearchQuery );
    }
  }

  ///////////////////////////////////////////////////////////////////////////////
  // Search for files metting the given criteria
  //
  void FileBrowser::FindManagedFiles( const std::string& searchQuery )
  {
    if ( !m_ContinueSearching )
      return;

    ::EnterCriticalSection( &m_LookingInCriticalSection );
    m_LookingIn = searchQuery;
    ::LeaveCriticalSection( &m_LookingInCriticalSection );

    try
    {
      File::V_ManagedFilePtr listOfFiles;
      File::GlobalManager().Find( searchQuery, listOfFiles );

      ::EnterCriticalSection( &m_FoundFilesCriticalSection );

      File::V_ManagedFilePtr::iterator it = listOfFiles.begin();
      File::V_ManagedFilePtr::iterator itEnd = listOfFiles.end();
      for ( ; it != itEnd; ++it )
      {
        FileInfoPtr fileInfo = new FileInfo();
        (*it)->CopyTo( fileInfo );
        m_FoundFiles.push_back( fileInfo );
      }

      ::LeaveCriticalSection( &m_FoundFilesCriticalSection );

    }
    catch( const Nocturnal::Exception& )
    {
      //SetStatusMsg( StatusMsgLevels::Error, "%s", ex.what() );
    } 
  }



  ///////////////////////////////////////////////////////////////////////////////
  //
  void FileBrowser::SearchFilesOnDisk()
  {
    std::string searchQuery;

    if ( m_SearchCriteria->m_SearchMode & SearchModes::SearchQuery )
      searchQuery += m_SearchCriteria->m_SearchQuery;

    for each ( const std::string& itFilter in m_SearchCriteria->m_Filter )
    {
      if ( !m_ContinueSearching )
        return;

      std::string filteredSearchQuery = searchQuery;
      if ( FileSystem::GetExtension( filteredSearchQuery ) != itFilter )
      {
        FileSystem::StripExtension( filteredSearchQuery );
        filteredSearchQuery += "*" + itFilter;
      }

      FindFilesOnDisk( m_SearchCriteria->m_LookIn, filteredSearchQuery );
    }
  }


  ///////////////////////////////////////////////////////////////////////////////
  // Search for files metting the given criteria
  // 
  void FileBrowser::FindFilesOnDisk( const std::string &dirPath, const std::string &spec )
  {
    if ( !m_ContinueSearching )
      return;

    ::EnterCriticalSection( &m_LookingInCriticalSection );
    m_LookingIn = dirPath + spec;
    ::LeaveCriticalSection( &m_LookingInCriticalSection );

    try
    {
      FileSystem::FileIterator fileIter( dirPath, spec, (FileSystem::FileIteratorFlags) FileSystem::IteratorFlags::NoDirs );
      while ( !fileIter.IsDone() )
      {
        if ( !m_ContinueSearching )
          return;

        FileInfoPtr fileInfo = new FileInfo( fileIter.Item() );

        ::EnterCriticalSection( &m_FoundFilesCriticalSection );
        m_FoundFiles.push_back( fileInfo );
        ::LeaveCriticalSection( &m_FoundFilesCriticalSection );

        fileIter.Next();
      }

      FileSystem::FileIterator dirIter( dirPath, "*.*", (FileSystem::FileIteratorFlags) FileSystem::IteratorFlags::NoFiles );

      while ( !dirIter.IsDone() )
      {
        if ( !m_ContinueSearching )
          return;

        FindFilesOnDisk( dirIter.Item(), spec );

        dirIter.Next();
      }
    }
    catch( const Nocturnal::Exception& )
    {
      // do nothing for now
    }
  }


  // Helper function to find a filter by name
  const Filter* FileBrowser::FindFilter( const std::string& name )
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

} // namespace File
