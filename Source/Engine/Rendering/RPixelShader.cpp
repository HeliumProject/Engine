#include "Precompile.h"
#include "Rendering/RPixelShader.h"

using namespace Helium;

/// Destructor.
RPixelShader::~RPixelShader()
{
}

/// @copydoc RShader::GetType()
RShader::EType RPixelShader::GetType() const
{
    return TYPE_PIXEL;
}
