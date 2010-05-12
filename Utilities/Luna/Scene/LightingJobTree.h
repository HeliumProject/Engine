#pragma once

#include <map>

#include "Common/Memory/SmartPtr.h"
#include "Core/Selectable.h"
#include "UIToolKit/SortTreeCtrl.h"
#include "LightingJob.h"

// Forwards
namespace Undo
{
  class Command;
  typedef Nocturnal::SmartPtr< Command > CommandPtr;
}

namespace Luna
{
  // More forwards
  class LightingJob;
  class Object;
  class Scene;
  class SceneEditor;
  class SceneManager;
  struct SceneNodeChangeArgs;
  struct SceneChangeArgs;
  struct LoadArgs;
  struct NodeChangeArgs;

  typedef Nocturnal::OrderedSet< wxTreeItemIdValue > OS_TreeItemIds;

  /////////////////////////////////////////////////////////////////////////////
  // Tree control displaying lighting jobs in the lighting UI.
  // 
  class LightingJobTree : public UIToolKit::SortTreeCtrl
  {
  private:
    typedef std::map< Luna::LightingJob*, wxTreeItemId > M_JobsToItems;

  private:
    SceneEditor* m_SceneEditor;
    Luna::SceneManager* m_SceneManager;
    M_JobsToItems m_JobTreeItems;
    OS_TreeItemIds m_SelectedItems;

  public:
    LightingJobTree( wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTR_HAS_BUTTONS, const wxValidator& validator = wxDefaultValidator, const wxString& name = "LightingJobTree" );
    virtual ~LightingJobTree();
    void Init( SceneEditor* sceneEditor );
    Undo::CommandPtr RemoveJobMembers( const OS_SelectableDumbPtr& itemsToRemove );
    void SelectLightingJob( Luna::LightingJob* job );
    const OS_TreeItemIds& GetSelectedItems() const;
    void SelectItems( const OS_SelectableDumbPtr& sceneSelection );
    virtual void DeleteChildren(const wxTreeItemId& item) NOC_OVERRIDE;
    virtual void DeleteAllItems() NOC_OVERRIDE;


  private:
    void AddJob( Luna::LightingJob* job );
    void RemoveJob( Luna::LightingJob* job );
    void RemoveJobs( Luna::Scene* scene );
    void RemoveChildrenFromSelection( const wxTreeItemId& item );

  private:
    void SceneAdded( const SceneChangeArgs& args );
    void SceneRemoving( const SceneChangeArgs& args );
    void SceneLoadFinished( const LoadArgs& args );
    void NodeCreated( const NodeChangeArgs& args );
    void NodeDeleted( const NodeChangeArgs& args );
    void NodeNameChanged( const SceneNodeChangeArgs& args );

  private:
    void OnSelectionChanging( wxTreeEvent& args );
    void OnDeleteItem( wxTreeEvent& args );
    void OnBeginLabelEdit( wxTreeEvent& args );
    void OnEndLabelEdit( wxTreeEvent& args );
    void OnContextMenu( wxTreeEvent& args );
  };
}
