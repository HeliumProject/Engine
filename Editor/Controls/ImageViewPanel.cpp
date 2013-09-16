#include "EditorPch.h"
#include "ImageViewPanel.h"

#include <wx/image.h>
#include <wx/dcclient.h>
#include <wx/dcmemory.h>

using namespace Helium;
using namespace Helium::Editor;

static void DrawBitmap( wxDC& dc, const wxBitmap& bitmap );

ImageViewPanel::ImageViewPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: wxPanel( parent, id, pos, size, style, name )
{
  Connect( wxEVT_PAINT, wxPaintEventHandler( ImageViewPanel::OnPaint ) );
  Connect( wxEVT_ERASE_BACKGROUND, wxEraseEventHandler( ImageViewPanel::OnEraseBackground ) );
}

ImageViewPanel::~ImageViewPanel()
{
  Disconnect( wxEVT_PAINT, wxPaintEventHandler( ImageViewPanel::OnPaint ) );
  Disconnect( wxEVT_ERASE_BACKGROUND, wxEraseEventHandler( ImageViewPanel::OnEraseBackground ) );
}

void ImageViewPanel::SetImage( const std::string& path, const ImageViewPanelFlag flags )
{
  m_Bitmap.LoadFile( path, wxBITMAP_TYPE_ANY );

  // determine how to scale the image
  float ratioHToW = ( (float) m_Bitmap.GetHeight() / (float) m_Bitmap.GetWidth() );

  int width, height;
  if ( ratioHToW >= 1.0 )
  {
    ratioHToW = 1.0 / ratioHToW;
    height = GetSize().GetWidth();
    width  = (int) ( ratioHToW * (float) GetSize().GetWidth() );
  }
  else
  {
    width = GetSize().GetWidth();
    height  = (int) ( ratioHToW * (float) GetSize().GetWidth() );
  }

  // store the scaled image
  wxImage image = m_Bitmap.ConvertToImage();
  image.Rescale( width, height );

  m_Bitmap = wxBitmap( image );

  if ( ( flags & ImageViewPanelFlags::Default ) != ImageViewPanelFlags::Default )
  {
    wxImage newImage = m_Bitmap.ConvertToImage();

    for( int x = 0; x < newImage.GetWidth(); ++x )
    {
      for( int y = 0; y < newImage.GetHeight(); ++y )
      {
        unsigned char r = 0, g = 0, b = 0;
        
        if ( flags & ImageViewPanelFlags::RenderChannelRed )
        {
          r = newImage.GetRed( x, y );
        }

        if ( flags & ImageViewPanelFlags::RenderChannelGreen )
        {
          g = newImage.GetGreen( x, y );
        }

        if ( flags & ImageViewPanelFlags::RenderChannelBlue )
        {
          b = newImage.GetBlue( x, y );
        }

        newImage.SetRGB( x, y, r, g, b );
      }
    }

    wxBitmap tempBitmap( newImage );
    m_Bitmap = tempBitmap;
  }

  Refresh();
}

void ImageViewPanel::ClearImage()
{
#if HELIUM_OS_WIN
  m_Bitmap.FreeResource();
#else
  HELIUM_ASSERT( false );
#endif

  Refresh();
}

void ImageViewPanel::OnPaint( wxPaintEvent& event )
{
  wxPaintDC dc( this );
  if ( m_Bitmap.Ok() )
  {
    DrawBitmap( dc, m_Bitmap );
  }
  else
  {
    wxMemoryDC memDC;
    wxBitmap tempBitmap( this->GetSize().GetWidth(), this->GetSize().GetHeight() );
    memDC.SelectObject( tempBitmap );
    memDC.SetBackground( *wxWHITE_BRUSH );
    memDC.Clear();
    memDC.SetPen( *wxRED_PEN );
    memDC.SetBrush( *wxTRANSPARENT_BRUSH );
    memDC.DrawRectangle( wxRect( 1, 1, this->GetSize().GetWidth() - 1, this->GetSize().GetHeight() - 1 ) );
    memDC.SelectObject( wxNullBitmap );
    DrawBitmap( dc, tempBitmap );
  }
}

void ImageViewPanel::OnEraseBackground( wxEraseEvent& event )
{
  if ( event.GetDC() )
  {
    if ( m_Bitmap.Ok() )
    {
      DrawBitmap( *event.GetDC(), m_Bitmap );
    }
    else
    {
      event.GetDC()->DrawRectangle( this->GetClientRect() );
    }
  }
  else
  {
    wxPaintDC dc( this );
    if ( m_Bitmap.Ok() )
    {
      DrawBitmap( dc, m_Bitmap );
    }
    else
    {
      dc.DrawRectangle( this->GetClientRect() );
    }
  }
}

static void DrawBitmap( wxDC& dc, const wxBitmap& bitmap )
{
  wxMemoryDC dcMem;
  dcMem.SelectObjectAsSource( bitmap );
  dc.Blit( 0, 0, bitmap.GetWidth(), bitmap.GetHeight(), &dcMem, 0, 0 );
  dcMem.SelectObject( wxNullBitmap );
}
