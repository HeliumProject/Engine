#include "Precompile.h"
#include "RemoveAssetNodeCommand.h"
#include "AssetNode.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
RemoveAssetNodeCommand::RemoveAssetNodeCommand( Luna::AssetNode* child, bool redo )
: m_Parent( child->GetParent() )
, m_Child( child )
, m_BeforeSibling( NULL )
{
  NOC_ASSERT( m_Parent );
  if ( redo )
  {
    Redo();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Adds the node back to the proper location within the hierarchy.
// 
void RemoveAssetNodeCommand::Undo()
{
  m_Parent->AddChild( m_Child, m_BeforeSibling );
}

///////////////////////////////////////////////////////////////////////////////
// Removes the node from its position in the hierarchy.
// 
void RemoveAssetNodeCommand::Redo()
{
  // Store the sibling that this node is before so that it can be put back in
  // the right place.
  m_BeforeSibling = NULL;
  OS_AssetNodeSmartPtr::Iterator sibling = m_Parent->GetChildren().FindNextSibling( m_Child );
  if ( sibling != m_Parent->GetChildren().End() )
  {
    m_BeforeSibling = *sibling;
  }

  // Remove the node.
  m_Parent->RemoveChild( m_Child );
}
