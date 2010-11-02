#pragma once

#include "Pipeline/API.h"

#include "Platform/Types.h"

#include "Foundation/File/Path.h"

#include <set>

namespace Helium
{
    namespace Asset
    {
        class PIPELINE_API AssetFactory
        {
        public:
            AssetFactory()
            {
            }
            virtual ~AssetFactory()
            {
            }

            virtual Helium::SmartPtr< class AssetClass > Create( const Helium::Path& path ) = 0;

            const std::set< tstring >& GetExtensions() const
            {
                return m_FileExtensions;
            }

            void GetExtensions( std::set< tstring >& extensions ) const
            {
                for ( std::set< tstring >::const_iterator itr = m_FileExtensions.begin(), end = m_FileExtensions.end(); itr != end; ++itr )
                {
                    extensions.insert( (*itr) );
                }
            }

        protected:

            // the set of extensions this factory can handle
            std::set< tstring >  m_FileExtensions;
        };
    }
}