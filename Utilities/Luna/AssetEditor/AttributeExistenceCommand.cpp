#include "Precompile.h"
#include "AttributeExistenceCommand.h"
#include "AssetClass.h"
#include "AssetReferenceNode.h"
#include "AttributeWrapper.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
AttributeExistenceCommand::AttributeExistenceCommand( Undo::ExistenceAction action, Luna::AssetClass* asset, Luna::AttributeWrapper* attribute )
: Undo::ExistenceCommand( action, new Undo::MemberFunctionConstRefReturn< Luna::AssetClass, Luna::AttributeWrapperPtr, bool >( asset, attribute, &Luna::AssetClass::AddAttribute ), new Undo::MemberFunctionConstRefReturn< Luna::AssetClass, Luna::AttributeWrapperPtr, bool >( asset, attribute, &Luna::AssetClass::RemoveAttribute ), false )
, m_Action( action )
, m_Asset( asset )
, m_Attribute( attribute )
, m_References( new Undo::BatchCommand() )
{
  // Build a batch command of each LAssetNodeReferenceNode that is referencing the
  // Luna::AssetClass.  This batch command will be used to update the hierachy during
  // undo/redo.
  switch ( m_Action )
  {
  case Undo::ExistenceActions::Add:
    {
      for each ( Luna::AssetReferenceNode* reference in m_Asset->GetAssetReferenceNodes() )
      {
        m_References->Push( reference->GetAddAttributeCommand( m_Attribute ) );
      }
      break;
    }
    
  case Undo::ExistenceActions::Remove:
    {
      for each ( Luna::AssetReferenceNode* reference in m_Asset->GetAssetReferenceNodes() )
      {
        m_References->Push( reference->GetRemoveAttributeCommand( m_Attribute ) );
      }
      break;
    }
  }

  // Automatically perform the add/remove operation.
  Redo();
}
    
///////////////////////////////////////////////////////////////////////////////
// Adds or removes the attribute and updates all references to the asset class.
// 
void AttributeExistenceCommand::Undo()
{
  switch ( m_Action )
  {
  case Undo::ExistenceActions::Add:
    m_References->Undo();
    break;
  case Undo::ExistenceActions::Remove:
    m_References->Undo();
    break;
  }

  __super::Undo();
}
    
///////////////////////////////////////////////////////////////////////////////
// Adds or removes the attribute and updates all references to the asset class.
// 
void AttributeExistenceCommand::Redo()
{
  switch ( m_Action )
  {
  case Undo::ExistenceActions::Add:
    m_References->Redo();
    break;
    
  case Undo::ExistenceActions::Remove:
    m_References->Redo();
    break;
  }

  __super::Redo();
}
