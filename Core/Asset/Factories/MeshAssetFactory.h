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
        class CORE_API MeshAssetFactory : public AssetFactory
        {
        public:
            MeshAssetFactory()
                : AssetFactory()
            {
                m_FileExtensions.insert( TXT( "obj" ) );
            }
            virtual ~MeshAssetFactory()
            {
            }

            AssetClassPtr Create( const Helium::Path& path ) HELIUM_OVERRIDE;
        };
    }
}