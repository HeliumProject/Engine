#pragma once

// Includes
#include "Editor/API.h"
#include "SceneOutlinerItemData.h"

namespace Editor
{
  // Forwards
  class HierarchyNode;

  /////////////////////////////////////////////////////////////////////////////
  // Wrapper for hierarchy data stored in tree items.  Derives from 
  // SceneOutlinerItemData and internally handles casting from Object to
  // Editor::HierarchyNode.  Used in the Hierarchy Outline to track hierarchy nodes
  // for each item in the tree (instead of just the Object base class).
  // 
  class HierarchyOutlinerItemData : public SceneOutlinerItemData
  {
  public:
    HierarchyOutlinerItemData( Editor::HierarchyNode* node );
    virtual ~HierarchyOutlinerItemData();

    Editor::HierarchyNode* GetHierarchyNode() const;
    void SetHierarchyNode( Editor::HierarchyNode* node );
  };
}
