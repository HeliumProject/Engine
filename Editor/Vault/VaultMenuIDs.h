#pragma once

#include "Platform/Types.h"
#include "Platform/Assert.h"
#include "Foundation/Reflect/Enumeration.h"
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
            static const u32 s_Count = COUNT_PLUS_START - START;

            ///////////////////////////////////////////////////////////////////////////
            static const tstring s_Labels[s_Count] = 
            {
                TXT( "INVALID" ), // placeholder for START

                TXT( "Help" ),

                TXT( "Details" ),
                TXT( "List" ),

                TXT( "Small" ),
                TXT( "Medium" ),
                TXT( "Large" ),

                TXT( "Path (Native Format)" ),
                TXT( "Path (Internal Tools Format)" ),

                TXT( "Sync" ),
                TXT( "Check Out" ),
                TXT( "Revision History" ),

                TXT( "Open" ),
                TXT( "Preview" ),

                TXT( "Refresh" ),

                TXT( "New" ),
                TXT( "New Folder" ),
                TXT( "Cut" ),
                TXT( "Copy" ),
                TXT( "Paste" ),
                TXT( "Rename" ),
                TXT( "Delete" ),
                TXT( "Properties" ),

                TXT( "Select All\tCtrl+A" ),

                TXT( "Sort" ),
                TXT( "Name" ),
                TXT( "Type" ),

                TXT( "Folders" ),
                TXT( "Show in Revision Control" ),
                TXT( "Windows Explorer" ), 

                TXT( "Customize Settings..." ),
            };

            inline const tstring& Label( i32 id )
            {
                HELIUM_ASSERT( id > START );
                HELIUM_ASSERT( id < COUNT_PLUS_START );
                return s_Labels[id-START];
            }
        };
        typedef VaultMenu::MenuIDs VaultMenuID;

        ///////////////////////////////////////////////////////////////////////
        namespace VaultViewModes
        {
            enum VaultViewMode
            {
                Details,
                List,

                ThumbnailsSmall,
                ThumbnailsMedium,
                ThumbnailsLarge,

                ThumbnailsCustom,
            };
            static void VaultViewModeEnumerateEnum( Reflect::Enumeration* info )
            {
                info->AddElement( Details, TXT( "Details" ) );
                info->AddElement( List, TXT( "List" ) );

                info->AddElement( ThumbnailsSmall, TXT( "Small" ) );
                info->AddElement( ThumbnailsMedium, TXT( "Medium" ) );
                info->AddElement( ThumbnailsLarge, TXT( "Large" ) );

                info->AddElement( ThumbnailsCustom, TXT( "Custom" ) );
            }
        }
        typedef VaultViewModes::VaultViewMode VaultViewMode;

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