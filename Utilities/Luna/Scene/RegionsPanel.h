#pragma once

#include "RegionsGenerated.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Region.h"
#include "Zone.h"

#include "Editor/ContextMenu.h"
#include "Editor/ContextMenuGenerator.h"
#include "Core/Selection.h"

namespace Luna
{
  class Scene; 
  class SceneEditor; 
  class SceneManager; 

  class RegionsPanel : public RegionsPanelBase
  {
  public: 
    RegionsPanel( SceneEditor* editor, Luna::SceneManager* manager, wxWindow* parent); 
    ~RegionsPanel(); 

  private: 
    // scene manager callbacks
    void SceneAdded( const SceneChangeArgs& args );
    void SceneRemoved( const SceneChangeArgs& args );
    void CurrentSceneChanged( const SceneChangeArgs& args );
    void NodeAdded( const NodeChangeArgs& args );
    void NodeRemoved( const NodeChangeArgs& args );
    void SceneLoadStarted( const LoadArgs& args );
    void SceneLoadFinished( const LoadArgs& args );

    bool SelectionChanging( const OS_SelectableDumbPtr& selection); 
    void SelectionChanged( const OS_SelectableDumbPtr& selection );

    // wx callbacks
		virtual void OnBeginLabelEdit( wxTreeEvent& event );
		virtual void OnEndLabelEdit( wxTreeEvent& event );
		virtual void OnTreeItemCollapsing( wxTreeEvent& event );
		virtual void OnTreeItemMenu( wxTreeEvent& event );
		virtual void OnTreeSelChanged( wxTreeEvent& event );
    virtual void OnTreeKeyDown( wxTreeEvent& event );

    // context menu callbacks
    void ContextCreateRegion(const ContextMenuArgsPtr&); 
    void ContextRegionAddRemoveZones(const ContextMenuArgsPtr&); 
    void ContextRename(const ContextMenuArgsPtr&); 
    void ContextDeleteRegion(const ContextMenuArgsPtr&); 
    void ContextZoneRemoveFromRegion(const ContextMenuArgsPtr&); 
    void ContextZoneChangeRegions(const ContextMenuArgsPtr&); 

    // node changed callbacks
    void NodeChanged( const ObjectChangeArgs& args ); 
    void NodeNameChanged( const SceneNodeChangeArgs& args ); 

    void InitTree();
    void BuildTree(); 
    void TeardownTree(); 
    void CreateZoneNodes(Luna::Region* region, wxTreeItemId regionNode, const S_ZoneDumbPtr& zones);

    wxTreeItemId BuildRegionSubtree(const LRegionPtr& regionWrapper); 
    bool         RemoveRegionSubtree(Luna::Region* region, bool* expanded = NULL); 

    void InitializeRegion(Luna::Region* region); 
    void CleanupRegion(Luna::Region* region); 

    void InitializeZone(Zone* zone); 
    void CleanupZone(Zone* zone); 

    wxTreeItemId AddZoneToRoot(const ZonePtr& zoneWrapper); 
    
    struct TreeData : public wxTreeItemData
    { 
    public: 
      TreeData(Object* obj)
        : m_Object(obj)
      {
      }

      Object* m_Object; 
    }; 

    typedef std::map<Luna::Region*, wxTreeItemId>      M_RegionToId; 
    typedef std::map<Zone*, wxTreeItemId>        M_ZoneToId; 
    typedef std::map<Nocturnal::TUID, ZonePtr>    M_TuidToZone; 

  private: 
    SceneEditor*       m_Editor; 
    Luna::SceneManager*      m_SceneManager; 
    M_RegionToId        m_RegionTreeMap;
    M_ZoneToId          m_ZoneTreeMap; 
    M_TuidToZone        m_ZoneMap; 
    wxTreeItemId        m_RegionRoot;
    wxTreeItemId        m_ZoneRoot; 
    Luna::Scene*             m_Scene; 

    ContextMenuItemSet m_RegionContextMenu; 
    ContextMenuItemSet m_PerRegionContextMenu; 
    ContextMenuItemSet m_PerOwnedZoneContextMenu; 
    ContextMenuItemSet m_PerZoneContextMenu; 

    i32                 m_RegionIcon; 
    i32                 m_ZoneIcon; 

    bool                m_LoadInProgress; 
    bool                m_IgnoreSelectionChange; 
  }; 



}