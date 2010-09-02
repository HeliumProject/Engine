#pragma once

#include "Platform/Types.h"

#include "Core/Scene/Scene.h"
#include "Core/Scene/SceneManager.h"
#include "Core/Scene/Selection.h"
#include "Core/Scene/Object.h"

#include "Editor/API.h"
#include "Editor/SceneOutlinerState.h"
#include "Editor/Controls/Tree/SortTreeCtrl.h"

namespace Helium
{
    namespace Editor
    {
        /////////////////////////////////////////////////////////////////////////////
        // Class for attaching Objects to items displayed in a tree control.
        // 
        class SceneOutlinerItemData : public wxTreeItemData
        {
        protected:
            Core::Object* m_Object;
            tstring       m_ItemText; 
            int           m_CachedCount; 
            bool          m_Countable; 

        public:
            SceneOutlinerItemData( Core::Object* object )
                : m_Object( object )
                , m_CachedCount( 0 )
            {
            }

            ~SceneOutlinerItemData()
            {
            }

            Core::Object* GetObject() const
            {
                return m_Object;
            }

            void SetObject( Core::Object* object )
            {
                m_Object = object;
            }

            void SetItemText( const tstring& text )
            {
                m_ItemText = text; 
            }

            const tstring& GetItemText()
            {
                return m_ItemText;
            }

            int GetCachedCount()
            {
                return m_CachedCount; 
            }
            void SetCachedCount( int count )
            {
                m_CachedCount = count; 
            }

            bool GetCountable()
            {
                return m_Countable;
            }
            void SetCountable( bool countable )
            {
                m_Countable = countable;
            }
        };

        /////////////////////////////////////////////////////////////////////////////
        // Abstract base class for GUIs that display trees of scene nodes.
        // 
        class SceneOutliner HELIUM_ABSTRACT : public wxEvtHandler
        {
        protected:
            // Typedefs
            typedef std::map< Core::Object*, wxTreeItemId > M_TreeItems;

        protected:
            // Member variables
            Core::SceneManager* m_SceneManager;
            Core::Scene* m_CurrentScene;
            SortTreeCtrl* m_TreeCtrl;
            M_TreeItems m_Items;
            SceneOutlinerState m_StateInfo;
            bool m_IgnoreSelectionChange;
            bool m_DisplayCounts; 

        public:
            // Functions
            SceneOutliner( Core::SceneManager* sceneManager );
            virtual ~SceneOutliner();
            SortTreeCtrl* InitTreeCtrl( wxWindow* parent, wxWindowID id );
            void SaveState( SceneOutlinerState& state );
            void RestoreState( const SceneOutlinerState& state );
            void DisableSorting();
            void EnableSorting();
            virtual void Sort( const wxTreeItemId& root = SortTreeCtrl::InvalidItemId );

        protected:
            SceneOutlinerItemData* GetTreeItemData( const wxTreeItemId& item );
            void UpdateCurrentScene( Core::Scene* scene );
            void DoRestoreState();

        protected:
            // Derived classes can optionally HELIUM_OVERRIDE these functions
            virtual void Clear();
            virtual wxTreeItemId AddItem( const wxTreeItemId& parent, const tstring& name, i32 image, SceneOutlinerItemData* data, bool isSelected, bool countable = true); 
            virtual void DeleteItem( Core::Object* object );
            void UpdateItemCounts( const wxTreeItemId& node, int delta );
            void UpdateItemVisibility( const wxTreeItemId& item, bool visible );

            virtual void ConnectSceneListeners();
            virtual void DisconnectSceneListeners();
            virtual void CurrentSceneChanging( Core::Scene* newScene );
            virtual void CurrentSceneChanged( Core::Scene* oldScene );

        protected:
            // Application event callbacks
            virtual void CurrentSceneChanged( const Core::SceneChangeArgs& args );
            virtual void SelectionChanged( const Core::SelectionChangeArgs& args );
            virtual void SceneNodeNameChanged( const Core::SceneNodeChangeArgs& args );
            void SceneNodeVisibilityChanged( const Core::SceneNodeChangeArgs& args );

        protected:
            // Derived classes must HELIUM_OVERRIDE these functions
            virtual SortTreeCtrl* CreateTreeCtrl( wxWindow* parent, wxWindowID id ) = 0;

        private:
            // Tree event callbacks
            virtual void OnEndLabelEdit( wxTreeEvent& args );
            virtual void OnSelectionChanging( wxTreeEvent& args );
            virtual void OnSelectionChanged( wxTreeEvent& args );
            virtual void OnExpanded( wxTreeEvent& args );
            virtual void OnCollapsed( wxTreeEvent& args );
            virtual void OnDeleted( wxTreeEvent& args );
            virtual void OnChar( wxKeyEvent& args );

        private:
            // Connect/disconnect dynamic event table for GUI callbacks
            void ConnectDynamicEventTable();
            void DisconnectDynamicEventTable();
        };
    }
}
