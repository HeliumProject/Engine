#include "Application/Inspect/Controls/InspectAction.h"
#include "Application/Inspect/Controls/InspectContainer.h"

// Using
using namespace Helium::Inspect;

#ifdef INSPECT_REFACTOR

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

#endif