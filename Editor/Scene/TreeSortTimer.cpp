#include "Precompile.h"
#include "TreeSortTimer.h"
#include "TreeMonitor.h"

using namespace Editor;


///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
TreeSortTimer::TreeSortTimer( TreeMonitor* treeMonitor )
: m_TreeMonitor( treeMonitor )
, m_IsResetting( false )
, m_IsFrozen( false )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
TreeSortTimer::~TreeSortTimer()
{
  if ( m_IsFrozen )
  {
    m_TreeMonitor->ThawSorting();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Starts the timer and freezes sorting in the tree controls.
// 
bool TreeSortTimer::Start( int milliseconds, bool oneShot )
{
  m_IsResetting = IsRunning();
  bool result = __super::Start( milliseconds, oneShot );
  m_IsResetting = false;

  if ( result && !m_IsFrozen )
  {
    // Only freeze if not already frozen
    m_TreeMonitor->FreezeSorting();
    m_IsFrozen = true;
  }

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Stops the timer and resumes sorting in the tree controls if appropriate.
// 
void TreeSortTimer::Stop()
{
  if ( !m_IsResetting && m_IsFrozen )
  {
    // Only thaw if this control has done a freeze and if the timer is not
    // being reset.
    m_TreeMonitor->ThawSorting();
    m_IsFrozen = false;
  }
  __super::Stop();
}

///////////////////////////////////////////////////////////////////////////////
// Called when the timer is up.  Thaws and re-freezes sorting.
// 
void TreeSortTimer::Notify()
{
  if ( m_IsFrozen )
  {
    m_TreeMonitor->ThawSorting();
    m_IsFrozen = false;

    if ( !IsOneShot() )
    {
      // Only re-freeze if this is a continuous timer.
      m_TreeMonitor->FreezeSorting();
      m_IsFrozen = true;
    }
  }
}
