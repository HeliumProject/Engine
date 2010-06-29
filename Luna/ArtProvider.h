#pragma once

#include <wx/artprov.h>

namespace Luna
{
    // luna's art
#define lunaART_UNKNOWN wxART_MAKE_ART_ID( lunaART_UNKNOWN )

    class ArtProvider : public wxArtProvider
    {
    protected:
        virtual wxBitmap CreateBitmap( const wxArtID& artId, const wxArtClient& artClient, const wxSize& size );
    };
}