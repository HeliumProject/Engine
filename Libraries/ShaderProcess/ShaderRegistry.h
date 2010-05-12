#pragma once

#include "TextureSlots.h"

#include "Asset/ShaderAsset.h"

namespace ShaderProcess
{
  // Forwards
  struct RuntimeTexture;
  struct ShaderCreationParams;

  // Creator for making a PS3 formatted buffer from a ShaderAsset.
  typedef void ( *MakeShaderBufferFunc )( const ShaderCreationParams& args );
  typedef std::map< i32, MakeShaderBufferFunc > M_ShaderCreator;

  // Creator for making a RuntimeTexture for a particular texture slot on a ShaderAsset.
  typedef bool ( *MakeRuntimeTextureFunc )( ShaderProcess::RuntimeTexture&, const Asset::ShaderAssetPtr&, TextureSlots::TextureSlot );
  typedef std::pair< i32, ShaderProcess::TextureSlots::TextureSlot > P_TypeToSlot;
  typedef std::map< P_TypeToSlot, MakeRuntimeTextureFunc > M_RuntimeTextureCreator;

  namespace ShaderRegistry
  {
    void Initialize();
    void Cleanup();

    void MakeShaderBuffer( const ShaderCreationParams& args );
    bool MakeRuntimeTexture( ShaderProcess::RuntimeTexture&, const Asset::ShaderAssetPtr&, TextureSlots::TextureSlot slot );

  };

}
