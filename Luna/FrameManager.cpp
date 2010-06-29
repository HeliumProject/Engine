#include "Precompile.h"
#include "FrameManager.h"
#include "FloatingWindow.h"

using namespace Luna;

FrameManager::FrameManager( wxWindow* managedWindow, unsigned int flags )
: wxAuiManager( managedWindow, flags )
{
}

wxAuiFloatingFrame* FrameManager::CreateFloatingFrame( wxWindow* parent, const wxAuiPaneInfo& p )
{
  return new FloatingWindow( parent, this, p );
}
