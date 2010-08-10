#pragma once

#include "Core/API.h"

#include "Platform/Types.h"

#include "Foundation/File/Path.h"

#include <set>

namespace Helium
{
    namespace Asset
    {
        class CORE_API AssetFactory
        {
        public:
            AssetFactory()
            {
            }
            virtual ~AssetFactory()
            {
            }

            virtual Helium::SmartPtr< class AssetClass > Create( const Helium::Path& path ) = 0;

            const std::set< tstring >& GetExtensions()
            {
                return m_FileExtensions;
            }

        protected:

            // the set of extensions this factory can handle
            std::set< tstring >  m_FileExtensions;
        };
    }
}