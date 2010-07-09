#pragma once

#include "VaultMenuIDs.h"
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
    class VaultPreferences;
    typedef Nocturnal::SmartPtr< VaultPreferences > VaultPreferencesPtr;

    class VaultFrame;

    class VaultPreferences : public Reflect::ConcreteInheritor< VaultPreferences, Preferences >
    {
    public:
        VaultPreferences( const tstring& defaultFolder = TXT( "" ),
                            ViewOptionID thumbnailMode = ViewOptionIDs::Medium,
                            u32 thumbnailSize = ThumbnailSizes::Medium );
        ~VaultPreferences();

        virtual const tstring& GetCurrentVersion() const;
        virtual tstring GetPreferencesPath() const;

        void GetWindowSettings( VaultFrame* browserFrame, wxAuiManager* manager = NULL );
        void SetWindowSettings( VaultFrame* browserFrame, wxAuiManager* manager = NULL );

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

        const tstring& GetDefaultFolderPath() const;
        void SetDefaultFolderPath( const tstring& path );

        CollectionManager* GetCollectionManager() { return m_CollectionManager; }
        SearchHistory* GetSearchHistory() { return m_SearchHistory; }

    public:
        REFLECT_DECLARE_CLASS( VaultPreferences, Preferences );
        static void EnumerateClass( Reflect::Compositor< VaultPreferences >& comp );

    private:
        WindowSettingsPtr     m_WindowSettings;
        tstring               m_DefaultFolder;
        ViewOptionID          m_ThumbnailMode;
        u32                   m_ThumbnailSize;
        bool                  m_DisplayPreviewAxis;
        CollectionManagerPtr  m_CollectionManager;
        SearchHistoryPtr      m_SearchHistory;
        u32                   m_DependencyCollectionRecursionDepth;
        u32                   m_UsageCollectionRecursionDepth;
    };
}
