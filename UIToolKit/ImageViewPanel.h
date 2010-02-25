#pragma once

#include "API.h"

#include <wx/panel.h>
#include <wx/bitmap.h>

namespace Nocturnal
{
  namespace ImageViewPanelFlags
  {
    enum ImageViewPanelFlag
    {
      RenderChannelRed   = 1 << 0,
      RenderChannelGreen = 1 << 1,
      RenderChannelBlue  = 1 << 2,

      Default = RenderChannelRed | RenderChannelGreen | RenderChannelBlue

    };
  }
  typedef ImageViewPanelFlags::ImageViewPanelFlag ImageViewPanelFlag;

  class UITOOLKIT_API ImageViewPanel : public wxPanel
  {
  public:
    ImageViewPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = "ImageViewPanel" );
    virtual ~ImageViewPanel();

    void SetImage( const std::string& path, const ImageViewPanelFlag flags = ImageViewPanelFlags::Default );
    void ClearImage();

  private:
    void OnPaint( wxPaintEvent& event );
    void OnEraseBackground( wxEraseEvent& event );

  private:
    wxBitmap m_Bitmap;
  };
}
