#pragma once

#include "Common/Automation/Event.h"
#include "Common/Types.h"
#include "Common/String/Natural.h"

typedef std::set< std::string, CaseSensitiveNatStrCmp > S_NaturalOrderString;

namespace Luna
{
  // Arguments for events dealing with changes to a row in the grid.
  struct GridRowChangeArgs
  {
    u32 m_RowNumber;  // Row number that was changed (zero-based index)

    GridRowChangeArgs( u32 row )
    : m_RowNumber( row )
    {
    }
  };

  // Definition for event delegates dealing with grid row changes.
  typedef Nocturnal::Signature< void, const GridRowChangeArgs& > GridRowChangeSignature;

  struct GridRowRenamedArgs : public GridRowChangeArgs
  {
    std::string m_OldName;
    std::string m_NewName;

    GridRowRenamedArgs( u32 row, const std::string& oldName, const std::string& newName )
    : GridRowChangeArgs( row )
    , m_OldName( oldName )
    , m_NewName( newName )
    {
    }
  };

  typedef Nocturnal::Signature< void, const GridRowRenamedArgs& > GridRowRenamedSignature;


  /////////////////////////////////////////////////////////////////////////////
  // Grid UI for managing the visibility, selectability, and names of layers.
  // Each row in the grid displays the name of the layer and checkboxes to 
  // change its visibility and selectability (in reference to how we render
  // and pick items in Luna.  The layers are maintained alphabetically.  The
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
    u32 m_ColumnViz;
    u32 m_ColumnSel;
    u32 m_ColumnName;

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
    i32 GetRowNumber( const std::string& name ) const;
    const std::string& GetRowName( u32 row ) const;
    bool SetRowName( const std::string& oldName, const std::string& newName );
    bool IsRowVisibleChecked( u32 row );
    void SetRowVisibleState( u32 row, bool checked );
    bool IsRowSelectableChecked( u32 row );
    void SetRowSelectableSate( u32 row, bool checked );
    virtual bool AddRow( const std::string& name, bool visible, bool selectable );
    bool RemoveRow( u32 index );
    bool RemoveRow( const std::string& name );
    bool RemoveAllRows();
    bool IsAnythingSelected() const;
    bool IsSelected( const std::string& name ) const;
    void SelectRow( u32 row, bool addToSelection );
    void DeselectAllRows();
    S_u32 GetSelectedRows() const;
    void BeginBatch();
    void EndBatch();

  protected:
    i32 InsertName( const std::string& name );
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
