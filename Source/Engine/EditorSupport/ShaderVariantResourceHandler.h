#pragma once

#include "EditorSupport/EditorSupport.h"

#if HELIUM_TOOLS

#include "PcSupport/ResourceHandler.h"

#include "Graphics/Shader.h"
#include "PcSupport/PlatformPreprocessor.h"

namespace Helium
{
    /// Resource handler for Shader resource types.
    class HELIUM_EDITOR_SUPPORT_API ShaderVariantResourceHandler : public ResourceHandler
    {
        HELIUM_DECLARE_ASSET( ShaderVariantResourceHandler, ResourceHandler );

    public:
        /// Load request pool block size.
        static const size_t LOAD_REQUEST_POOL_BLOCK_SIZE = 8;

        /// @name Construction/Destruction
        //@{
        ShaderVariantResourceHandler();
        virtual ~ShaderVariantResourceHandler();
        //@}

        /// @name Resource Handling Support
        //@{
        virtual const AssetType* GetResourceType() const override;

        virtual bool CacheResource(
            AssetPreprocessor* pAssetPreprocessor, Resource* pResource, const String& rSourceFilePath ) override;
        //@}

    private:
        /// Shader variant load request.
        struct LoadRequest
        {
            /// Shader type.
            RShader::EType shaderType;
            /// User option index.
            uint32_t userOptionIndex;

            /// Shader variant instance.
            ShaderVariantPtr spVariant;
            /// Load request count.
            volatile int32_t requestCount;
        };

        /// Shader variant load request hasher.
        class LoadRequestHash
        {
        public:
            /// @name Overloaded Operators
            //@{
            size_t operator()( const LoadRequest* pRequest ) const;
            //@}
        };

        /// Shader variant load request comparer.
        class LoadRequestEquals
        {
        public:
            /// @name Overloaded Operators
            //@{
            bool operator()( const LoadRequest* pRequest0, const LoadRequest* pRequest1 ) const;
            //@}
        };

        /// Shader variant load request lookup set type.
        typedef ConcurrentHashSet< LoadRequest*, LoadRequestHash, LoadRequestEquals > LoadRequestSetType;

        /// Shader variant load request pool.
        ObjectPool< LoadRequest > m_loadRequestPool;
        /// Load request lookup set.
        LoadRequestSetType m_loadRequestSet;

        /// @name Shader Variant Load Override Support
        //@{
        size_t BeginLoadVariant( Shader* pShader, RShader::EType shaderType, uint32_t userOptionIndex );
        bool TryFinishLoadVariant( size_t loadId, ShaderVariantPtr& rspVariant );
        //@}

        /// @name Shader Variant Load Override Callbacks
        //@{
        static size_t BeginLoadVariantCallback(
            void* pCallbackData, Shader* pShader, RShader::EType shaderType, uint32_t userOptionIndex );
        static bool TryFinishLoadVariantCallback( void* pCallbackData, size_t loadId, ShaderVariantPtr& rspVariant );
        //@}

        /// @name Private Static Utility Functions
        //@{
        static bool CompileShader(
            ShaderVariant* pVariant, PlatformPreprocessor* pPreprocessor, size_t platformIndex,
            size_t shaderProfileIndex, RShader::EType shaderType, const void* pShaderSourceData,
            size_t shaderSourceSize, const DynamicArray< PlatformPreprocessor::ShaderToken >& rTokens,
            DynamicArray< uint8_t >& rCompiledCodeBuffer );
        //@}
    };
}

#endif  // HELIUM_TOOLS
