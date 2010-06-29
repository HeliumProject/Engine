#pragma once

#include "Platform/Types.h"

namespace Luna
{
  // Forwards
  class AssetNode;
  class AssetOutliner;

  /////////////////////////////////////////////////////////////////////////////
  // Base class for all data that is stored in individual items of the
  // AssetOutliner.  Stores a pointer to the Luna::AssetNode that this data is
  // associated with.
  // 
  class AssetOutlineItemData : public wxTreeItemData
  {
  protected:
    AssetOutliner* m_Outliner;
    Luna::AssetNode* m_AssetNode;

  public:
    AssetOutlineItemData( AssetOutliner* outliner, Luna::AssetNode* assetNode );
    virtual ~AssetOutlineItemData();

    Luna::AssetNode* GetAssetNode() const;
  };
}
