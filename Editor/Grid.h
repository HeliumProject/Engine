#pragma once

#include <set>

#include "Foundation/Event.h"
#include "Platform/Types.h"
#include "Foundation/String/Natural.h"

typedef std::set< tstring, CaseSensitiveNatStrCmp > S_NaturalOrderString;

namespace Helium
{
    namespace Editor
    {
        // Arguments for events dealing with changes to a row in the grid.
        struct GridRowChangeArgs
        {
            uint32_t m_RowNumber;  // Row number that was changed (zero-based index)

            GridRowChangeArgs( uint32_t row )
                : m_RowNumber( row )
            {
            }
        };

        // Definition for event delegates dealing with grid row changes.
        typedef Helium::Signature< const GridRowChangeArgs& > GridRowChangeSignature;

        struct GridRowRenamedArgs : public GridRowChangeArgs
        {
            tstring m_OldName;
            tstring m_NewName;

            GridRowRenamedArgs( uint32_t row, const tstring& oldName, const tstring& newName )
                : GridRowChangeArgs( row )
                , m_OldName( oldName )
                , m_NewName( newName )
            {
            }
        };

        typedef Helium::Signature< const GridRowRenamedArgs& > GridRowRenamedSignature;


        /////////////////////////////////////////////////////////////////////////////
        // Grid UI for managing the visibility, selectability, and names of layers.
        // Each row in the grid displays the name of the layer and checkboxes to 
        // change its visibility and selectability (in reference to how we render
        // and pick items in Editor.  The layers are maintained alphabetically.  The
        // grid assumes that the name of each layer is unique.
        // 
        class Grid : public wxEvtHandler
        {
        private:
            // Columns of the grid, in order from left to right
            enum Columns
            {
                Visibility = 0, // Visibility checkbox for a row
                Selectability,  // Selectability checkbox for a row
                Name,           // Name of the row (string)

                COLUMN_TOTAL    // Total number of columns in the grid
            };

        protected:
            wxPanel* m_Panel;
            wxGrid* m_Grid;
            wxGridCellCoords m_MouseDownCell;
            S_NaturalOrderString m_Names;
            GridRowChangeSignature::Event m_RowVisibilityChanged;
            GridRowChangeSignature::Event m_RowSelectabilityChanged;
            GridRowRenamedSignature::Event m_RowRenamed;
            bool m_IsCellChanging;
            wxSizer* m_Sizer;
            uint32_t m_ColumnViz;
            uint32_t m_ColumnSel;
            uint32_t m_ColumnName;

        public:
            Grid( wxWindow* parent, wxWindowID gridID, bool allowRename = false, bool showColLabels = true );
            virtual ~Grid();
            void AddRowVisibilityChangedListener( const GridRowChangeSignature::Delegate& listener );
            void RemoveRowVisibilityChangedListener( const GridRowChangeSignature::Delegate& listener );
            void AddRowSelectabilityChangedListener( const GridRowChangeSignature::Delegate& listener );
            void RemoveRowSelectabilityChangedListener( const GridRowChangeSignature::Delegate& listener );
            void AddRowRenamedListener( const GridRowRenamedSignature::Delegate& listener );
            void RemoveRowRenamedListener( const GridRowRenamedSignature::Delegate& listener );
            wxPanel* GetPanel() const;
            int32_t GetRowNumber( const tstring& name ) const;
            const tstring& GetRowName( uint32_t row ) const;
            bool SetRowName( const tstring& oldName, const tstring& newName );
            bool IsRowVisibleChecked( uint32_t row );
            void SetRowVisibleState( uint32_t row, bool checked );
            bool IsRowSelectableChecked( uint32_t row );
            void SetRowSelectableSate( uint32_t row, bool checked );
            virtual bool AddRow( const tstring& name, bool visible, bool selectable );
            bool RemoveRow( uint32_t index );
            bool RemoveRow( const tstring& name );
            bool RemoveAllRows();
            bool IsAnythingSelected() const;
            bool IsSelected( const tstring& name ) const;
            void SelectRow( uint32_t row, bool addToSelection );
            void DeselectAllRows();
            std::set< uint32_t > GetSelectedRows() const;
            void BeginBatch();
            void EndBatch();

        protected:
            int32_t InsertName( const tstring& name );
            virtual void ResizeColumns();
            void OnSize( wxSizeEvent& event );
            void OnMouseLeftDown( wxMouseEvent& event );
            void OnMouseLeftUp( wxMouseEvent& event );
            void OnCellChange( wxGridEvent& event );

        private:
            // Event table
            DECLARE_EVENT_TABLE();
        };
    }
}