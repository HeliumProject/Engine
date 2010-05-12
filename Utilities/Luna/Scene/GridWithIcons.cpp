#include "Precompile.h"
#include "GridWithIcons.h"
#include "GridCellImageRenderer.h"
#include "UIToolKit/ImageManager.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// Adds a new column for icons and renumbers the base class's column numbers to
// compensate for it.
// 
GridWithIcons::GridWithIcons( wxWindow* parent, wxWindowID gridID, bool allowRename, bool showColLabels )
: Grid( parent, gridID, allowRename, showColLabels )
, m_ColumnIcon( Icon )
{
  m_Grid->BeginBatch();

  // Insert additional column into grid for icons (just before the name column)
  m_Grid->InsertCols( m_ColumnName );

  // Override base class's column numbers to compensate for the newly added column
  m_ColumnViz = Visibility;
  m_ColumnSel = Selectability;
  m_ColumnName = Name;

  // Setup the newly created Icon column
  m_Grid->SetColLabelValue( m_ColumnIcon, " " );
  m_Grid->SetColSize( m_ColumnIcon, 18 );
  wxGridCellAttr* attr = new wxGridCellAttr();
  attr->SetRenderer( new GridCellImageRenderer() );
  attr->SetEditor( new wxGridCellNumberEditor() );
  attr->SetReadOnly( true );
  m_Grid->SetColAttr( m_ColumnIcon, attr );

  // Apply the changes to the grid
  m_Grid->EndBatch();
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
GridWithIcons::~GridWithIcons()
{
}

///////////////////////////////////////////////////////////////////////////////
// Adds a row to the grid.
// 
bool GridWithIcons::AddRow( const std::string& name, bool visible, bool selectable, i32 imageIndex )
{
  bool isOk = false;
  if ( AddRow( name, visible, selectable ) )
  {
    // Find the row we just inserted
    i32 row = GetRowNumber( name );

    if ( row >= 0 )
    {
      const std::string& imageName = UIToolKit::GlobalImageManager().GetNameFromImageIndex( imageIndex );
      m_Grid->SetCellValue( row, Icon, wxT( imageName.c_str() ) );
      isOk = true;
    }
  }
  return isOk;
}

///////////////////////////////////////////////////////////////////////////////
// Overridden from the base class to make it protected.  Users of this class
// should use the AddRow function above.
// 
bool GridWithIcons::AddRow( const std::string& name, bool visible, bool selectable )
{
  return __super::AddRow( name, visible, selectable );
}

///////////////////////////////////////////////////////////////////////////////
// Overridden from the base class to take into account the extra row that was
// added.
// 
void GridWithIcons::ResizeColumns()
{
  wxSize gridSize = m_Grid->GetSize();
  wxSize gridWindowSize = m_Grid->GetGridWindow()->GetSize();
  bool isVScrollbarShowing = ( gridWindowSize.x != gridSize.x );
  const i32 scrollGutter = isVScrollbarShowing ? wxSystemSettings::GetMetric( wxSYS_VSCROLL_X ) : 0;
  const i32 startWidth = m_Grid->GetColSize( m_ColumnViz ) + m_Grid->GetColSize( m_ColumnSel ) + m_Grid->GetColSize( m_ColumnIcon );
  const i32 totalWidth = m_Panel->GetSize().x;
  const i32 pad = m_Grid->GetScrollLineX() + scrollGutter;
  const i32 width = totalWidth - startWidth - pad;
  m_Grid->SetColSize( m_ColumnName, width );
}
