#include "EditorPch.h"
#include "Grid.h"

#include "Platform/Assert.h"
#include "Foundation/Log.h"

using namespace Helium;
using namespace Helium::Editor;

// String representations of true/false as needed by cells in the grid
static const wxString BOOL_TRUE = wxT( "1" );
static const wxString BOOL_FALSE = wxT( "" );

///////////////////////////////////////////////////////////////////////////////
// Helper function for sorting an array of ints.
// 
int wxCMPFUNC_CONV wxArrayIntCompare( int* first, int* second )
{
  return *first - *second;
}

///////////////////////////////////////////////////////////////////////////////
// Table for GUI events
// 
BEGIN_EVENT_TABLE( Grid, wxEvtHandler )
EVT_SIZE( Grid::OnSize )
EVT_LEFT_DOWN( Grid::OnMouseLeftDown )
EVT_LEFT_UP( Grid::OnMouseLeftUp )
END_EVENT_TABLE()

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
Grid::Grid( wxWindow* parent, wxWindowID gridID, bool allowRename, bool showColLabels )
: m_Panel( new wxPanel( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER, TXT( "Grid Panel" ) ) )
, m_Grid( new wxGrid( m_Panel, gridID ) )
, m_MouseDownCell( -1, -1 )
, m_IsCellChanging( false )
, m_Sizer( NULL )
, m_ColumnViz( Visibility )
, m_ColumnSel( Selectability )
, m_ColumnName( Name )
{
  // Do this first!
  m_Grid->CreateGrid( 0, COLUMN_TOTAL, wxGrid::wxGridSelectRows );

  BeginBatch();

  // Insert this class into the event handler list for the grid window
  m_Grid->GetGridWindow()->PushEventHandler( this );
  m_Grid->Connect( m_Grid->GetId(), wxEVT_GRID_CELL_CHANGE, wxGridEventHandler( Grid::OnCellChange ), NULL, this );

  // Set up the grid
  m_Grid->SetColLabelValue( m_ColumnViz, wxT( "V" ) );
  m_Grid->SetColLabelValue( m_ColumnSel, wxT( "S" ) );
  m_Grid->SetColLabelValue( m_ColumnName, wxT( "Name" ) );
  m_Grid->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
  m_Grid->SetRowLabelSize( 0 );
  m_Grid->SetColLabelSize( showColLabels ? 20 : 0 );

  // Prevent resizing rows and columns (headers and body of grid)
  m_Grid->DisableDragColSize();
  m_Grid->DisableDragGridSize(); 

  m_Grid->EnableEditing( true );

  // Don't let the horizontal scrollbar appear (there's a gutter on the right that
  // makes the scrollbar appear even when not needed).
  m_Grid->SetScrollLineX( 1 ); 

  // Prevent highlighting of individual cells (we want whole row selection)
  m_Grid->SetCellHighlightPenWidth( 0 ); 
  m_Grid->SetCellHighlightROPenWidth( 0 ); // i.e. Read-only pen width

  // Settings for visible and selectable columns
  wxGridCellAttr* boolAttr = new wxGridCellAttr();
  boolAttr->SetRenderer( new wxGridCellBoolRenderer() );
  boolAttr->SetReadOnly( true );
  boolAttr->SetAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );
  m_Grid->SetColAttr( m_ColumnViz, boolAttr );
  // Reuse same settings, but make a copy since the grid takes ownership of the attribute
  m_Grid->SetColAttr( m_ColumnSel, boolAttr->Clone() );

  // Make the two bool columns as small as possible
  m_Grid->AutoSizeColumn( m_ColumnViz );
  m_Grid->AutoSizeColumn( m_ColumnSel );

  // Settings for Name column
  wxGridCellAttr* stringAttr = new wxGridCellAttr();
  stringAttr->SetReadOnly( !allowRename );
  m_Grid->SetColAttr( m_ColumnName, stringAttr );

  // Set up the sizer
  m_Sizer = new wxBoxSizer( wxVERTICAL );
  m_Sizer->Add( m_Grid, 1, wxEXPAND );
  m_Sizer->Layout();
  m_Panel->SetAutoLayout( true );
  m_Panel->SetSizer( m_Sizer );
  m_Sizer->SetSizeHints( m_Panel );
  EndBatch();
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
Grid::~Grid()
{
  // Release our position as event handler for the grid
  m_Grid->GetGridWindow()->PopEventHandler();
  m_Grid->Disconnect( m_Grid->GetId(), wxEVT_GRID_CELL_CHANGE, wxGridEventHandler( Grid::OnCellChange ), NULL, this );

  // If the panel does not have a parent, we are responsible for deleting it
  if ( m_Panel->GetParent() == NULL )
  {
    delete m_Panel;
    m_Panel = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Adds a listener for when the visibility checkbox in any of the rows is 
// changed.
// 
void Grid::AddRowVisibilityChangedListener( const GridRowChangeSignature::Delegate& listener )
{
  m_RowVisibilityChanged.Add( listener );
}

///////////////////////////////////////////////////////////////////////////////
// Removes a previously added listener for visibility checkbox changes.
// 
void Grid::RemoveRowVisibilityChangedListener( const GridRowChangeSignature::Delegate& listener )
{
  m_RowVisibilityChanged.Remove( listener );
}

///////////////////////////////////////////////////////////////////////////////
// Adds a listener for when the selectability checkbox in any of the rows is
// changed.
// 
void Grid::AddRowSelectabilityChangedListener( const GridRowChangeSignature::Delegate& listener )
{
  m_RowSelectabilityChanged.Add( listener );
}

///////////////////////////////////////////////////////////////////////////////
// Removes a previously added listener for selectability checkbox changes.
// 
void Grid::RemoveRowSelectabilityChangedListener( const GridRowChangeSignature::Delegate& listener )
{
  m_RowSelectabilityChanged.Remove( listener );
}

///////////////////////////////////////////////////////////////////////////////
// Add a listener for when a row is renamed (only works if this object was 
// constructed as allowing rows to be renamed).
// 
void Grid::AddRowRenamedListener( const GridRowRenamedSignature::Delegate& listener )
{
  m_RowRenamed.Add( listener );
}

///////////////////////////////////////////////////////////////////////////////
// Remove a listener for row rename events.
// 
void Grid::RemoveRowRenamedListener( const GridRowRenamedSignature::Delegate& listener )
{
  m_RowRenamed.Remove( listener );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the panel that contains the grid control.  This panel can be nested
// insided another panel, or added to the main frame of the application.
// 
wxPanel* Grid::GetPanel() const
{
  return m_Panel;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the row number for the row with the specified name, or -1 if no
// row with that name was found.
// 
int32_t Grid::GetRowNumber( const std::string& name ) const
{
  int32_t row = -1;
  S_NaturalOrderString::const_iterator found = m_Names.find( name );
  if ( found != m_Names.end() )
  {
    row = distance( m_Names.begin(), found );
  }

  return row;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the name of the specified row, or an empty string if the specified
// row was larger than the number of layers contained in the grid.
// 
const std::string& Grid::GetRowName( uint32_t row ) const
{
  static const std::string emptyString( TXT( "" ) );

  if ( row >= static_cast< uint32_t >( m_Names.size() ) )
  {
    return emptyString;
  }

  S_NaturalOrderString::const_iterator itr = m_Names.begin();
  advance( itr, row );
  HELIUM_ASSERT( *itr == std::string( m_Grid->GetCellValue( row, m_ColumnName ).c_str() ) );
  return *itr;
}

///////////////////////////////////////////////////////////////////////////////
// Changes the name of the row with the oldName, to the specified newName. 
// Returns true if everything went as planned.
// 
bool Grid::SetRowName( const std::string& oldName, const std::string& newName )
{
  bool isOk = false;

  int32_t oldRow = GetRowNumber( oldName );
  if ( oldRow >= 0 )
  {
    // Save the state of the old row (including whether or not it was selected).
    bool isVisible = IsRowVisibleChecked( oldRow );
    bool isSelectable = IsRowSelectableChecked( oldRow );
    bool isSelected = IsSelected( oldName );

    // Remove the old row
    bool removed = RemoveRow( oldRow );
    if ( !removed )
    {
      Log::Error( TXT( "Grid::SetRowName - unable to remove old row %s (#%d).\n" ), oldName.c_str(), oldRow );
    }

    // Insert a new row with the new name and old state
    bool inserted = AddRow( newName, isVisible, isSelectable );
    if ( !inserted )
    {
      Log::Error( TXT( "Grid::SetRowName - unable to insert new row %s.\n" ), newName.c_str() );
    }
    else
    {
      // Restore selection if necessary
      if ( isSelected )
      {
        int32_t newRow = GetRowNumber( newName );
        HELIUM_ASSERT( newRow >= 0 );
        SelectRow( newRow, true );
      }
      isOk = true;
    }
  }

  return isOk;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the "Visiblity" column for the specified row has a checkmark
// in it.
// 
bool Grid::IsRowVisibleChecked( uint32_t row )
{
  return m_Grid->GetCellValue( row, m_ColumnViz ) == BOOL_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the checkmark state for the "Visibility" column of the specified row.
// No events are generated since this is not in response to a user interaction
// (such as clicking on the checkbox with a mouse).
// 
void Grid::SetRowVisibleState( uint32_t row, bool checked )
{
  m_Grid->SetCellValue( row, m_ColumnViz, checked ? BOOL_TRUE : BOOL_FALSE );
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the "selectable" column for the specified row has a checkmark
// in it.
// 
bool Grid::IsRowSelectableChecked( uint32_t row )
{
  return m_Grid->GetCellValue( row, m_ColumnSel ) == BOOL_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the checkmark state for the "selectable" column of the specified row.
// No events are generated since this is not in response to a user interaction
// (such as clicking on the checkbox with a mouse).
// 
void Grid::SetRowSelectableSate( uint32_t row, bool checked )
{
  m_Grid->SetCellValue( row, m_ColumnSel, checked ? BOOL_TRUE : BOOL_FALSE );
}

///////////////////////////////////////////////////////////////////////////////
// Adds a layer to the grid with the specified name, visibility, and 
// selectability.  Returns true if the layer was successfully added, otherwise
// returns false.
// 
bool Grid::AddRow( const std::string& name, bool visible, bool selectable )
{
  bool isOk = false;
  int32_t row = InsertName( name );
  if ( row >= 0 && m_Grid->InsertRows( row, 1 ) )
  {
    m_Grid->SetCellValue( name.c_str(), row, m_ColumnName );
    m_Grid->SetCellValue( visible    ? BOOL_TRUE : BOOL_FALSE, row, m_ColumnViz );
    m_Grid->SetCellValue( selectable ? BOOL_TRUE : BOOL_FALSE, row, m_ColumnSel );
    isOk = true;
  }
  else
  {
    Log::Error( TXT( "Unable to insert layer [%s] into grid at row [%d]\n" ), name.c_str(), row );
    HELIUM_BREAK(); // This shouldn't happen
  }

  return isOk;
}

///////////////////////////////////////////////////////////////////////////////
// Removes the layer at the specified row index in the grid.
// 
bool Grid::RemoveRow( uint32_t row )
{
  bool isOk = false;
  if ( row < m_Names.size() )
  {
    S_NaturalOrderString::iterator itr = m_Names.begin();
    advance( itr, row );
    HELIUM_ASSERT( *itr == std::string( m_Grid->GetCellValue( row, m_ColumnName ).c_str() ) );
    m_Names.erase( itr );
    isOk = m_Grid->DeleteRows( row, 1 );
  }

  return isOk;
}

///////////////////////////////////////////////////////////////////////////////
// Removes the layer with the specified name from the grid.
// 
bool Grid::RemoveRow( const std::string& name )
{
  bool isOk = false;
  int32_t row = GetRowNumber( name );
  if ( row >= 0 )
  {
    isOk = RemoveRow( row );
  }

  return isOk;
}

///////////////////////////////////////////////////////////////////////////////
// Removes all cells from the grid and clears the internal list of names
// maintained for each row.
// 
bool Grid::RemoveAllRows()
{
  bool isOk = false;
  HELIUM_ASSERT( m_Names.size() == m_Grid->GetNumberRows() ); // sanity check
  if ( m_Names.size() > 0 )
  {
    m_Names.clear();
    isOk = m_Grid->DeleteRows( 0, m_Grid->GetNumberRows() );
  }
  return isOk;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if anything in the grid is selected.
// 
bool Grid::IsAnythingSelected() const
{
  return m_Grid->IsSelection();
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the row with the specified name is selected.  Returns false
// if the requested row name does not exist.
// 
bool Grid::IsSelected( const std::string& name ) const
{
  bool isSelected = false;
  int32_t row = GetRowNumber( name );
  if ( row >= 0 )
  {
    isSelected = m_Grid->IsInSelection( row, m_ColumnName );
  }
  return isSelected;
}

///////////////////////////////////////////////////////////////////////////////
// Selects the specified row (rows are zero-based indexed, starting at the top).
// If addToSelection is true, the row is added to the list of currently 
// selected rows.  Otherwise, the selection is cleared and only the specified
// row will be selected when this function returns.
// 
void Grid::SelectRow( uint32_t row, bool addToSelection )
{
  // Seems to be some weirdness with row selection in wxGrid.  For example,
  // I would have thought that we could just call:
  // m_Grid->SelectRow( row, addToSelection );
  // But if you fetch the selection back, in some cases, it won't match
  // what the grid is displaying.  It seems to work better if we manually
  // simulate the whole row selection.

  m_Grid->SelectBlock( row, 0, row, m_Grid->GetNumberCols() - 1, addToSelection );
}

///////////////////////////////////////////////////////////////////////////////
// Deselects all the rows in the grid that are currently selected.
// 
void Grid::DeselectAllRows()
{
  m_Grid->ClearSelection();
}

///////////////////////////////////////////////////////////////////////////////
// Returns a set of all the rows that are currently selected, in ascending 
// order by row number.
// 
std::set< uint32_t > Grid::GetSelectedRows() const
{
  // NOTE: m_Grid->GetSelectedRows() only reports rows that are selected by clicking on the
  // row header (or calling SelectRows directly).  This does us no good since we don't have
  // row headers.  We have to calculate the row selection manually.
  wxArrayInt selectionArray = m_Grid->GetSelectedRows();

  std::set< uint32_t > selection;

  for ( size_t i = 0; i < selectionArray.Count(); ++i )
  {
      selection.insert( selectionArray[ i ] );
  }

  //wxGridCellCoordsArray topLeftArray = m_Grid->GetSelectionBlockTopLeft();
  //wxGridCellCoordsArray bottomRightArray = m_Grid->GetSelectionBlockBottomRight();
  //HELIUM_ASSERT( topLeftArray.GetCount() == bottomRightArray.GetCount() );

  //uint32_t currentTop = 0;
  //uint32_t currentBottom = 0;
  //const size_t numCells = topLeftArray.GetCount();
  //for ( ; currentTop < numCells; ++currentTop, ++currentBottom )
  //{
  //  uint32_t firstRow = topLeftArray[currentTop].GetRow();
  //  uint32_t lastRow = bottomRightArray[currentBottom].GetRow();
  //  for ( uint32_t currentRow = firstRow; currentRow <= lastRow; ++currentRow )
  //  {
  //    selection.insert( currentRow );
  //  }
  //}

  return selection;
}

///////////////////////////////////////////////////////////////////////////////
// Increments the internal batch count.  Halts redrawing the grid UI until the
// batch count reaches zero.  Every call to BeginBatch() should be accompanied
// by a call to EndBatch().
// 
void Grid::BeginBatch()
{
  m_Grid->BeginBatch();
}

///////////////////////////////////////////////////////////////////////////////
// Decrements the internal batch count.  When the batch count reaches zero,
// the grid will be redrawn, and continue to update as normal.  See BeginBatch()
// for more information.
// 
void Grid::EndBatch()
{
  m_Grid->EndBatch();
}

///////////////////////////////////////////////////////////////////////////////
// Inserts the name into our list.  Returns the index of the name within the
// list (the list of names is alphabetical).  Returns -1 if the name could
// not be inserted into the list for some reason (perhaps it was already
// in the list?).
// 
int32_t Grid::InsertName( const std::string& name )
{
  int32_t row = -1;

  // The name should be unique, so add it to our list
  std::pair< S_NaturalOrderString::const_iterator, bool > inserted = m_Names.insert( name );
  if ( inserted.second )
  {
    // Figure out where it was inserted in the list
    row = distance( m_Names.begin(), inserted.first );
  }
  else
  {
    Log::Error( TXT( "Layer named %s already exists\n" ), name.c_str() );
  }

  return row;
}

///////////////////////////////////////////////////////////////////////////////
// Expands or contracts the name column to fill the space available in the
// containing window.
// 
void Grid::ResizeColumns()
{
  wxSize gridSize = m_Grid->GetSize();
  wxSize gridWindowSize = m_Grid->GetGridWindow()->GetSize();
  //wxSize sizerSize = m_Sizer ? m_Sizer->GetSize() : wxSize( -1, -1 );
  //wxSize panelSize; 
  //m_Panel->GetSize( &panelSize.x, &panelSize.y );
  //wxPoint virtualSize;
  //m_Grid->GetVirtualSize( &virtualSize.x, &virtualSize.y );
  //Log::Debug( "=============================================\n" );
  //Log::Debug( "Grid Size:        %d, %d\n", gridSize.x, gridSize.y );
  //Log::Debug( "Grid Window Size: %d, %d\n", gridWindowSize.x, gridWindowSize.y );
  //Log::Debug( "Sizer size:       %d, %d\n", sizerSize.x, sizerSize.y );
  //Log::Debug( "Panel Size:       %d, %d\n", panelSize.x, panelSize.y );
  //Log::Debug( "Grid Virt Size:   %d, %d\n", virtualSize.x, virtualSize.y );
  //Log::Debug( "=============================================\n\n" );

  // If the vertical scrollbar is currently visible, include it into our calculation to
  // prevent the horizontal scrollbar from also showing up.  This control does not ever
  // allow the horizontal scrollbar to be present.
  bool isVScrollbarShowing = ( gridWindowSize.x != gridSize.x );
  const int32_t scrollGutter = isVScrollbarShowing ? wxSystemSettings::GetMetric( wxSYS_VSCROLL_X ) : 0;
  const int32_t startWidth = m_Grid->GetColSize( m_ColumnViz ) + m_Grid->GetColSize( m_ColumnSel );
  const int32_t totalWidth = m_Panel->GetSize().x;
  const int32_t pad = m_Grid->GetScrollLineX() + scrollGutter;
  const int32_t width = totalWidth - startWidth - pad;
  m_Grid->SetColSize( m_ColumnName, width );
}

///////////////////////////////////////////////////////////////////////////////
// Called when the grid window is resized.  Resizes the grid to fill the 
// available space.
// 
void Grid::OnSize( wxSizeEvent& event )
{
  ResizeColumns();
  m_Grid->ForceRefresh();
}

///////////////////////////////////////////////////////////////////////////////
// Called when the left mouse button is clicked in the grid window.  Stores
// the position of the cell that was clicked on to be used later in 
// OnMouseLeftUp.
// 
void Grid::OnMouseLeftDown( wxMouseEvent& event )
{
  event.Skip();

  // Convert the mouse coordinates to a location on the grid (take scrolling into account).
  wxPoint unscrolledPos;
  m_Grid->CalcUnscrolledPosition( event.GetX(), event.GetY(), &unscrolledPos.x, &unscrolledPos.y );
  m_Grid->XYToCell( unscrolledPos.x, unscrolledPos.y, m_MouseDownCell );
}

///////////////////////////////////////////////////////////////////////////////
// Called when the left mouse button is released over the grid window.  If the
// last mouse down event is in the same cell as this mouse up event, and that
// cell contains one of our checkbox fields, the state of the checkbox is 
// toggled.
// 
void Grid::OnMouseLeftUp( wxMouseEvent& event )
{
  event.Skip();

  // Convert the mouse coordinates to a location on the grid (take scrolling into account).
  wxPoint unscrolledPos;
  m_Grid->CalcUnscrolledPosition( event.GetX(), event.GetY(), &unscrolledPos.x, &unscrolledPos.y );
  wxGridCellCoords gridLoc;
  m_Grid->XYToCell( unscrolledPos.x, unscrolledPos.y, gridLoc );

  // If the event occurred over a cell, and it was the same cell that last
  // received the mouse down event...
  if ( gridLoc.GetRow() >= 0 && gridLoc.GetCol() >= 0 && gridLoc.GetRow() == m_MouseDownCell.GetRow() && gridLoc.GetCol() == m_MouseDownCell.GetCol() )
  {
    // If the cell in question is one of our bool (checkbox) cells...
    if ( gridLoc.GetCol() == m_ColumnViz || gridLoc.GetCol() == m_ColumnSel )
    {
      // Toggle the value in the cell
      bool value = m_Grid->GetCellValue( gridLoc.GetRow(), gridLoc.GetCol() ) == BOOL_TRUE;
      wxString newValue = value ? BOOL_FALSE : BOOL_TRUE;
      m_Grid->SetCellValue( gridLoc.GetRow(), gridLoc.GetCol(), newValue );

      // Notify any listeners that there has been a change to this row
      if ( gridLoc.GetCol() == m_ColumnViz )
      {
        m_RowVisibilityChanged.Raise( GridRowChangeArgs( gridLoc.GetRow() ) );
      }
      else
      {
        m_RowSelectabilityChanged.Raise( GridRowChangeArgs( gridLoc.GetRow() ) );
      }
    }
  }

  // Clear out the mouse down cell
  m_MouseDownCell.Set( -1, -1 );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a cell in the grid has changed.  Used for rename events.
// 
void Grid::OnCellChange( wxGridEvent& event )
{
  event.Skip();
  if ( !m_IsCellChanging && event.GetCol() == m_ColumnName && event.GetRow() >= 0 && event.GetRow() < static_cast< int32_t >( m_Names.size() ) )
  {
    // Protect this function from being re-entered
    m_IsCellChanging = true;

    // Determine the new name and old name
    const std::string newName ( m_Grid->GetCellValue( event.GetRow(), event.GetCol() ).c_str() );
    S_NaturalOrderString::const_iterator found = m_Names.begin();
    std::advance( found, event.GetRow() );
    const std::string oldName = *found;

    // Yuck... revert the cell back to the old name, then just let the listener of the
    // rename event handle the actual rename.
    m_Grid->BeginBatch();
    m_Grid->SetCellValue( event.GetRow(), event.GetCol(), oldName.c_str() );
    SelectRow( event.GetRow(), true ); // Maintain selection

    std::string errorMsg;
    if ( m_Names.find( newName ) != m_Names.end() )
    {
      errorMsg = TXT( "There is already an item with the name '" ) + newName + TXT( "'." );
    }
    else
    {
      // Let listeners handle rename event
      m_RowRenamed.Raise( GridRowRenamedArgs( event.GetRow(), oldName, newName ) );
    }

    int32_t newRow = GetRowNumber( newName );
    if ( newRow >= 0 && !m_Grid->IsVisible( newRow, m_ColumnName, false ) )
    {
      m_Grid->MakeCellVisible( newRow, m_ColumnName );
    }

    m_Grid->EndBatch();

    if ( !errorMsg.empty() )
    {
      wxMessageBox( errorMsg.c_str(), wxT( "Error" ), wxOK | wxCENTER | wxICON_ERROR, m_Grid );
    }

    m_IsCellChanging = false;
  }
}
