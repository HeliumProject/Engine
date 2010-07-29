#include "Precompile.h"
#include "ParentCommand.h"
#include "Transform.h"
#include "Foundation/Reflect/Object.h"

using namespace Editor;

ParentCommand::ParentCommand(const HierarchyNodePtr& child, const HierarchyNodePtr& parent)
{
  m_Node = child;
  m_NextParent = parent;
  m_PreviousParent = child->GetParent();

  m_Node->SetParent( m_NextParent.Ptr() );

  Editor::Transform* transform = Reflect::ObjectCast< Editor::Transform >( m_Node );
  if ( transform )
  {
    Push( transform->ComputeObjectComponents() );
  }
}

void ParentCommand::Undo()
{
  Swap();

  __super::Undo();
}

void ParentCommand::Redo()
{
  Swap();

  __super::Redo();
}

void ParentCommand::Swap()
{
  // swap parents
  HierarchyNodePtr n = m_NextParent;
  m_NextParent = m_PreviousParent;
  m_PreviousParent = n;

  m_Node->SetParent( m_NextParent.Ptr() );
}