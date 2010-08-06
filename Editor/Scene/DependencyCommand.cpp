#include "Precompile.h"
#include "DependencyCommand.h"
#include "Editor/Scene/SceneNode.h"

#include "Foundation/Log.h"

using namespace Helium;
using namespace Helium::Editor;

DependencyCommand::DependencyCommand( DependencyAction action, const SceneNodePtr& ancestor, const SceneNodePtr& descendant )
: m_Action( action )
, m_Ancestor( ancestor )
, m_Descendant( descendant )
{
  // Make sure the dependency does not already exist if we are trying to connect the ancestor and descendant
  if ( action == Connect )
  {
    // Is m_Desendant already in m_Ancestor->GetDescendants()? It shouldn't be.
    S_SceneNodeSmartPtr::const_iterator foundDescendant = m_Ancestor->GetDescendants().find( m_Descendant );
    if ( foundDescendant != m_Ancestor->GetDescendants().end() )
    {
      Log::Error( TXT( "DependencyCommand - Connection between ancestor (%s) and descendant (%s) already exists\n" ), m_Ancestor->GetName().c_str(), m_Descendant->GetName().c_str() );
      HELIUM_BREAK();
    }

    // Is m_Ancestor already in m_Descendant->GetAncestors()? It shouldn't be.
    S_SceneNodeDumbPtr::const_iterator foundAncestor = m_Descendant->GetAncestors().find( m_Ancestor );
    if ( foundAncestor != m_Descendant->GetAncestors().end() )
    {
      Log::Error( TXT( "DependencyCommand - Connection between ancestor (%s) and descendant (%s) already exists\n" ), m_Ancestor->GetName().c_str(), m_Descendant->GetName().c_str() );
      HELIUM_BREAK();
    }
  }
  // Make sure the dependency exists if we are trying to disconnect the ancestor and descendant
  else if ( action == Disconnect )
  {
    // Is m_Desendant already in m_Ancestor->GetDescendants()? It should be.
    S_SceneNodeSmartPtr::const_iterator foundDescendant = m_Ancestor->GetDescendants().find( m_Descendant );
    if ( foundDescendant == m_Ancestor->GetDescendants().end() )
    {
      Log::Error( TXT( "DependencyCommand - Ancestor (%s) and descendant (%s) are not connected\n" ), m_Ancestor->GetName().c_str(), m_Descendant->GetName().c_str() );
      HELIUM_BREAK();
    }

    // Is m_Ancestor already in m_Descendant->GetAncestors()? It should be.
    S_SceneNodeDumbPtr::const_iterator foundAncestor = m_Descendant->GetAncestors().find( m_Ancestor );
    if ( foundAncestor == m_Descendant->GetAncestors().end() )
    {
      Log::Error( TXT( "DependencyCommand - Ancestor (%s) and descendant (%s) are not connected\n" ), m_Ancestor->GetName().c_str(), m_Descendant->GetName().c_str() );
      HELIUM_BREAK();
    }
  }

  Redo();
}

DependencyCommand::~DependencyCommand()
{
}

void DependencyCommand::Undo()
{
  switch ( m_Action )
  {
  case Connect:
    {
      //Log::Debug( "DependencyCommand::Undo - Breaking connection between ancestor (%s) and descendant (%s)\n", m_Ancestor->GetName().c_str(), m_Descendant->GetName().c_str() );
      m_Descendant->RemoveDependency( m_Ancestor );
    }
    break;

  case Disconnect:
    {
      //Log::Debug( "DependencyCommand::Undo - Creating connection between ancestor (%s) and descendant (%s)\n", m_Ancestor->GetName().c_str(), m_Descendant->GetName().c_str() );
      m_Descendant->CreateDependency( m_Ancestor );
    }
    break;
  }
}

void DependencyCommand::Redo()
{
  switch ( m_Action )
  {
  case Connect:
    {
      //Log::Debug( "DependencyCommand::Redo - Creating connection between ancestor (%s) and descendant (%s)\n", m_Ancestor->GetName().c_str(), m_Descendant->GetName().c_str() );
      m_Descendant->CreateDependency( m_Ancestor );
    }
    break;

  case Disconnect:
    {
      //Log::Debug( "DependencyCommand::Redo - Breaking connection between ancestor (%s) and descendant (%s)\n", m_Ancestor->GetName().c_str(), m_Descendant->GetName().c_str() );
      m_Descendant->RemoveDependency( m_Ancestor );
    }
    break;
  }
}
