#pragma once

#include "Core/API.h"
#include "Core/Asset/AssetFactory.h"
#include "Core/Asset/AssetClass.h"

#include "Platform/Compiler.h"

#include "Foundation/File/Path.h"

namespace Helium
{
    namespace Asset
    {
        class CORE_API TextureAssetFactory : public AssetFactory
        {
        public:
            TextureAssetFactory()
                : AssetFactory()
            {
                m_FileExtensions.insert( TXT( "png" ) );
				m_FileExtensions.insert( TXT( "jpg" ) );
				m_FileExtensions.insert( TXT( "tif" ) );
				m_FileExtensions.insert( TXT( "tga" ) );
				m_FileExtensions.insert( TXT( "psd" ) );
				m_FileExtensions.insert( TXT( "raw" ) );
            }
            virtual ~TextureAssetFactory()
            {
            }

            AssetClassPtr Create( const Helium::Path& path ) HELIUM_OVERRIDE;
        };
    }
}