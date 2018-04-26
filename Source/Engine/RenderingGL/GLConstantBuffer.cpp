#include "Precompile.h"

#include "RenderingGL.h"
#include "RenderingGL/GLConstantBuffer.h"

#include "GL/glew.h"

using namespace Helium;

/// Constructor.
///
/// @param[in] pData          Constant buffer data allocated using DefaultAllocator.  This object will assume
///                           ownership of the buffer memory once it has been constructed.
/// @param[in] registerCount  Number of floating-point vector registers covered by the buffer data.  Each register
///                           is assumed to contain four single-precision (32-bit) floating-point values.
GLConstantBuffer::GLConstantBuffer( void* pData, uint16_t registerCount )
: m_pData( pData )
, m_tag( 0 )
, m_registerCount( registerCount )
{
	HELIUM_ASSERT( pData );
}

/// Destructor.
GLConstantBuffer::~GLConstantBuffer()
{
	if( m_pData )
	{
		DefaultAllocator().Free( m_pData );
		m_pData = NULL;
	}
}

/// @copydoc RConstantBuffer::Map()
void* GLConstantBuffer::Map( ERendererBufferMapHint /*hint*/ )
{
	return m_pData;
}

/// @copydoc RConstantBuffer::Unmap()
void GLConstantBuffer::Unmap()
{
	// Increment the tag in order to notify the immediate command proxy that this buffer has been (potentially)
	// modified.
	++m_tag;
}