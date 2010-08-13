#pragma once

#include "Editor/API.h"
#include "Platform/Types.h"

#include <wx/panel.h>
#include <wx/bitmap.h>

namespace Helium
{
    namespace Editor
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

        class EDITOR_API ImageViewPanel : public wxPanel
        {
        public:
            ImageViewPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT( "ImageViewPanel" ) );
            virtual ~ImageViewPanel();

            void SetImage( const tstring& path, const ImageViewPanelFlag flags = ImageViewPanelFlags::Default );
            void ClearImage();

        private:
            void OnPaint( wxPaintEvent& event );
            void OnEraseBackground( wxEraseEvent& event );

        private:
            wxBitmap m_Bitmap;
        };
    }
}