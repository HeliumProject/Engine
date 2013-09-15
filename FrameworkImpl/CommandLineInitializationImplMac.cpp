#include "FrameworkImplPch.h"
#include "CommandLineInitializationImpl.h"

#include "Foundation/StringConverter.h"

using namespace Helium;

/// @copydoc CommandLineInitializer::Initialize()
bool CommandLineInitializationImpl::Initialize( String& rModuleName, DynamicArray< String >& rArguments )
{
    rModuleName.Clear();
    rArguments.Clear();

    HELIUM_ASSERT( false );

    return true;
}
