#pragma once

#include <map>
#include <vector>
#include "Foundation/Automation/Event.h"
#include "Foundation/Container/OrderedSet.h"
#include "Content/LayerTypes.h"
namespace Luna
{
  // Forwards & typdefs (some are duplicates so that additonal headers are not needed)
  class Grid;
  class Scene;
  class SceneManager;

  struct GridRowChangeArgs;
  struct GridRowRenamedArgs;
  struct NodeChangeArgs;
  struct SceneChangeArgs;

  class Layer;
  typedef std::map< std::string, Luna::Layer* > M_LayerDumbPtr;

  class Selectable;
  typedef Nocturnal::OrderedSet<Selectable*> OS_SelectableDumbPtr;

  struct SceneNodeChangeArgs;
  typedef Nocturnal::Signature< void, const SceneNodeChangeArgs& > SceneNodeChangeSignature;

  /////////////////////////////////////////////////////////////////////////////
  // Class that manages the UI for changing the selectability/visibility of
  // layers.  Creates a panel that contains a toolbar and a grid for managing
  // the layers.  This panel can be added to the main frame of an application.
  // 
  class LayerGrid : public wxEvtHandler, public Nocturnal::RefCountBase<LayerGrid>
  {
  public:
    // Unique IDs for each command that can be triggered from the toolbar
    // It shouldn't matter if these overlap with other IDs in the application.
    enum CommandIDs
    {
      ID_NewLayer = wxID_HIGHEST + 1,
      ID_NewLayerFromSelection,
      ID_DeleteLayer,
      ID_AddSelectionToLayer,
      ID_RemoveSelectionFromLayer,
      ID_SelectLayerMembers,
      ID_SelectLayer,
      ID_CleanUpLayers,
      ID_COUNT
    };

  private:

    // Helper class for tracking which layer is having its name changed.  This is
    // needed because we have to store the old name of the layer before the name
    // change takes place, otherwise we can't find the row that the newly renamed
    // layer corresponds to.
    class NameChangeInfo
    {
    public:
      Luna::Layer* m_Layer;
      std::string m_OldName;

    public:
      NameChangeInfo();
      virtual ~NameChangeInfo();
      void Clear();
    };

  protected:
    Luna::SceneManager* m_SceneManager;
    Luna::Scene*        m_Scene;
    wxPanel*            m_Panel;
    Grid*               m_Grid;
    wxToolBar*          m_ToolBar;
    M_LayerDumbPtr      m_Layers;
    NameChangeInfo      m_NameChangeInfo;
    u32                 m_LayerType;

  public:
   LayerGrid( wxWindow* parent, Luna::SceneManager* sceneManager, u32 lType);
    virtual ~LayerGrid();
    wxPanel* GetPanel() const;
    wxToolBar*  GetToolBar()  const;
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

  private:
    // Event table
    DECLARE_EVENT_TABLE();
  };

  typedef Nocturnal::SmartPtr< LayerGrid >  LayerGridPtr;
  typedef std::vector< LayerGridPtr >       V_LayerGrid;
}
