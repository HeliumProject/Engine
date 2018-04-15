#include "Precompile.h"
#include "Framework/NullRendererInitialization.h"

using namespace Helium;

/// @copydoc RendererInitialization::Initialize()
bool NullRendererInitialization::Initialize()
{
    // No Renderer instance is created, so simply return that we have been successful.
    return true;
}

void Helium::NullRendererInitialization::Shutdown()
{

}