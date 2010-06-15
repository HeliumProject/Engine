#pragma once

#include "Application/API.h"

#include "Command.h"

namespace Undo
{
  //
  // This is a tuple of commands for making multiple changes to multiple objects in a single command
  //

  class APPLICATION_API BatchCommand : public Command
  {
  protected:
    bool m_IsSignificant;
    V_CommandSmartPtr m_Commands;

  public:
    BatchCommand();
    BatchCommand(const V_CommandSmartPtr& objects);
    virtual ~BatchCommand();
    void Set(const V_CommandSmartPtr& commands);
    void Push(const CommandPtr& command);
    virtual void Undo() NOC_OVERRIDE;
    virtual void Redo() NOC_OVERRIDE;
    virtual bool IsSignificant() const NOC_OVERRIDE;
    virtual bool IsEmpty() const;
  };

  typedef Nocturnal::SmartPtr<BatchCommand> BatchCommandPtr;
}