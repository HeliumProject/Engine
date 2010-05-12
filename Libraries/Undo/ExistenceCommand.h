#pragma once

#include "API.h"
#include "Command.h"
#include "FunctionCaller.h"

namespace Undo
{
  /////////////////////////////////////////////////////////////////////////////
  // Defines whether you want to Add or Remove an item.
  // 
  namespace ExistenceActions
  {
  enum ExistenceAction
  {
      Add,
      Remove
  };
  }
  typedef ExistenceActions::ExistenceAction ExistenceAction;


  /////////////////////////////////////////////////////////////////////////////
  // Undoable command for adding or removing an item.
  //
  class UNDO_API ExistenceCommand : public Command
  {
  private:
    ExistenceAction m_Action;
    FunctionCallerPtr m_Add;
    FunctionCallerPtr m_Remove;

  public:
    // Constructor
    ExistenceCommand( ExistenceAction action, FunctionCallerPtr add, FunctionCallerPtr remove, bool redo = true );
    virtual ~ExistenceCommand();
    virtual void Undo() NOC_OVERRIDE;
    virtual void Redo() NOC_OVERRIDE;
  };
  typedef Nocturnal::SmartPtr< ExistenceCommand > ExistenceCommandPtr;
}