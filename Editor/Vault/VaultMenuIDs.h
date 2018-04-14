#pragma once

#include "Platform/Types.h"
#include "Platform/Assert.h"
#include "Reflect/MetaEnum.h"
#include <sstream>

namespace Helium
{
    namespace Editor
    {
        namespace VaultMenu
        {
            ///////////////////////////////////////////////////////////////////////////
            enum MenuIDs
            {
                START = wxID_HIGHEST, // Do not use: must be first

                HelpPanel,

                ViewResultDetails,
                ViewResultList,

                ViewThumbnailsSmall,
                ViewThumbnailsMedium,
                ViewThumbnailsLarge,

                CopyPathNative,
                CopyPath,

                Sync,
                CheckOut,
                History,

                Open,
                Preview,

                Refresh,

                New,
                NewFolder,
                Cut,
                Copy,
                Paste,
                Rename,
                Delete,
                Properties,

                SelectAll,

                Sort,
                SortByName,
                SortByType,

                ShowInFolders,
                ShowInPerforce,
                ShowInWindowsExplorer,

                Settings,

                COUNT_PLUS_START // Do not use: must be last
            };
            static const uint32_t s_Count = COUNT_PLUS_START - START;

            ///////////////////////////////////////////////////////////////////////////
            static const char* s_Labels[s_Count] = 
            {
                "INVALID", // placeholder for START

                "Help",

                "Details",
                "List",

                "Small",
                "Medium",
                "Large",

                "FilePath (Native Format)",
                "FilePath (Internal Tools Format)",

                "Sync",
                "Check Out",
                "Revision History",

                "Open",
                "Preview",

                "Refresh",

                "New",
                "New Folder",
                "Cut",
                "Copy",
                "Paste",
                "Rename",
                "Delete",
                "Properties",

                "Select All\tCtrl+A",

                "Sort",
                "Name",
                "Type",

                "Folders",
                "Show in Revision Control",
                "Windows Explorer", 

                "Customize Settings...",
            };

            inline const char* Label( int32_t id )
            {
                HELIUM_ASSERT( id > START );
                HELIUM_ASSERT( id < COUNT_PLUS_START );
                return s_Labels[id-START];
            }
        };
        typedef VaultMenu::MenuIDs VaultMenuID;

        ///////////////////////////////////////////////////////////////////////
        class VaultViewMode
        {
        public:
            enum Enum
            {
                None,

                Details,
                List,

                ThumbnailsSmall,
                ThumbnailsMedium,
                ThumbnailsLarge,

                ThumbnailsCustom,
            };

            HELIUM_DECLARE_ENUM( VaultViewMode );

            static void PopulateMetaType( Reflect::MetaEnum& info )
            {
                info.AddElement( Details,           "Details" );
                info.AddElement( List,              "List" );
                info.AddElement( ThumbnailsSmall,   "Small" );
                info.AddElement( ThumbnailsMedium,  "Medium" );
                info.AddElement( ThumbnailsLarge,   "Large" );
                info.AddElement( ThumbnailsCustom,  "Custom" );
            }
        };

        ///////////////////////////////////////////////////////////////////////
        namespace VaultThumbnailsSizes
        {
            enum VaultThumbnailsSize
            {
                Small   = 64,
                Medium  = 128,
                Large   = 256
            };
            inline std::string Label( VaultThumbnailsSize size )
            {
                std::stringstream str;
                str << "(" << size << "x" << size << ")";
                return str.str();
            }
        }
        typedef VaultThumbnailsSizes::VaultThumbnailsSize VaultThumbnailsSize;

        ///////////////////////////////////////////////////////////////////////
        // Sorting methods for the shown items
        namespace VaultSortMethods
        {
            enum VaultSortMethod
            {
                AlphabeticalByName,
                AlphabeticalByType,
            };
        }
        typedef VaultSortMethods::VaultSortMethod VaultSortMethod;

        ///////////////////////////////////////////////////////////////////////
        namespace VaultSortOptions
        {
            enum VaultSortOption
            {
                Refresh = 1 << 1,
                Force   = 1 << 2,
            };
        };
    }
}