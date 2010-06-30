#pragma once

#include "Application/API.h"

#include <wx/artprov.h>

namespace Nocturnal
{
    // Nocturnal's custom art IDs
#define NOCTURNAL_UNKNOWN_ART_ID wxART_MAKE_ART_ID( NOCTURNAL_UNKNOWN_ART_ID )


    class APPLICATION_API ArtProvider : public wxArtProvider
    {
    protected:
        virtual wxBitmap CreateBitmap( const wxArtID& artId, const wxArtClient& artClient, const wxSize& size );
    };
}