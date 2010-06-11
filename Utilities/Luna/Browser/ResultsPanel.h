#pragma once

#include "BrowserMenuIDs.h"
#include "BrowserGenerated.h"
#include "BrowserFrame.h"
#include "BrowserSearchPanel.h"
#include "Foundation/Memory/SmartPtr.h"

namespace Asset
{
    class AssetFile;
    class AssetFile;
    typedef Nocturnal::SmartPtr< AssetFile > AssetFilePtr;
    typedef std::vector< AssetFilePtr > V_AssetFiles;

    class AssetFolder;
    typedef Nocturnal::SmartPtr< AssetFolder > AssetFolderPtr;
    typedef std::vector< AssetFolderPtr > V_AssetFolders;
}

namespace Luna
{
    //
    // Forwards
    //
    class ThumbnailView;
    struct ThumbnailSelectionArgs;
    struct ThumbnailHighlightArgs;

    class SearchResults;
    typedef Nocturnal::SmartPtr< SearchResults > SearchResultsPtr;

    namespace ViewModes
    {
        enum ViewMode
        {
            Invalid,   // Do not use, just for initialization

            Thumbnail,
            AdvancedSearch,
        };
    }
    typedef ViewModes::ViewMode ViewMode;

    // Selection change event signature and arguments
    struct ResultChangeArgs
    {
        size_t m_NumSelected;
        std::string m_HighlightPath;

        ResultChangeArgs( size_t numSelected, const std::string& highlight )
            : m_NumSelected( numSelected )
            , m_HighlightPath( highlight )
        {
        }

        ResultChangeArgs()
            : m_NumSelected( 0 )
            , m_HighlightPath( "" )
        {
        }
    };
    typedef Nocturnal::Signature< void, const ResultChangeArgs& > ResultSignature;

    // Highlight change event signature and arguments
    struct ResultHighlightChangeArgs
    {
        std::string m_HighlightPath;

        ResultHighlightChangeArgs( const std::string& path )
            : m_HighlightPath( path )
        {
        }
    };
    typedef Nocturnal::Signature< void, const ResultHighlightChangeArgs& > ResultHighlightSignature;

    ///////////////////////////////////////////////////////////////////////////////
    /// Class ResultsPanel
    ///////////////////////////////////////////////////////////////////////////////
    class ResultsPanel : public ResultsPanelGenerated 
    {		
    public:
        ResultsPanel( const std::string& rootDirectory, BrowserFrame* browserFrame );
        virtual ~ResultsPanel();

        void SetViewMode( ViewMode view );
        ViewMode GetViewMode() const;

        void SetThumbnailSize( u16 zoom );
        void SetThumbnailSize( ThumbnailSize zoom );

        void SetResults( SearchResults* results );
        void ClearResults();

        void SelectPath( const std::string& path );
        u32 GetSelectedPaths( V_string& paths, bool useForwardSlashes = true );
        void GetSelectedFilesAndFolders( Asset::V_AssetFiles& files, Asset::V_AssetFolders& folders );

        u32 GetNumFiles() const;
        u32 GetNumFolders() const;

    private:
        void OnThumbnailSelectionChanged( const ThumbnailSelectionArgs& args );
        void OnThumbnailHighlightChanged( const ThumbnailHighlightArgs& args );

    private:
        std::string m_RootDirectory;
        ViewMode m_CurrentMode;
        wxWindow* m_CurrentView;
        ThumbnailView* m_ThumbnailView;
        BrowserSearchPanel* m_BrowserSearchPanel;
        BrowserFrame* m_BrowserFrame;

    private:
        ResultSignature::Event m_ResultsChanged;
    public:
        void AddResultsChangedListener( const ResultSignature::Delegate& listener )
        {
            m_ResultsChanged.Add( listener );
        }
        void RemoveResultsChangedListener( const ResultSignature::Delegate& listener )
        {
            m_ResultsChanged.Remove( listener );
        }

    private:
        DECLARE_EVENT_TABLE();
    };
}
