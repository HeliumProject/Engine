#pragma once

#include "Engine/AssetLoader.h"

namespace Helium
{
    class CachePackageLoader;

    /// Asset loader for loading objects from binary cached data.
    class HELIUM_ENGINE_API CacheAssetLoader : public AssetLoader
    {
    public:
        /// @name Construction/Destruction
        //@{
        CacheAssetLoader();
        virtual ~CacheAssetLoader();
        //@}

        /// @name Static Initialization
        //@{
        static bool InitializeStaticInstance();
        //@}

    protected:
        /// Package loader (currently only one, may support multiple later).
        CachePackageLoader* m_pPackageLoader;

        /// @name Loading Implementation
        //@{
        virtual PackageLoader* GetPackageLoader( AssetPath path );
        virtual void TickPackageLoaders();
        //@}
    };
}
