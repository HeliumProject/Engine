#pragma once

#include "Grid.h"

namespace Helium
{
    namespace Editor
    {
        /////////////////////////////////////////////////////////////////////////////
        // Overrides Grid and adds a column for rendering icons in each row.
        // 
        class GridWithIcons : public Grid
        {
        private:
            // New list of the column numbers
            enum Columns
            {
                Visibility = 0,
                Selectability,
                Icon,
                Name,

                COLUMN_TOTAL
            };

        protected:
            uint32_t m_ColumnIcon;

        public:
            GridWithIcons( wxWindow* parent, wxWindowID gridID, bool allowRename = false, bool showColLabels = true );
            virtual ~GridWithIcons();
            virtual bool AddRow( const std::string& name, bool visible, bool selectable, int32_t imageIndex );

        protected:
            virtual bool AddRow( const std::string& name, bool visible, bool selectable ) override;
            virtual void ResizeColumns() override;
        };
    }
}