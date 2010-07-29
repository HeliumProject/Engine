#include "Precompile.h"
#include "HierarchyOutlinerItemData.h"
#include "HierarchyNode.h"

// Using
using namespace Editor;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
HierarchyOutlinerItemData::HierarchyOutlinerItemData( Editor::HierarchyNode* node )
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
Editor::HierarchyNode* HierarchyOutlinerItemData::GetHierarchyNode() const
{
  return Reflect::AssertCast< Editor::HierarchyNode >( GetObject() );
}

///////////////////////////////////////////////////////////////////////////////
// Sets the hierarchy node associated with this data.
// 
void HierarchyOutlinerItemData::SetHierarchyNode( Editor::HierarchyNode* node )
{
  SetObject( node );
}
