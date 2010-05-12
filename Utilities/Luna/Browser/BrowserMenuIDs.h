#pragma once

#include "Common/Types.h"
#include "Common/Assert.h"
#include "Reflect/Enumeration.h"
#include <sstream>

namespace Luna
{
  namespace BrowserMenu
  {
    ///////////////////////////////////////////////////////////////////////////
    enum MenuIDs
    {
      START = wxID_HIGHEST, // Do not use: must be first

      FoldersPanel,
      CollectionsPanel,
      HelpPanel,
      
      ViewSmall,
      ViewMedium,
      ViewLarge,
      ViewDetails,

      AdvancedSearch,
      AdvancedSearchGo,
      AdvancedSearchCancel,

      CopyPathWindows,
      CopyPathClean,

      CopyFileIDHex,
      CopyFileIDDecimal,

      Sync,
      CheckOut,
      History,

      Open,
      Preview,
      ViewOnTarget,
      ViewOnTargetWithOptions,

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

      ShowCollection,

      NewCollectionFromSelection,
      NewDepedencyCollectionFromSelection,
      NewUsageCollectionFromSelection,

      NewCollection,
      NewDependencyCollection,
      NewUsageCollection,

      OpenCollection,
      CloseCollection,

      RenameCollection,
      DeleteCollection,

      ImportCollection,
      ImportIntoCollection,
      ExportCollection,

      AddToCollection,
      RemoveFromCollection,

      Preferences,

      COUNT_PLUS_START // Do not use: must be last
    };
    static const u32 s_Count = COUNT_PLUS_START - START;

    ///////////////////////////////////////////////////////////////////////////
    static const std::string s_Labels[s_Count] = 
    {
      "INVALID", // placeholder for START

      "Folders",
      "Collections",
      "Help",

      "Small",
      "Medium",
      "Large",
      "Details",

      "Advanced Search",
      "Go",
      "Cancel",

      "Path (Windows Format)",
      "Path (Insomniac Format)",

      "File ID (Hex)",
      "File ID (Decimal)",

      "Sync",
      "Check Out",
      "Revision History",

      "Open",
      "Preview",
      "View on PS3",
      "View on PS3 (+options)",

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
      "Show in Perforce",
      "Windows Explorer", 

      "Show",

      "Static Collection",
      "Dependency Collection",
      "Usage Collection",

      "Static Collection",
      "Asset Dependencies Collection",
      "Asset Usage Collection",

      "Open",
      "Close",

      "Rename",
      "Delete",

      "Import...",
      "Import into...",
      "Export...",

      "Add Selected Assets",
      "Remove Selected Assets",

      "Customize Preferences...",
    };
    
    inline const std::string& Label( i32 id )
    {
      NOC_ASSERT( id > START );
      NOC_ASSERT( id < COUNT_PLUS_START );
      return s_Labels[id-START];
    }
  };
  typedef BrowserMenu::MenuIDs BrowserMenuID;

  ///////////////////////////////////////////////////////////////////////////
  namespace ThumbnailSizes
  {
    enum ThumbnailSize
    {
      Small = 64,
      Medium = 128,
      Large = 256
    };
    inline std::string Label( ThumbnailSize size )
    {
      std::stringstream str;
      str << "(" << size << "x" << size << ")";
      return str.str();
    }
  }
  typedef ThumbnailSizes::ThumbnailSize ThumbnailSize;

  ///////////////////////////////////////////////////////////////////////////
  namespace ViewOptionIDs
  {
    enum ViewOptionID
    {
      Small = BrowserMenu::ViewSmall,
      Medium = BrowserMenu::ViewMedium,
      Large = BrowserMenu::ViewLarge,
      Custom = -1,
    };

    static void ViewOptionIDEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(Small, "Small");
      info->AddElement(Medium, "Medium");
      info->AddElement(Large, "Large");
      info->AddElement(Custom, "Custom");
    }
  }
  typedef ViewOptionIDs::ViewOptionID ViewOptionID;
}
