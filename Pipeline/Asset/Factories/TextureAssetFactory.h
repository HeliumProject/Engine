#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Asset/AssetFactory.h"
#include "Pipeline/Asset/AssetClass.h"

#include "Foundation/File/Path.h"

namespace Helium
{
    namespace Asset
    {
        class PIPELINE_API TextureAssetFactory : public AssetFactory
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