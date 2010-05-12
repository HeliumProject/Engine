#pragma once

#include "Undo\Command.h"

namespace Luna
{
  class Selection;

  class RefreshSelectionCommand : public Undo::Command
  {
  private:
    Selection* m_Selection;

  public:
    RefreshSelectionCommand( Selection* selection );
    virtual ~RefreshSelectionCommand();

    virtual void Undo() NOC_OVERRIDE;
    virtual void Redo() NOC_OVERRIDE;
  };
}
