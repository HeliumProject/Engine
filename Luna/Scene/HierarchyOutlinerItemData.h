#pragma once

// Includes
#include "Luna/API.h"
#include "SceneOutlinerItemData.h"

namespace Luna
{
  // Forwards
  class HierarchyNode;

  /////////////////////////////////////////////////////////////////////////////
  // Wrapper for hierarchy data stored in tree items.  Derives from 
  // SceneOutlinerItemData and internally handles casting from Object to
  // Luna::HierarchyNode.  Used in the Hierarchy Outline to track hierarchy nodes
  // for each item in the tree (instead of just the Object base class).
  // 
  class HierarchyOutlinerItemData : public SceneOutlinerItemData
  {
  public:
    HierarchyOutlinerItemData( Luna::HierarchyNode* node );
    virtual ~HierarchyOutlinerItemData();

    Luna::HierarchyNode* GetHierarchyNode() const;
    void SetHierarchyNode( Luna::HierarchyNode* node );
  };
}
