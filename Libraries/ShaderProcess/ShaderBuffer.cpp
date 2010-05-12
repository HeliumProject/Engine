#include "ShaderBuffer.h"
#include "ShaderRegistry.h"

#include "Asset/ShaderAsset.h"
#include "igCore/igHeaders/ps3structs.h"
#include "TUID/TUID.h"

// Using
using namespace ShaderProcess;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// Pass in the ShaderAsset that you will be building into this buffer.
// 
ShaderBuffer::ShaderBuffer( Asset::ShaderAsset* shaderAsset ) 
: m_ShaderAsset( shaderAsset )
{
  NOC_ASSERT( m_ShaderAsset );

  Reset();
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ShaderBuffer::~ShaderBuffer()
{
}
  
///////////////////////////////////////////////////////////////////////////////
// Resets this buffer so that Build can be called again.  Does NOT reset which
// ShaderAsset is built by this object.
// 
void ShaderBuffer::Reset()
{
  m_Buffer = NULL;
  for ( u32 slot = 0; slot < TextureSlots::NumTextureSlots; ++slot )
  {
    m_TextureIndices[ slot ] = ShaderBuffer::INVALID_INDEX;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Fills out the buffer with data from the ShaderAsset that was specified in
// the constructor.
// 
bool ShaderBuffer::Build( const V_TextureParam& slots )
{
  bool result = false;

  if ( !m_Buffer.ReferencesObject() )
  {
    // Initialize the buffer.
    m_Buffer = new Nocturnal::BasicBuffer();
    const size_t maxBufferSize = sizeof( IGPS3::Shader );
    m_Buffer->SetMaxSize( maxBufferSize );
    m_Buffer->GrowBy( maxBufferSize );

    // Fill the buffer with the shader's information.
    ShaderCreationParams args( m_Buffer, m_ShaderAsset, slots );
    ShaderRegistry::MakeShaderBuffer( args );

    // The texture slots always appear at the beginning of the shader struct.
    // Store their locations and fill them out with the default values.
    u32 offset = 0;
    for ( u32 slot = 0; slot < TextureSlots::NumTextureSlots; ++slot, offset += sizeof( u32 ) )
    {
      Nocturnal::SmartBuffer::Location loc( offset, m_Buffer );
      m_Buffer->AddAtLocU32( m_TextureIndices[ slot ], loc );
      m_TexIndexLocs[ slot ] = loc;
    }

    // Make sure the buffer did not exceed the maximum size (presumably this would be handled in 
    // the smart buffer class, but just in case).
    if ( m_Buffer->GetSize() > maxBufferSize )
    {
      throw Nocturnal::Exception( "Shader buffer for '%s' ("TUID_HEX_FORMAT") exceeded the maximum size of %d bytes.", m_ShaderAsset->GetFilePath().c_str(), m_ShaderAsset->GetFileID(), maxBufferSize );
    }

    result = true;
  }

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if this object has already had Build() called on it.  Call Reset()
// if you need to build again.
// 
bool ShaderBuffer::IsBuilt() const
{
  return m_Buffer.ReferencesObject();
}

///////////////////////////////////////////////////////////////////////////////
// Allows you to specify the index of a particular texture slot.  If this 
// object has already had Build() called, the value will be updated within
// the internal buffer data.  If Build() has not been called yet, the value will
// be cached, and written out when Build() is eventually called.
// 
void ShaderBuffer::SetTextureIndex( TextureSlots::TextureSlot whichTexture, u32 index )
{
  // Attempted to index outside of the range of texture slots in a shader.
  NOC_ASSERT( whichTexture < TextureSlots::NumTextureSlots );

  if ( whichTexture < TextureSlots::NumTextureSlots )
  {
    m_TextureIndices[ whichTexture ] = index;
    if ( IsBuilt() )
    {
      m_Buffer->AddAtLocU32( index, m_TexIndexLocs[ whichTexture ] );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Returns a pointer to the internal buffer where data is stored.  If the 
// buffer has not been built yet, NULL is returned.
// 
Nocturnal::BasicBufferPtr ShaderBuffer::GetBuffer()
{
  return m_Buffer;
}

///////////////////////////////////////////////////////////////////////////////
// Returns a pointer to the internal buffer where data is stored.  This class
// will no longer retain the buffer after you call this function (you will be
// taking ownership of the buffer).
// 
Nocturnal::BasicBufferPtr ShaderBuffer::RelinquishBuffer()
{
  Nocturnal::BasicBufferPtr buffer = m_Buffer;
  m_Buffer = NULL;
  return buffer;
}
