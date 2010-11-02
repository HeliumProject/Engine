#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Asset/AssetFactory.h"
#include "Pipeline/Asset/AssetClass.h"

#include "Platform/Compiler.h"

#include "Foundation/File/Path.h"

namespace Helium
{
    namespace Asset
    {
        class PIPELINE_API MeshAssetFactory : public AssetFactory
        {
        public:
            MeshAssetFactory()
                : AssetFactory()
            {
                m_FileExtensions.insert( TXT( "obj" ) );
				m_FileExtensions.insert( TXT( "fbx" ) );
            }
            virtual ~MeshAssetFactory()
            {
            }

            AssetClassPtr Create( const Helium::Path& path ) HELIUM_OVERRIDE;
        };
    }
}