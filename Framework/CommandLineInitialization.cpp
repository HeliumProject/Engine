#include "FrameworkPch.h"
#include "Framework/CommandLineInitialization.h"

using namespace Helium;

/// Destructor.
CommandLineInitialization::~CommandLineInitialization()
{
}

/// @fn bool CommandLineInitialization::Initialize( String& rModuleName, DynamicArray< String >& rArguments )
/// Initialize the command-line information for the application.
///
/// @param[out] rModuleName  Name of the application module (executable, etc.).
/// @param[out] rArguments   List of command-line arguments.  This does not include the module name.
///
/// @return  True if command-line initialization was successful, false if not.
