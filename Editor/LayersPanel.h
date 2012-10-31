#pragma once

#include "Editor/EditorGenerated.h"
#include "Grid.h"

#include "Foundation/Event.h"
#include "Foundation/OrderedSet.h"

#include "SceneGraph/Scene.h"
#include "SceneGraph/Layer.h"
#include "SceneGraph/SceneManager.h"

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
                SceneGraph::Layer* m_Layer;
                tstring m_OldName;

            public:
                NameChangeInfo();
                virtual ~NameChangeInfo();
                void Clear();
            };

        public:
            LayersPanel( SceneGraph::SceneManager* manager, wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
            virtual ~LayersPanel();

            bool AddLayer( SceneGraph::Layer* layer );
            bool RemoveLayer( SceneGraph::Layer* layer );
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
            virtual void  GenerateLayerName(SceneGraph::Layer* layer) {}
            virtual void  CleanUpLayers()                       {}

        protected:
            SceneGraph::SceneManager*     m_SceneManager;
            SceneGraph::Scene*            m_Scene;
            SceneGraph::M_LayerDumbPtr    m_Layers;
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
            void SelectionChanged( const SceneGraph::SelectionChangeArgs& args );
            void NameChanging( const SceneGraph::SceneNodeChangeArgs& args );
            void NameChanged( const SceneGraph::SceneNodeChangeArgs& args );
            void LayerVisibleChanged( const GridRowChangeArgs& args );
            void LayerSelectableChanged( const GridRowChangeArgs& args );
            void RowRenamed( const GridRowRenamedArgs& args );
            void CurrentSceneChanging( const SceneGraph::SceneChangeArgs& args );
            void CurrentSceneChanged( const SceneGraph::SceneChangeArgs& args );
            void NodeAdded( const SceneGraph::NodeChangeArgs& args );
            void NodeRemoved( const SceneGraph::NodeChangeArgs& args );
        };
    }
}