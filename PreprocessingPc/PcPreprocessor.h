#pragma once

#include "PreprocessingPc/PreprocessingPc.h"
#include "PcSupport/PlatformPreprocessor.h"

namespace Helium
{
    /// PC object and resource preprocessing support.
    class HELIUM_PREPROCESSING_PC_API PcPreprocessor : public PlatformPreprocessor
    {
    public:
        /// @name Construction/Destruction
        //@{
        PcPreprocessor();
        virtual ~PcPreprocessor();
        //@}

        /// @name Platform Parameters
        //@{
        virtual EByteOrder GetByteOrder() const;
        //@}

        /// @name Shader Compiling
        //@{
        virtual size_t GetShaderProfileCount() const;
        virtual bool CompileShader(
            const FilePath& rShaderPath, size_t profileIndex, RShader::EType type, const void* pShaderCode,
            size_t shaderCodeSize, const ShaderToken* pTokens, size_t tokenCount, DynamicArray< uint8_t >& rCompiledCode,
            DynamicArray< String >* pErrorMessages );
        virtual bool FillShaderReflectionData(
            size_t profileIndex, const void* pCompiledCode, size_t compiledCodeSize,
            DynamicArray< ShaderConstantBufferInfo >& rConstantBuffers, DynamicArray< ShaderSamplerInfo >& rSamplers,
            DynamicArray< ShaderTextureInfo >& rTextures );
        //@}
    };
}
