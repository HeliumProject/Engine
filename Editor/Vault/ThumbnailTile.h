#pragma once

#include "Thumbnail.h"

#include "Foundation/File/Path.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Container/OrderedSet.h"

namespace Helium
{
    namespace Editor
    {
        class ThumbnailTile : public Helium::RefCountBase
        {
        public:
            ThumbnailTile( const Helium::Path& path );
            virtual ~ThumbnailTile();

        public:
            const Helium::Path& GetPath() const;

            inline uint32_t GetRow() const
            {
                return m_Row;
            }

            inline uint32_t GetColumn() const
            {
                return m_Column;
            }

            void GetRowColumn( uint32_t& row, uint32_t& col ) const;
            void SetRowColumn( uint32_t row, uint32_t col );

            inline bool IsSelected() const 
            {
                return m_IsSelected;
            }

            inline void SetSelected( bool selected )
            {
                m_IsSelected = selected;
            }

            inline bool IsHighlighed() const
            {
                return m_IsHighlighted;
            }

            inline void SetHighlighted( bool highlight )
            {
                m_IsHighlighted = highlight;
            }

            tstring GetLabel() const;
            tstring GetEditableName() const;
            const tstring& GetFullPath() const;
            tstring GetTypeLabel() const;
            bool GetTypeColor( DWORD& color ) const;

            const Thumbnail* GetThumbnail() const;
            void SetThumbnail( ThumbnailPtr thumbnail );

        private:
            Helium::Path m_Path;

            uint32_t m_Row;
            uint32_t m_Column;

            bool m_IsSelected;
            bool m_IsHighlighted;

            ThumbnailPtr m_Thumbnail;
        };
        typedef Helium::SmartPtr< ThumbnailTile > ThumbnailTilePtr;

        typedef std::vector< ThumbnailTilePtr > V_ThumbnailTilePtr;

        typedef std::set< ThumbnailTilePtr > S_ThumbnailTiles;
        typedef Helium::OrderedSet< ThumbnailTilePtr > OS_ThumbnailTiles;

        typedef std::map< const Helium::Path, ThumbnailTilePtr > M_PathToTilePtr;
    }
}