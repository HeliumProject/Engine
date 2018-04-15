#include "Precompile.h"
#include "RenderingD3D9/D3D9ConstantBuffer.h"

using namespace Helium;

/// Constructor.
///
/// @param[in] pData          Constant buffer data allocated using DefaultAllocator.  This object will assume
///                           ownership of the buffer memory once it has been constructed.
/// @param[in] registerCount  Number of floating-point vector registers covered by the buffer data.  Each register
///                           is assumed to contain four single-precision (32-bit) floating-point values.
D3D9ConstantBuffer::D3D9ConstantBuffer( void* pData, uint16_t registerCount )
: m_pData( pData )
, m_tag( 0 )
, m_registerCount( registerCount )
{
    HELIUM_ASSERT( pData );
}

/// Destructor.
D3D9ConstantBuffer::~D3D9ConstantBuffer()
{
    DefaultAllocator().Free( m_pData );
}

/// @copydoc RConstantBuffer::Map()
void* D3D9ConstantBuffer::Map( ERendererBufferMapHint /*hint*/ )
{
    return m_pData;
}

/// @copydoc RConstantBuffer::Unmap()
void D3D9ConstantBuffer::Unmap()
{
    // Increment the tag in order to notify the immediate command proxy that this buffer has been (potentially)
    // modified.
    ++m_tag;
}
