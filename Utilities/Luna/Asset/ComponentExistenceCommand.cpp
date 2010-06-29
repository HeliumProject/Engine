#include "Precompile.h"
#include "ComponentExistenceCommand.h"
#include "AssetClass.h"
#include "AssetReferenceNode.h"
#include "ComponentWrapper.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ComponentExistenceCommand::ComponentExistenceCommand( Undo::ExistenceAction action, Luna::AssetClass* asset, Luna::ComponentWrapper* component )
: Undo::ExistenceCommand( action, new Undo::MemberFunctionConstRefReturn< Luna::AssetClass, Luna::ComponentWrapperPtr, bool >( asset, component, &Luna::AssetClass::AddComponent ), new Undo::MemberFunctionConstRefReturn< Luna::AssetClass, Luna::ComponentWrapperPtr, bool >( asset, component, &Luna::AssetClass::RemoveComponent ), false )
, m_Action( action )
, m_Asset( asset )
, m_Component( component )
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
        m_References->Push( reference->GetAddComponentCommand( m_Component ) );
      }
      break;
    }
    
  case Undo::ExistenceActions::Remove:
    {
      for each ( Luna::AssetReferenceNode* reference in m_Asset->GetAssetReferenceNodes() )
      {
        m_References->Push( reference->GetRemoveComponentCommand( m_Component ) );
      }
      break;
    }
  }

  // Automatically perform the add/remove operation.
  Redo();
}
    
///////////////////////////////////////////////////////////////////////////////
// Adds or removes the component and updates all references to the asset class.
// 
void ComponentExistenceCommand::Undo()
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
// Adds or removes the component and updates all references to the asset class.
// 
void ComponentExistenceCommand::Redo()
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
