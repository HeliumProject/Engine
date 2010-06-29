#include "Precompile.h"
#include "AssetOutlineItemData.h"

#include "AssetNode.h"
#include "AssetOutliner.h"

// Using
using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
AssetOutlineItemData::AssetOutlineItemData( AssetOutliner* outliner, Luna::AssetNode* assetNode )
: m_Outliner( outliner )
, m_AssetNode( assetNode )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
AssetOutlineItemData::~AssetOutlineItemData()
{
  // Let the outliner know that we are being deleted.
  m_Outliner->ItemDeleted( this );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the asset node that this item represents.
// 
Luna::AssetNode* AssetOutlineItemData::GetAssetNode() const
{
  return m_AssetNode;
}
