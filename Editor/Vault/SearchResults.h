#pragma once

#include <vector>

#include "Editor/API.h"
#include "Foundation/File/Path.h"
#include "Foundation/Memory/SmartPtr.h"

namespace Helium
{
    namespace Editor
    {
        class SearchResults : public Helium::RefCountBase< SearchResults >
        {
        public:
            SearchResults( u32 vaultSearchID = 0 );
            SearchResults( const SearchResults* results );
            virtual ~SearchResults();

            void Clear();
            bool HasResults() const;

            const std::map< u64, Helium::Path >& GetPathsMap() const;
            bool AddPath( const Helium::Path& path );
            bool RemovePath( const Helium::Path& path );

            i32 GetSearchID() { return m_VaultSearchID; }
        private:
            i32 m_VaultSearchID; // This is the ID of the VaultSearch that created these results, for easy of debugging
            std::map< u64, Helium::Path >  m_Paths;

            const Helium::Path* Find( const u64& hash ) const;
        };
        typedef Helium::SmartPtr< SearchResults > SearchResultsPtr;
    }
}