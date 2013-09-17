#pragma once

#include "PcSupport/PcSupport.h"

#include "Rendering/RShader.h"

#include "Foundation/FilePath.h"

namespace Helium
{
    struct ShaderConstantBufferInfo;
    struct ShaderSamplerInfo;
    struct ShaderTextureInfo;

    /// Platform object and resource preprocessing interface.
    class HELIUM_PC_SUPPORT_API PlatformPreprocessor
    {
    public:
        /// Byte order identifiers.
        enum EByteOrder
        {
            BYTE_ORDER_FIRST   =  0,
            BYTE_ORDER_INVALID = -1,

            /// Little-endian.
            BYTE_ORDER_LITTLE,
            /// Big-endian.
            BYTE_ORDER_BIG,

            BYTE_ORDER_MAX,
            BYTE_ORDER_LAST = BYTE_ORDER_MAX - 1
        };

        /// Shader preprocessor token.
        struct HELIUM_PC_SUPPORT_API ShaderToken
        {
            /// Token name.
            CharString name;
            /// Token definition.
            CharString definition;

            /// @name Construction/Destruction
            //@{
            inline ShaderToken();
            inline ShaderToken( const CharString& rName, const CharString& rDefinition );
            //@}
        };

        /// @name Construction/Destruction
        //@{
        virtual ~PlatformPreprocessor();
        //@}

        /// @name Platform Parameters
        //@{
        virtual EByteOrder GetByteOrder() const = 0;
        inline bool SwapBytes() const;
        //@}

        /// @name Shader Compiling
        //@{
        virtual size_t GetShaderProfileCount() const = 0;
        virtual bool CompileShader(
            const FilePath& rShaderPath, size_t profileIndex, RShader::EType type, const void* pShaderCode,
            size_t shaderCodeSize, const ShaderToken* pTokens, size_t tokenCount, DynamicArray< uint8_t >& rCompiledCode,
            DynamicArray< String >* pErrorMessages ) = 0;
        virtual bool FillShaderReflectionData(
            size_t profileIndex, const void* pCompiledCode, size_t compiledCodeSize,
            DynamicArray< ShaderConstantBufferInfo >& rConstantBuffers, DynamicArray< ShaderSamplerInfo >& rSamplers,
            DynamicArray< ShaderTextureInfo >& rTextures ) = 0;
        //@}
    };
}

#include "PcSupport/PlatformPreprocessor.inl"
