#pragma once

#include "Platform/Compiler.h"

namespace Editor
{
  class TreeMonitor;

  /////////////////////////////////////////////////////////////////////////////
  // Class for turning tree sorting on and off periodically.  Intended to speed
  // up rapid object placement (shrubbing) so that tree sorting in the UI is 
  // disabled until there is a significant pause in placing objects.
  // 
  class TreeSortTimer : public wxTimer
  {
  private:
    TreeMonitor* m_TreeMonitor;
    bool m_IsResetting;
    bool m_IsFrozen;

  public:
    enum Constants
    {
      DefaultMilliseconds = 2000, // 2 seconds
    };

  public:
    TreeSortTimer( TreeMonitor* treeMonitor );
    virtual ~TreeSortTimer();

    virtual bool Start( int milliseconds = -1, bool oneShot = false ) HELIUM_OVERRIDE;
    virtual void Stop() HELIUM_OVERRIDE;
    virtual void Notify() HELIUM_OVERRIDE;
  };
}
