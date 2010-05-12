#pragma once

#include "Asset/ShaderAsset.h"
#include "TextureSlots.h"

namespace ShaderProcess
{
  // Forwards
  struct RuntimeTexture;

  //
  // Wrapper for a pointer to a RuntimeTexture.  The m_UseTexture flag should
  // be checked to determine whether the RuntimeTexture pointer is valid.  Note
  // that just because the pointer is NULL, that doesn't make it invalid (NULL
  // might mean to just use the default texture).  That's why there's a bool flag
  // provided to determine if the pointer (or lack thereof) should be used.
  //
  struct TextureParam
  {
    // Pointer to a runtime texture (may be NULL).
    const RuntimeTexture* m_Texture;

    // Flag indicating whether the value (or lack thereof) in m_Texture should
    // be used to build the shader.
    bool m_UseTexture;

    TextureParam()
      : m_Texture( NULL )
      , m_UseTexture( false )
    {
    }

    TextureParam( const RuntimeTexture* texture, bool useTexture )
      : m_Texture( texture )
      , m_UseTexture( useTexture )
    {
    }
  };
  typedef std::vector< TextureParam > V_TextureParam;

  //
  // Collects all the parameters together that are needed to build a shader into a format
  // useable by the game.
  //
  struct ShaderCreationParams
  {
    Nocturnal::BasicBufferPtr m_Buffer;     // Buffer that shader should be written to (in PS3 format)
    Asset::ShaderAssetPtr m_Shader;   // Shader to write to the buffer.
    V_TextureParam m_TextureSlots; // Information on the texture slots that are part of the shader.

    ShaderCreationParams( const Nocturnal::BasicBufferPtr& buffer, const Asset::ShaderAssetPtr& shader, const V_TextureParam& slots  )
      : m_Buffer( buffer )
      , m_Shader( shader )
      , m_TextureSlots( slots )
    {
    }
  };

  // Functions for creating PS3 formatted shader buffers.
  void MakeStandardShaderBuffer( const ShaderCreationParams& args );
  void MakeGroundFogShaderBuffer( const ShaderCreationParams& args );
  void MakeFurShaderBuffer( const ShaderCreationParams& args );
  void MakeRefractionShaderBuffer( const ShaderCreationParams& args );
  void MakeFurFragmentShaderBuffer( const ShaderCreationParams& args );
  void MakeAnisotropicShaderBuffer( const ShaderCreationParams& args );
  void MakeAudioVisualShaderBuffer( const ShaderCreationParams& args );
  void MakeOffScreenShaderBuffer( const ShaderCreationParams& args );
  void MakeFoliageShaderBuffer( const ShaderCreationParams& args );
  void MakeBRDFShaderBuffer( const ShaderCreationParams& args );
  void MakeGraphShaderBuffer( const ShaderCreationParams& args );

  // Functions for creating a RuntimeTexture from a particular slot on a shader.
  bool MakeStandardShaderColorMapTexture( RuntimeTexture& texture, const Asset::ShaderAssetPtr& shader, TextureSlot slot );
  bool MakeStandardShaderNormalMapTexture( RuntimeTexture& texture, const Asset::ShaderAssetPtr& shader, TextureSlot slot );
  bool MakeStandardShaderExpensiveMapTexture( RuntimeTexture& texture, const Asset::ShaderAssetPtr& shader, TextureSlot slot );
  bool MakeStandardShaderDetailMapTexture( RuntimeTexture& texture, const Asset::ShaderAssetPtr& shader, TextureSlot slot );
  bool MakeAnisotropicExpensiveMapTexture( RuntimeTexture& texture, const Asset::ShaderAssetPtr& shader, TextureSlot slot );
  bool MakeGrainMapTexture( RuntimeTexture& texture, const Asset::ShaderAssetPtr& shader, TextureSlot slot );
  bool MakeFurControlTexture( RuntimeTexture& texture, const Asset::ShaderAssetPtr& shader, TextureSlot slot );
  bool MakeFoliageTexture( RuntimeTexture& texture, const Asset::ShaderAssetPtr& shader, TextureSlot slot );
  bool MakeFoliageShadowTexture( RuntimeTexture& texture, const Asset::ShaderAssetPtr& shader, TextureSlot slot );
  bool MakeBRDFMapTexture( RuntimeTexture& texture, const Asset::ShaderAssetPtr& shader, TextureSlot slot );
  bool MakeOffScreenTexture( RuntimeTexture& texture, const Asset::ShaderAssetPtr& shader, TextureSlot slot );
  
  bool MakeCustomMapATexture( RuntimeTexture& texture, const Asset::ShaderAssetPtr& shader, TextureSlot slot );
  bool MakeCustomMapBTexture( RuntimeTexture& texture, const Asset::ShaderAssetPtr& shader, TextureSlot slot );

  u8   EncodeRTSpecularPowerWSMaterial(f32 specular_power, u32 wetsurface_material);
}
