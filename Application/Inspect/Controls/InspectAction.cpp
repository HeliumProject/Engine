#include "Application/Inspect/Controls/InspectAction.h"
#include "Application/Inspect/Controls/InspectContainer.h"

// Using
using namespace Helium::Inspect;

///////////////////////////////////////////////////////////////////////////////
// 
// 
Action::Action()
{
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
bool Action::Write()
{
  m_ActionEvent.Raise( this );
  return true;
}
