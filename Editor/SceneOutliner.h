#pragma once

#include "Platform/Types.h"

#include "Core/SceneGraph/Scene.h"
#include "Core/SceneGraph/SceneManager.h"
#include "Core/SceneGraph/Selection.h"
#include "Foundation/Reflect/Object.h"

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
            Reflect::Object* m_Object;
            tstring       m_ItemText; 
            int           m_CachedCount; 
            bool          m_Countable; 

        public:
            SceneOutlinerItemData( Reflect::Object* object )
                : m_Object( object )
                , m_CachedCount( 0 )
            {
            }

            ~SceneOutlinerItemData()
            {
            }

            Reflect::Object* GetObject() const
            {
                return m_Object;
            }

            void SetObject( Reflect::Object* object )
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
            typedef std::map< Reflect::Object*, wxTreeItemId > M_TreeItems;

        protected:
            // Member variables
            SceneGraph::SceneManager* m_SceneManager;
            SceneGraph::Scene* m_CurrentScene;
            SortTreeCtrl* m_TreeCtrl;
            M_TreeItems m_Items;
            SceneOutlinerState m_StateInfo;
            bool m_IgnoreSelectionChange;
            bool m_DisplayCounts; 

        public:
            // Functions
            SceneOutliner( SceneGraph::SceneManager* sceneManager );
            virtual ~SceneOutliner();
            SortTreeCtrl* InitTreeCtrl( wxWindow* parent, wxWindowID id );
            void SaveState( SceneOutlinerState& state );
            void RestoreState( const SceneOutlinerState& state );
            void DisableSorting();
            void EnableSorting();
            virtual void Sort( const wxTreeItemId& root = SortTreeCtrl::InvalidItemId );

        protected:
            SceneOutlinerItemData* GetTreeItemData( const wxTreeItemId& item );
            void UpdateCurrentScene( SceneGraph::Scene* scene );
            void DoRestoreState();

        protected:
            // Derived classes can optionally HELIUM_OVERRIDE these functions
            virtual void Clear();
            virtual wxTreeItemId AddItem( const wxTreeItemId& parent, const tstring& name, i32 image, SceneOutlinerItemData* data, bool isSelected, bool countable = true); 
            virtual void DeleteItem( Reflect::Object* object );
            void UpdateItemCounts( const wxTreeItemId& node, int delta );
            void UpdateItemVisibility( const wxTreeItemId& item, bool visible );

            virtual void ConnectSceneListeners();
            virtual void DisconnectSceneListeners();
            virtual void CurrentSceneChanging( SceneGraph::Scene* newScene );
            virtual void CurrentSceneChanged( SceneGraph::Scene* oldScene );

        protected:
            // Application event callbacks
            virtual void CurrentSceneChanged( const SceneGraph::SceneChangeArgs& args );
            virtual void SelectionChanged( const SceneGraph::SelectionChangeArgs& args );
            virtual void SceneNodeNameChanged( const SceneGraph::SceneNodeChangeArgs& args );
            void SceneNodeVisibilityChanged( const SceneGraph::SceneNodeChangeArgs& args );

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
