#pragma once

#include "VaultGenerated.h"
#include "VaultFrame.h"
#include "VaultMenuIDs.h"

#include "Application/Inspect/DragDrop/DropTarget.h"
#include "Application/Inspect/DragDrop/ReflectClipboardData.h"

namespace Helium
{
    namespace Editor
    {
        class SortTreeCtrl;

        ///////////////////////////////////////////////////////////////////////////////
        /// Class FoldersPanel
        ///////////////////////////////////////////////////////////////////////////////
        class FoldersPanel : public FoldersPanelGenerated 
        {
        public:
            FoldersPanel( VaultFrame* browserFrame );
            virtual ~FoldersPanel();

            wxTreeCtrl* GetTreeCtrl();
            SortTreeCtrl* GetSortTreeCtrl();
            void Unselect();

            void SetPath( const tstring& path );
            void GetPath( tstring& path ) const;

        protected:
            // Virtual event handlers, overide them in your derived class
            virtual void OnTreeBeginDrag( wxTreeEvent& event ) HELIUM_OVERRIDE;
            virtual void OnTreeItemDelete( wxTreeEvent& event ) HELIUM_OVERRIDE;
            virtual void OnTreeEndDrag( wxTreeEvent& event ) HELIUM_OVERRIDE;
            virtual void OnTreeItemMenu( wxTreeEvent& event ) HELIUM_OVERRIDE;
            virtual void OnTreeItemRightClick( wxTreeEvent& event ) HELIUM_OVERRIDE;

            // Drag-n-Drop Listeners
            wxDragResult DragOver( const Inspect::DragArgs& args );
            wxDragResult Drop( const Inspect::DragArgs& args );
            void DragLeave( Helium::Void );

        private:
            wxTreeItemId DragHitTest( SortTreeCtrl* treeCtrl, wxPoint point );

        private:
            VaultFrame* m_VaultFrame;

            // Drag-n-Drop
            wxTreeItemId  m_DragOverItem;
            bool          m_DragOriginatedHere;

            enum ContextMenuIDs
            {
                ID_Open = VaultMenu::Open,

                ID_CheckOut = VaultMenu::CheckOut,

                ID_CopyPathNative = VaultMenu::CopyPathNative,
                ID_CopyPath = VaultMenu::CopyPath,

                ID_ShowInFolders = VaultMenu::ShowInFolders,
                ID_ShowInPerforce = VaultMenu::ShowInPerforce,
                ID_ShowInWindows = VaultMenu::ShowInWindowsExplorer,

                ID_New = VaultMenu::New,
                ID_NewFolder = VaultMenu::NewFolder,
                ID_Cut = VaultMenu::Cut,
                ID_Copy = VaultMenu::Copy,
                ID_Paste = VaultMenu::Paste,
                ID_Rename = VaultMenu::Rename,
                ID_Delete = VaultMenu::Delete,
                ID_Properties = VaultMenu::Properties,
            };
        };
    }
}