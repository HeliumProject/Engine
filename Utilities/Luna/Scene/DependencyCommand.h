#pragma once

#include "Undo/Command.h"

namespace Luna
{
  // Forwards and typedefs
  class SceneNode;
  typedef Nocturnal::SmartPtr< Luna::SceneNode > SceneNodePtr;

  /////////////////////////////////////////////////////////////////////////////
  // Command for making or breaking connections between ancestor and descendant
  // dependency nodes.
  // 
  class DependencyCommand : public Undo::Command
  {
  public:
    // Is this command connecting or disconnecting the nodes?
    enum DependencyAction
    {
      Connect,
      Disconnect
    };

  private:
    DependencyAction m_Action;
    SceneNodePtr m_Ancestor;
    SceneNodePtr m_Descendant;

  public:
    DependencyCommand( DependencyAction action, const SceneNodePtr& ancestor, const SceneNodePtr& descendant );
    virtual ~DependencyCommand();
    virtual void Undo() NOC_OVERRIDE;
    virtual void Redo() NOC_OVERRIDE;
  };
}
