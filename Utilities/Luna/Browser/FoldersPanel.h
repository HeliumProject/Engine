#pragma once

#include "BrowserGenerated.h"
#include "BrowserFrame.h"
#include "BrowserMenuIDs.h"

#include "Inspect/DropTarget.h"
#include "Inspect/ReflectClipboardData.h"

namespace UIToolKit
{
  class SortTreeCtrl;
}

namespace Luna
{
  ///////////////////////////////////////////////////////////////////////////////
  /// Class FoldersPanel
  ///////////////////////////////////////////////////////////////////////////////
  class FoldersPanel : public FoldersPanelGenerated 
  {
  public:
    FoldersPanel( BrowserFrame* browserFrame );
    virtual ~FoldersPanel();

    wxTreeCtrl* GetTreeCtrl();
    UIToolKit::SortTreeCtrl* GetSortTreeCtrl();
    void Unselect();

    void SetPath( const std::string& path );
    void GetPath( std::string& path ) const;

  protected:
    // Virtual event handlers, overide them in your derived class
    virtual void OnTreeBeginDrag( wxTreeEvent& event ) NOC_OVERRIDE;
    virtual void OnTreeItemDelete( wxTreeEvent& event ) NOC_OVERRIDE;
    virtual void OnTreeEndDrag( wxTreeEvent& event ) NOC_OVERRIDE;
    virtual void OnTreeItemMenu( wxTreeEvent& event ) NOC_OVERRIDE;
    virtual void OnTreeItemRightClick( wxTreeEvent& event ) NOC_OVERRIDE;

    // Drag-n-Drop Listeners
    wxDragResult DragOver( const Inspect::DragArgs& args );
    wxDragResult Drop( const Inspect::DragArgs& args );
    void DragLeave( Nocturnal::Void );

  private:
    wxTreeItemId DragHitTest( UIToolKit::SortTreeCtrl* treeCtrl, wxPoint point );

  private:
    BrowserFrame* m_BrowserFrame;

    // Drag-n-Drop
    wxTreeItemId  m_DragOverItem;
    bool          m_DragOriginatedHere;

    enum ContextMenuIDs
    {
      ID_Open = BrowserMenu::Open,
      
      ID_CheckOut = BrowserMenu::CheckOut,

      ID_CopyPathWindows = BrowserMenu::CopyPathWindows,
      ID_CopyPathClean = BrowserMenu::CopyPathClean,

      ID_ShowInFolders = BrowserMenu::ShowInFolders,
      ID_ShowInPerforce = BrowserMenu::ShowInPerforce,
      ID_ShowInWindows = BrowserMenu::ShowInWindowsExplorer,

      ID_New = BrowserMenu::New,
      ID_NewFolder = BrowserMenu::NewFolder,
      ID_Cut = BrowserMenu::Cut,
      ID_Copy = BrowserMenu::Copy,
      ID_Paste = BrowserMenu::Paste,
      ID_Rename = BrowserMenu::Rename,
      ID_Delete = BrowserMenu::Delete,
      ID_Properties = BrowserMenu::Properties,
    };
  };
}
