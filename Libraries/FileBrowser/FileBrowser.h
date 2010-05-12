#pragma once

#include <vector>
#include <map>

#include <string>

#include "API.h"
#include <wx/wx.h>
#include <wx/listctrl.h>

#include "Finder/FinderSpec.h"

#include "TUID/TUID.h"
#include "Common/Types.h"

#include "FileInfo.h"
#include "FileBrowserGenerated.h"

namespace File
{
  //
  // Forward declares
  //

  class SearchCriteria;
  class DisplayOptions;
  class UpdateTimer;
  class ThrobberTimer;
  class ShowDetailsTimer;
  class ShowDetailsThread;

  /////////////////////////////////////////////////////////////////////////////
  // Small class to manage the filter information.
  // 
  class Filter
  {
  private:
    std::string m_Name;
    std::string m_Filters;
    S_string    m_Extensions;

  public:
    Filter( const std::string& name, const std::string& filters );

    virtual ~Filter()
    {
      clear();
    }

    void clear()
    {
      m_Filters.clear();
      m_Extensions.clear();
    }

    bool operator<( const Filter& rhs ) const
    {
      return m_Name < rhs.m_Name;
    }

    const std::string& GetName() const
    {
      return m_Name;
    }

    const std::string& GetFilters() const
    {
      return m_Filters;
    }

    const S_string& GetExtensions() const
    {
      return m_Extensions;
    }
  };


  /////////////////////////////////////////////////////////////////////////////

  namespace DisplayColumnTypes
  {
    enum DisplayColumnType
    {
      NONE        = 0,
      Name        = 1,
      FileType,
      Folder,
      CreatedBy,
      Size,
      Id,
    };
  }
  typedef FILEBROWSER_API DisplayColumnTypes::DisplayColumnType DisplayColumnType;
  typedef FILEBROWSER_API std::vector< DisplayColumnType > V_DisplayColumnType;
  
  /////////////////////////////////////////////////////////////////////////////

  class FILEBROWSER_API FileBrowser : public FileBrowserGenerated
  {
  public:
    /////////////////////////////////////////////
    //
    // Ctor/Dtor
    //

    FileBrowser( 
      wxWindow* parent,
      int id = wxID_ANY, 
      wxString title = wxEmptyString, 
      wxPoint pos = wxDefaultPosition, 
      wxSize size = wxSize( 800, 600 ), 
      int style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

    void Create( 
      wxWindow* parent,
      int id = wxID_ANY, 
      wxString title = wxEmptyString, 
      wxPoint pos = wxDefaultPosition, 
      wxSize size = wxSize( 800, 600 ), 
      int style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );


    virtual ~FileBrowser();


    /////////////////////////////////////////////
    //
    // API
    //

    // gets the currently selected file path(s)
    std::string GetPath() const;
    const S_string& GetPaths() const;

    // returns the currently selected filter eg: "*.*", "*.png;*.jpg", etc...
    std::string GetFilter(); 

    // sets the "Look in" directory
    void SetDirectory( const std::string& directory = "", bool userSpecified = false );

    void SetFilter( const Finder::FinderSpec& filterSpec );
    void SetFilter( const char* filter );

    void AddFilter( const Finder::FinderSpec& filterSpec );   
    void AddFilter( const char* filter );

    void SetFilterIndex( int filterIndex );
    void SetFilterIndex( const Finder::FinderSpec& spec );

    bool IsTuidRequired() const;
    void SetTuidRequired( bool isRequired = true );

    void SetRequestedFileID( const tuid& request );

    void EnableMultipleSelection( bool enable = true );
    void DisableMultipleSelection() { EnableMultipleSelection( false ); }
    bool IsMultipleSelectionEnabled() const;


    tuid GetFileID() const;
    const S_tuid& GetFileIDs() const;


    void ShowDialogButtons( bool show = true );
    void HideDialogButtons();



    /////////////////////////////////////////////
    //
    // Dialog API
    //

    // show the dialog modally and return the value passed to EndModal()
    virtual int ShowModal();

    // may be called to terminate the dialog with the given return code
    virtual void EndModal( int retCode = wxID_CANCEL );

    virtual bool ValidatePath( const std::string& path, std::string& error ) { return true; }

    friend class UpdateTimer;
    friend class ThrobberTimer;
    friend class ShowDetailsTimer;
    friend class ShowDetailsThread;

  protected:

    DECLARE_EVENT_TABLE()

   
    /////////////////////////////////////////////
    //
    // Event processor functions
    //

    void OnSize( wxSizeEvent& evt );

    void OnOK( wxCommandEvent& evt );
    void OnCancel( wxCommandEvent& evt );
    void OnClose( wxCloseEvent& args );

    void FileBrowser::StartSearch();
    void OnSearch( wxCommandEvent& evt ) { StartSearch(); }

    void OnBeginSearch( wxCommandEvent& evt );
    void OnEndSearch( wxCommandEvent& evt );

    void OnClear( wxCommandEvent& evt );

    void OnBrowseButton( wxCommandEvent& evt );
    
    void OnColClick( wxListEvent& evt );
    void OnItemActivated( wxListEvent& evt );
    void OnItemSelection( wxListEvent& evt );
    void OnItemDeselection( wxListEvent& evt );
    void OnItemEndEdit( wxListEvent& evt );

    void OnChar( wxKeyEvent& evt );
    void OnFileType( wxCommandEvent& evt );

    
    virtual void EnableOperationButton( bool enable = true );


    /////////////////////////////////////////////
    //
    // Dialog API
    //

    // common part of all ctors
    void Init();


  private:

    /////////////////////////////////////////////
    //
    // Ctor/Dtor create and destroy functions
    //

    void UpdateFilter();


    /////////////////////////////////////////////
    //
    // Display functions
    //

    void ThrobberTimerTic();
    void UpdateTimerTic();
    void ShowDetailsTimerTic();

    void InitResultsDisplay();

    void AddResult( FileInfo* fileInfo );
    void PopulateResults( V_FileInfoPtr& foundFiles );
    void RepopulateResults();
    
    void AddPath( const std::string& path );
    void RemovePath( const std::string& path );
    void ClearPaths();

    void BeginShowDetails( FileInfo* fileInfo );
    void ShowDetails( FileInfo* fileInfo );

    void SetStatusText( const std::string& searchingText );


    /////////////////////////////////////////////
    //
    // Search
    //

    bool CollectSearchCriteria();

    void Search();
    inline void BeginSearchEvent();
    inline void EndSearchEvent();
    
    void SearchByTUID();

    void FindManagedFilesByHistory( const std::string& searchQuery, const std::string& modifiedBy, const std::string& operation = "%", bool searchHistoryData = false );

    void SearchManagedFiles();
    void SearchByHistoryData();
    void FindManagedFiles( const std::string& searchQuery );

    void SearchFilesOnDisk();
    void FindFilesOnDisk( const std::string &dirPath, const std::string &spec = "" );

    const Filter* FindFilter( const std::string& name );

    /////////////////////////////////////////////
    //
    // Members
    //
    typedef Nocturnal::OrderedSet< Filter >       OS_Filter;
    typedef std::map< i32, FileInfoPtr >          M_FileInfoIndex;  // the map for the ListCtrl data

    int                       m_ReturnCode;
    S_string                  m_Paths;

    bool                      m_IsTuidRequired;
    S_tuid                    m_FileIDs; 

    tuid                      m_RequestedFileID;

    bool                      m_IsLookInUserDefined;
    OS_Filter                 m_Filters;

    FileInfoPtr               m_SelectedFileInfo;

    long                      m_CurFileInfoIndex;
    S_FileInfoPtr             m_FileInfos;
    M_FileInfoIndex           m_FileInfoIndexTable;

    ShowDetailsTimer*         m_ShowDetailsTimer;
    bool                      m_ShowDetails;
    bool                      m_ShowDetailsChanged;
    HANDLE                    m_EventEndShowDetails;
    ::CRITICAL_SECTION        m_ShowDetailsCriticalSection;
    std::string               m_ShowDetailsText;
    std::string               m_ShowDetailsThumbnailPath;

    // searching
    SearchCriteria*           m_SearchCriteria;

    bool                      m_ContinueSearching;

    std::string               m_SearchingText;
    std::string               m_LookingIn;
    V_FileInfoPtr             m_FoundFiles;

    HANDLE                    m_EventEndSearch;

    ::CRITICAL_SECTION        m_FoundFilesCriticalSection;
    ::CRITICAL_SECTION        m_LookingInCriticalSection;

    UpdateTimer*              m_UpdateTimer;
    ThrobberTimer*            m_ThrobberTimer;
    int                       m_DotIndex;

  
  protected:

    /////////////////////////////////////////////////////////////////////////////
    typedef std::string (*GetFileInfoDataFunc)( FileInfo* fileInfo );

    class DisplayColumn
    {
    public: 
      DisplayColumn()
        : m_DisplayColumnType( DisplayColumnTypes::NONE )
        , m_ColumnName( "" )
        , m_ColumnWidth( 200 )
        , m_GetFileInfoDataFunc( NULL )
      {
      }

      DisplayColumn( DisplayColumnType displayColType, const char* colName, int colWidth, GetFileInfoDataFunc getDataFunc )
        : m_DisplayColumnType( displayColType )
        , m_ColumnName( colName )
        , m_ColumnWidth( colWidth )
        , m_GetFileInfoDataFunc( getDataFunc )
      {
      }

      DisplayColumnType   Type() { return m_DisplayColumnType; }
      const std::string&  Name() { return m_ColumnName; }
      int                 Width() { return m_ColumnWidth; }
      std::string         Data( FileInfo* fileInfo )
      {
        if ( m_GetFileInfoDataFunc != NULL )
        {
          return m_GetFileInfoDataFunc( fileInfo ); 
        }

        return std::string( "" );
      }

    private:
      DisplayColumnType     m_DisplayColumnType;
      std::string           m_ColumnName;
      int                   m_ColumnWidth;
      GetFileInfoDataFunc   m_GetFileInfoDataFunc;
    };

    typedef std::map< DisplayColumnType, DisplayColumn > M_DisplayColumn;
    M_DisplayColumn           m_DisplayColumns;

    V_DisplayColumnType       m_DisplayColumnList;
    V_DisplayColumnType       m_DisplayDetailsList;

  };

}
