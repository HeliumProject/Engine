#include "Precompile.h"
#include "AddAssetNodeCommand.h"
#include "AssetNode.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor 
// 
AddAssetNodeCommand::AddAssetNodeCommand( Luna::AssetNode* parent, Luna::AssetNode* child, Luna::AssetNode* beforeSibling, bool redo )
: m_Parent( parent )
, m_Child( child )
, m_BeforeSibling( beforeSibling )
{
  NOC_ASSERT( m_Parent );
  NOC_ASSERT( m_Child );

  if ( redo )
  {
    Redo();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Removes the node from the hierarchy.
// 
void AddAssetNodeCommand::Undo()
{
  // Store the location of the node in the hierachy so that it can restored properly.
  m_BeforeSibling = NULL;
  OS_AssetNodeSmartPtr::Iterator sibling = m_Parent->GetChildren().FindNextSibling( m_Child );
  if ( sibling != m_Parent->GetChildren().End() )
  {
    m_BeforeSibling = *sibling;
  }

  // Remove the node.
  m_Parent->RemoveChild( m_Child );
}

///////////////////////////////////////////////////////////////////////////////
// Adds the node to the hierarchy.
// 
void AddAssetNodeCommand::Redo()
{
  m_Parent->AddChild( m_Child, m_BeforeSibling );
}
