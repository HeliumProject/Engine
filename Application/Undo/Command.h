#pragma once

#include <vector>

#include "Application/API.h"

#include "Platform/Compiler.h"
#include "Foundation/Memory/SmartPtr.h"

namespace Undo
{
  class APPLICATION_API Command : public Nocturnal::RefCountBase<Command>
  {
  public:
    //
    // Construction
    //

    Command()
    {

    }

    virtual ~Command()
    {

    }


    //
    // Entry Points
    //

    // undo it
    virtual void Undo() = 0;

    // do it again
    virtual void Redo() = 0;


    //
    // A significant command is one that changes data in such a way that a save is required.
    //  By default, all commands are significant.  Override this in a derived class if you do not 
    //  want your command to cause the application to think that a save is required.  It is up
    //  to the application code to check this value and behave appropriately.
    //

    virtual bool IsSignificant() const
    {
      return true;
    }
  };

  typedef Nocturnal::SmartPtr<Command> CommandPtr;
  typedef std::vector<CommandPtr> V_CommandSmartPtr;
}