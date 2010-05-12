#include "Precompile.h"
#include "HierarchyOutlinerItemData.h"
#include "HierarchyNode.h"

// Using
using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
HierarchyOutlinerItemData::HierarchyOutlinerItemData( Luna::HierarchyNode* node )
: SceneOutlinerItemData( node )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
HierarchyOutlinerItemData::~HierarchyOutlinerItemData()
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns the hierarhcy node that goes with this data.
// 
Luna::HierarchyNode* HierarchyOutlinerItemData::GetHierarchyNode() const
{
  return Reflect::AssertCast< Luna::HierarchyNode >( GetObject() );
}

///////////////////////////////////////////////////////////////////////////////
// Sets the hierarchy node associated with this data.
// 
void HierarchyOutlinerItemData::SetHierarchyNode( Luna::HierarchyNode* node )
{
  SetObject( node );
}
