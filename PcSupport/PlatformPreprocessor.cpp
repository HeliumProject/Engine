//----------------------------------------------------------------------------------------------------------------------
// PlatformPreprocessor.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "PcSupportPch.h"
#include "PcSupport/PlatformPreprocessor.h"

using namespace Lunar;

/// Destructor.
PlatformPreprocessor::~PlatformPreprocessor()
{
}

/// @fn PlatformPreprocessor::EByteOrder PlatformPreprocessor::GetByteOrder() const
/// Get the byte order in which data should be stored for the target platform.
///
/// @return  Preferred platform byte order.

/// @fn size_t PlatformPreprocessor::GetShaderProfileCount() const
/// Get the number of different shader profiles for the target platform.
///
/// @return  Shader profile count.
///
/// @see CompileShader()

/// @fn bool PlatformPreprocessor::CompileShader( size_t profileIndex, RShader::EType type, const void* pShaderCode, size_t shaderCodeSize, const ShaderToken* pTokens, size_t tokenCount, DynArray< uint8_t >& rMicrocode, DynArray< String >* pErrorMessages )
/// Compile a shader for the target platform.
///
/// @param[in]  rShaderPath     Path to the shader file being compiled.
/// @param[in]  profileIndex    Index of the target shader profile (must be a value less than that returned by
///                             GetShaderProfileCount()).
/// @param[in]  type            Shader type.
/// @param[in]  pShaderCode     Pointer to the loaded shader code to compile.
/// @param[in]  shaderCodeSize  Size of the shader code, in bytes.
/// @param[in]  pTokens         Array of shader preprocessor tokens.
/// @param[in]  tokenCount      Number of shader preprocessor tokens in the given array.
/// @param[out] rCompiledCode   Buffer in which the compiled shader code will be stored.
/// @param[out] pErrorMessages  Optional array in which to store error messages generated during the shader
///                             compilation process.
///
/// @return  True if the shader was compiled successfully, false if not.
///
/// @see GetShaderProfileCount()

/// @fn bool PlatformPreprocessor::FillShaderReflectionData( size_t profileIndex, const void* pCompiledCode, size_t compiledCodeSize, DynArray< ShaderConstantBufferInfo >& rConstantBuffers, DynArray< ShaderSamplerInfo >& rSamplers, DynArray< ShaderTextureInfo >& rTextures )
/// Fill out data about the shader constants and texture inputs.
///
/// @param[in]     profileIndex      Index of the target shader profile (must be a value less than that returned by
///                                  GetShaderProfileCount()).
/// @param[in]     pCompiledCode     Buffer containing the compiled shader code.
/// @param[in]     compiledCodeSize  Size of the compiled code buffer, in bytes.
/// @param[in,out] rConstantBuffers  Array containing the information about each constant buffer in the shader.  For
///                                  PC shader model 4, this will fill out the entire set of information about each
///                                  constant buffer.  For other targets, this will only fill out the offsets and
///                                  sizes, so it should already have the names of each constant buffer and constant
///                                  in the proper order (as read from the compiled Direct3D shader model 4
///                                  reflection data).
/// @param[in,out] rSamplers         Array containing the information about each sampler input for the shader.
/// @param[in,out] rTextures         Array containing the information about each texture input for the shader.
///
/// @return  True if the reflection information was filled out successfully, false if not.
