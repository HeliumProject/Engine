#pragma once

#include "ThumbnailTile.h"
#include "ThumbnailIterator.h"

#include "VaultMenuIDs.h"

namespace Helium
{
    namespace Editor
    {
        /////////////////////////////////////////////////////////////////////////////
        // Tracks all the different ways that the thumbnail tiles can be sorted so
        // that we can quickly display a sorted list in the view.
        // 
        class ThumbnailSorter
        {
        public:
            ThumbnailSorter();
            virtual ~ThumbnailSorter();

            void SetSortMethod( VaultSortMethod sortMethod );
            VaultSortMethod GetSortMethod() const;

            void Clear();

            void Add( ThumbnailTile* tile );
            void Add( const M_PathToTilePtr& tiles );

            ThumbnailIteratorPtr GetIterator( ThumbnailTile* startingTile = NULL ) const;

            i32 Compare( const ThumbnailTile* first, const ThumbnailTile* second ) const;

        private:
            typedef bool( *SortFunction )( const ThumbnailTile*, const ThumbnailTile* );
            static bool SortAlphabeticalByName( const ThumbnailTile* first, const ThumbnailTile* second );
            static bool SortAlphabeticalByType( const ThumbnailTile* first, const ThumbnailTile* second );

        private:
            VaultSortMethod m_SortMethod;

            typedef std::set< ThumbnailTile*, SortFunction > S_AlphaByName;
            S_AlphaByName m_AlphaByName;

            typedef std::set< ThumbnailTile*, SortFunction > S_AlphaByType;
            S_AlphaByType m_AlphaByType;
        };
    }
}