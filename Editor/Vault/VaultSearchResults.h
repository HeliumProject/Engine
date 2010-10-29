#pragma once

#include <vector>

#include "Editor/API.h"
#include "Foundation/File/Path.h"
#include "Foundation/Memory/SmartPtr.h"

namespace Helium
{
    namespace Editor
    {
        class VaultSearchResults : public Helium::RefCountBase< VaultSearchResults >
        {
        public:
            VaultSearchResults( uint32_t vaultSearchID = 0 );
            VaultSearchResults( const VaultSearchResults* results );
            virtual ~VaultSearchResults();

            void Clear();
            bool HasResults() const;

            const std::map< uint64_t, Helium::Path >& GetPathsMap() const;
            bool AddPath( const Helium::Path& path );
            bool RemovePath( const Helium::Path& path );

            int32_t GetSearchID() { return m_VaultSearchID; }

        private:
            // This is the ID of the VaultSearch that created these results, for easy of debugging
            int32_t m_VaultSearchID;
            
            std::map< uint64_t, Helium::Path > m_Paths;

            const Helium::Path* Find( const uint64_t& hash ) const;
        };
        typedef Helium::SmartPtr< VaultSearchResults > VaultSearchResultsPtr;
    }
}
