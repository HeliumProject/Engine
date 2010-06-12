#pragma once

#include "BrowserMenuIDs.h"
#include "CollectionManager.h"
#include "SearchHistory.h"
#include "SearchQuery.h"

#include "Foundation/Container/OrderedSet.h"
#include "Editor/WindowSettings.h"
#include "Editor/Preferences.h"
#include "Foundation/TUID.h"

namespace Luna
{
    /////////////////////////////////////////////////////////////////////////////
    class BrowserPreferences;
    typedef Nocturnal::SmartPtr< BrowserPreferences > BrowserPreferencesPtr;

    class BrowserFrame;

    class BrowserPreferences : public Reflect::ConcreteInheritor< BrowserPreferences, Preferences >
    {
    public:
        BrowserPreferences( const std::string& defaultFolder = "",
                            ViewOptionID thumbnailMode = ViewOptionIDs::Medium,
                            u32 thumbnailSize = ThumbnailSizes::Medium );
        ~BrowserPreferences();

        virtual const std::string& GetCurrentVersion() const;
        virtual std::string GetPreferencesPath() const;

        void GetWindowSettings( BrowserFrame* browserFrame, wxAuiManager* manager = NULL );
        void SetWindowSettings( BrowserFrame* browserFrame, wxAuiManager* manager = NULL );

        const ViewOptionID GetThumbnailMode() const;
        void SetThumbnailMode( ViewOptionID thumbnailMode );

        const u32 GetThumbnailSize() const;
        void SetThumbnailSize( u32 thumbnailSize );

        bool DisplayPreviewAxis() const;
        void SetDisplayPreviewAxis( bool display );
        const Reflect::Field* DisplayPreviewAxisField() const;

        u32 GetUsageCollectionRecursionDepth() const;
        void SetUsageCollectionRecursionDepth( u32 recursionDepth );
        const Reflect::Field* UsageCollectionRecursionDepth() const;

        u32 GetDependencyCollectionRecursionDepth() const;
        void SetDependencyCollectionRecursionDepth( u32 recursionDepth );
        const Reflect::Field* DependencyCollectionRecursionDepth() const;

        const std::string& GetDefaultFolderPath() const;
        void SetDefaultFolderPath( const std::string& path );

        CollectionManager* GetCollectionManager() { return m_CollectionManager; }
        SearchHistory* GetSearchHistory() { return m_SearchHistory; }

    public:
        REFLECT_DECLARE_CLASS( BrowserPreferences, Preferences );
        static void EnumerateClass( Reflect::Compositor< BrowserPreferences >& comp );

    private:
        WindowSettingsPtr     m_WindowSettings;
        std::string           m_DefaultFolder;
        ViewOptionID          m_ThumbnailMode;
        u32                   m_ThumbnailSize;
        bool                  m_DisplayPreviewAxis;
        CollectionManagerPtr  m_CollectionManager;
        SearchHistoryPtr      m_SearchHistory;
        u32                   m_DependencyCollectionRecursionDepth;
        u32                   m_UsageCollectionRecursionDepth;
    };
}
