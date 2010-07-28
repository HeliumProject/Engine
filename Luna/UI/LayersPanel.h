#pragma once

#include "LunaGenerated.h"
#include "Grid.h"

#include "Foundation/Automation/Event.h"
#include "Foundation/Container/OrderedSet.h"

#include "Scene/Scene.h"
#include "Scene/Layer.h"
#include "Scene/SceneManager.h"

namespace Luna
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
            Layer* m_Layer;
            tstring m_OldName;

        public:
            NameChangeInfo();
            virtual ~NameChangeInfo();
            void Clear();
        };

    public:
        LayersPanel( SceneManager* manager, wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
        virtual ~LayersPanel();

        bool AddLayer( Luna::Layer* layer );
        bool RemoveLayer( Luna::Layer* layer );
        void BeginBatch();
        void EndBatch();

        virtual bool  IsSelectionItemsLinked()                { return false;             }
        virtual bool  IsSelectableValid(Selectable* sl) const { return sl ? true : false; }
        virtual bool  IsSelectionValid() const                { return true;              }
        virtual void  UnlinkSelectedElements()        {}
        virtual void  SelectLinkedElements()          {}
        virtual void  GenerateLayerName(Layer* layer) {}
        virtual void  CleanUpLayers()                 {}

    protected:
        SceneManager*   m_SceneManager;
        Scene*          m_Scene;
        Grid*           m_Grid;
        M_LayerDumbPtr  m_Layers;
        NameChangeInfo  m_NameChangeInfo;

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
        void SelectionChanged( const OS_SelectableDumbPtr& selection );
        void NameChanging( const SceneNodeChangeArgs& args );
        void NameChanged( const SceneNodeChangeArgs& args );
        void LayerVisibleChanged( const GridRowChangeArgs& args );
        void LayerSelectableChanged( const GridRowChangeArgs& args );
        void RowRenamed( const GridRowRenamedArgs& args );
        void CurrentSceneChanging( const SceneChangeArgs& args );
        void CurrentSceneChanged( const SceneChangeArgs& args );
        void NodeAdded( const NodeChangeArgs& args );
        void SceneNodeRemoved( const NodeChangeArgs& args );


    };
}