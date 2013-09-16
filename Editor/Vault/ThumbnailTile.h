#pragma once

#include "Thumbnail.h"

#include "Foundation/FilePath.h"
#include "Foundation/SmartPtr.h"
#include "Application/OrderedSet.h"

namespace Helium
{
    namespace Editor
    {
        class ThumbnailTile : public Helium::RefCountBase< ThumbnailTile >
        {
        public:
            ThumbnailTile( const Helium::FilePath& path );
            ~ThumbnailTile();

        public:
            const Helium::FilePath& GetPath() const;

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

            std::string GetLabel() const;
            std::string GetEditableName() const;
            const std::string& GetFullPath() const;
            std::string GetTypeLabel() const;
            bool GetTypeColor( uint32_t& color ) const;

            const Thumbnail* GetThumbnail() const;
            void SetThumbnail( ThumbnailPtr thumbnail );

        private:
            Helium::FilePath m_Path;

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

        typedef std::map< Helium::FilePath, ThumbnailTilePtr > M_PathToTilePtr;
    }
}