#pragma once

#include "Editor/EditorGeneratedWrapper.h"
#include "Grid.h"

#include "Foundation/Event.h"
#include "Application/OrderedSet.h"

#include "EditorScene/Scene.h"
#include "EditorScene/Layer.h"
#include "EditorScene/SceneManager.h"

namespace Helium
{
    namespace Editor
    {
        class LayersPanel : public LayersPanelGenerated
        {
        private:

            // Helper class for tracking which layer is having its name changed.  This is
            // needed because we have to store the old name of the layer before the name
            // change takes place, otherwise we can't find the row that the newly renamed
            // layer corresponds to.
            class NameChangeInfo
            {
            public:
                Editor::Layer* m_Layer;
                std::string m_OldName;

            public:
                NameChangeInfo();
                virtual ~NameChangeInfo();
                void Clear();
            };

        public:
            LayersPanel( Editor::SceneManager* manager, wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
            virtual ~LayersPanel();

            bool AddLayer( Editor::Layer* layer );
            bool RemoveLayer( Editor::Layer* layer );
            void BeginBatch();
            void EndBatch();

            virtual bool  IsSelectionItemsLinked()
            {
                return false;
            }

            virtual bool  IsSelectionValid() const
            {
                return true;
            }

            virtual void  UnlinkSelectedElements()              {}
            virtual void  SelectLinkedElements()                {}
            virtual void  GenerateLayerName(Editor::Layer* layer) {}
            virtual void  CleanUpLayers()                       {}

        protected:
            Editor::SceneManager*     m_SceneManager;
            Editor::Scene*            m_Scene;
            Editor::M_LayerDumbPtr    m_Layers;
            Grid*                   m_Grid;
            NameChangeInfo          m_NameChangeInfo;

        protected:
            void ConnectSceneListeners();
            void DisconnectSceneListeners();
            void RemoveAllLayers();
            void UpdateToolBarButtons();
            void LayerSelectedItems( bool addToLayer );
            void DebugDumpSelection();
            void DeleteSelectedLayers();

            // 
            // UI callbacks
            // 
        private:
            void OnNewLayer( wxCommandEvent& event );
            void OnNewLayerFromSelection( wxCommandEvent& event );
            void OnDeleteLayer( wxCommandEvent& event );
            void OnAddSelectionToLayer( wxCommandEvent& event );
            void OnRemoveSelectionFromLayer( wxCommandEvent& event );
            void OnSelectLayerMembers( wxCommandEvent& event );
            void OnSelectLayer( wxCommandEvent& event );
            void OnCleanUpLayers( wxCommandEvent& event );

            // 
            // Event callbacks
            // 
        private:
            void SelectionChanged( const Editor::SelectionChangeArgs& args );
            void NameChanging( const Editor::SceneNodeChangeArgs& args );
            void NameChanged( const Editor::SceneNodeChangeArgs& args );
            void LayerVisibleChanged( const GridRowChangeArgs& args );
            void LayerSelectableChanged( const GridRowChangeArgs& args );
            void RowRenamed( const GridRowRenamedArgs& args );
            void CurrentSceneChanging( const Editor::SceneChangeArgs& args );
            void CurrentSceneChanged( const Editor::SceneChangeArgs& args );
            void NodeAdded( const Editor::NodeChangeArgs& args );
            void NodeRemoved( const Editor::NodeChangeArgs& args );
        };
    }
}