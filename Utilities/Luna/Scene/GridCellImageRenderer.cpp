#include "Precompile.h"
#include "GridCellImageRenderer.h"
#include "UIToolKit/ImageManager.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
GridCellImageRenderer::GridCellImageRenderer()
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
GridCellImageRenderer::~GridCellImageRenderer()
{
}

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
    const wxBitmap& bmp = UIToolKit::GlobalImageManager().GetBitmap( imageName.c_str() );
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
    const wxBitmap& bmp = UIToolKit::GlobalImageManager().GetBitmap( imageName.c_str() );
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

