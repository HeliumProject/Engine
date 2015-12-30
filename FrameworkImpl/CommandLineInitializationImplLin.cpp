#include "FrameworkImplPch.h"
#include "CommandLineInitializationImpl.h"

#include "Foundation/StringConverter.h"

using namespace Helium;

/// @copydoc CommandLineInitialization::Startup()
void CommandLineInitializationImpl::Startup( String& rModuleName, DynamicArray< String >& rArguments )
{
	rModuleName.Clear();
	rArguments.Clear();

	// NYI
	HELIUM_ASSERT( false );
}
