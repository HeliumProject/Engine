#include "FrameworkPch.h"
#include "Framework/System.h"

using namespace Helium;

System* System::sm_pInstance = NULL;

/// Constructor.
System::System()
{
}

/// Destructor.
System::~System()
{
}

/// Shut down the system and all application components.
///
/// This is automatically called on system destruction as well.
///
/// @see Initialize()
void System::Shutdown()
{
    m_arguments.Clear();
}

/// Get the singleton System instance.
///
/// A system instance must be initialized first through the interface of one of the System subclasses.
///
/// @return  Pointer to the System instance.
///
/// @see DestroyStaticInstance()
System* System::GetInstance()
{
    return sm_pInstance;
}

/// Destroy the singleton System instance.
///
/// Note that destroying the System instance does not automatically shut it down.  Shutdown() must be called
/// explicitly on the instance prior to destroying.
///
/// @see GetInstance()
void System::DestroyStaticInstance()
{
    delete sm_pInstance;
    sm_pInstance = NULL;
}
