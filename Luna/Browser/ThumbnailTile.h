#pragma once

#include "Thumbnail.h"

#include "Foundation/File/Path.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Container/OrderedSet.h"

namespace Luna
{
    class ThumbnailTile : public Nocturnal::RefCountBase< ThumbnailTile >
    {
    public:
        ThumbnailTile( const Nocturnal::Path& path );
        virtual ~ThumbnailTile();

    public:
        const Nocturnal::Path& GetPath() const;

        inline u32 GetRow() const
        {
            return m_Row;
        }

        inline u32 GetColumn() const
        {
            return m_Column;
        }

        void GetRowColumn( u32& row, u32& col ) const;
        void SetRowColumn( u32 row, u32 col );

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
        Nocturnal::Path m_Path;

        u32 m_Row;
        u32 m_Column;

        bool m_IsSelected;
        bool m_IsHighlighted;

        ThumbnailPtr m_Thumbnail;
    };
    typedef Nocturnal::SmartPtr< ThumbnailTile > ThumbnailTilePtr;

    typedef std::vector< ThumbnailTilePtr > V_ThumbnailTilePtr;

    typedef std::set< ThumbnailTilePtr > S_ThumbnailTiles;
    typedef Nocturnal::OrderedSet< ThumbnailTilePtr > OS_ThumbnailTiles;

    typedef std::map< const Nocturnal::Path, ThumbnailTilePtr > M_PathToTilePtr;
}
