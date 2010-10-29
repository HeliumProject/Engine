#include "Precompile.h"
#include "GridWithIcons.h"
#include "Editor/ArtProvider.h"

using namespace Helium;
using namespace Helium::Editor;

/////////////////////////////////////////////////////////////////////////////
// Class for rendering a bitmap within a grid cell.  The name of the bitmap
// to draw should be stored as a string in the cell.
// 
class GridCellImageRenderer : public wxGridCellRenderer
{
public:
    virtual void Draw( wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, const wxRect& rect, int row, int col, bool isSelected ) HELIUM_OVERRIDE;
    virtual wxSize GetBestSize(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, int row, int col ) HELIUM_OVERRIDE;
    virtual wxGridCellRenderer* Clone() const HELIUM_OVERRIDE;
};

///////////////////////////////////////////////////////////////////////////////
// Attempts to draw an image at the specified grid location.  The name of the
// image to draw is suppose to be stored in the cell.  If an image is able to
// be located for the cell, it is drawn.
// 
void GridCellImageRenderer::Draw( wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, const wxRect& rect, int row, int col, bool isSelected )
{
  // Let the base class fill in the background color for us.
  __super::Draw( grid, attr, dc, rect, row, col, isSelected );

  // Try to get the name of the bitmap from the cell.
  const wxString& imageName = grid.GetCellValue( row, col );
  if ( !imageName.IsEmpty() )
  {
    // Draw the bitmap in the cell.
    const wxBitmap& bmp = wxArtProvider::GetBitmap( imageName.c_str() );
    dc.DrawBitmap( bmp, rect.GetLeftTop(), true );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Returns the best size for the cell by determining which bitmap will be drawn
// in the cell and return its dimensions plus some padding.
// 
wxSize GridCellImageRenderer::GetBestSize(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, int row, int col )
{
  // Initialize the size to a default value, and update the value if we find an
  // image for the specified cell.
  wxSize size( 18, 18 ); 

  // Try to get the name of the bitmap from the cell.
  const wxString& imageName = grid.GetCellValue( row, col );
  if ( !imageName.IsEmpty() )
  {
    const wxBitmap& bmp = wxArtProvider::GetBitmap( imageName.c_str() );
    if ( !bmp.IsNull() )
    {
      // Return the bitmap's dimensions plus some pad
      int pad = 2;
      size.x = bmp.GetWidth() + pad;
      size.y = bmp.GetHeight() + pad;
    }
  }

  return size;
}

///////////////////////////////////////////////////////////////////////////////
// Returns a new copy of this renderer.
// 
wxGridCellRenderer* GridCellImageRenderer::Clone() const
{
  return ( new GridCellImageRenderer() );
}

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
  m_Grid->SetColLabelValue( m_ColumnIcon, TXT( " " ) );
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
bool GridWithIcons::AddRow( const tstring& name, bool visible, bool selectable, int32_t imageIndex )
{
  bool isOk = false;
  if ( AddRow( name, visible, selectable ) )
  {
    // Find the row we just inserted
    int32_t row = GetRowNumber( name );

    if ( row >= 0 )
    {
      //const tstring& imageName = Helium::GlobalImageManager().GetNameFromImageIndex( imageIndex );
      //m_Grid->SetCellValue( row, Icon, imageName.c_str() );
      isOk = true;
    }
  }
  return isOk;
}

///////////////////////////////////////////////////////////////////////////////
// Overridden from the base class to make it protected.  Users of this class
// should use the AddRow function above.
// 
bool GridWithIcons::AddRow( const tstring& name, bool visible, bool selectable )
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
  const int32_t scrollGutter = isVScrollbarShowing ? wxSystemSettings::GetMetric( wxSYS_VSCROLL_X ) : 0;
  const int32_t startWidth = m_Grid->GetColSize( m_ColumnViz ) + m_Grid->GetColSize( m_ColumnSel ) + m_Grid->GetColSize( m_ColumnIcon );
  const int32_t totalWidth = m_Panel->GetSize().x;
  const int32_t pad = m_Grid->GetScrollLineX() + scrollGutter;
  const int32_t width = totalWidth - startWidth - pad;
  m_Grid->SetColSize( m_ColumnName, width );
}
