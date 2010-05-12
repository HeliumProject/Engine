#pragma once

#include "Common/Memory/SmartPtr.h"
#include "Common/Types.h"
#include "smartbuffer/BasicBuffer.h"
#include "TextureSlots.h"
#include "ShaderUtils.h"

// Forwards
namespace Asset
{
  class ShaderAsset;
}

namespace ShaderProcess
{
  // Forwards
  struct RuntimeTexture;

  /////////////////////////////////////////////////////////////////////////////
  // Helper class to build a ShaderAsset into the proper type of PS3 data 
  // buffer.  Separates the texture index information from the rest of the 
  // shader data so that it can be written back into the buffer by the caller.
  // This is because the texture data does not always need to be rebuilt (for
  // example, in Luna while editing a shaders in realtime, the textures are 
  // conditionally rebuilt if the user changes a value that would affect them).
  // 
  class ShaderBuffer
  {
    // Constants for this class.
    enum Constants
    {
      INVALID_INDEX = 0xFFFFFFFF, // Invalid index into a texture array
    };
    
  private:
    Asset::ShaderAsset* m_ShaderAsset;
    Nocturnal::BasicBufferPtr m_Buffer;
    Nocturnal::SmartBuffer::Location m_TexIndexLocs[ TextureSlots::NumTextureSlots ];
    i32 m_TextureIndices[ TextureSlots::NumTextureSlots ];
    
  public:
    ShaderBuffer( Asset::ShaderAsset* shaderAsset );
    virtual ~ShaderBuffer();
    void Reset();
    bool Build( const V_TextureParam& channels );
    bool IsBuilt() const;
    void SetTextureIndex( TextureSlots::TextureSlot whichTexture, u32 index = ShaderBuffer::INVALID_INDEX );
    Nocturnal::BasicBufferPtr GetBuffer();
    Nocturnal::BasicBufferPtr RelinquishBuffer();
  };
}
