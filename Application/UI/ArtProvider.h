#pragma once

#include "Application/API.h"

#include <map>

#include <wx/animate.h>
#include <wx/artprov.h>
#include <wx/imaglist.h>

#include "FileIconsTable.h"
#include "Foundation/File/Path.h"
#include "Platform/Compiler.h"
#include "Platform/Types.h"

namespace Nocturnal
{
    // Nocturnal's custom art IDs
    namespace ArtIDs
    {
        static const wxChar* Unknown = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_UNKNOWN );

        static const wxChar* PerspectiveCamera = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_PERSPECTIVE_CAMERA );
        static const wxChar* FrontOrthoCamera = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_FRONT_ORTHO_CAMERA );
        static const wxChar* SideOrthoCamera = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_SIDE_ORTHO_CAMERA );
        static const wxChar* TopOrthoCamera = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_TOP_ORTHO_CAMERA );

        static const wxChar* Select = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_SELECT );
        static const wxChar* Translate = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_TRANSLATE );
        static const wxChar* Rotate = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_ROTATE );
        static const wxChar* Scale = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_SCALE );
        static const wxChar* Duplicate = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_DUPLICATE );
    }

    /////////////////////////////////////////////////////////////////////////////
    class APPLICATION_API ArtProvider : public wxArtProvider
    {
    public:
        static wxSize DefaultImageSize;

        ArtProvider();
        virtual ~ArtProvider();

    protected:
        virtual wxBitmap CreateBitmap( const wxArtID& artId, const wxArtClient& artClient, const wxSize& size ) NOC_OVERRIDE;
    
    private:
        // delayed initialization
        void Create();

    private:
        typedef std::map< wxArtID, tstring > M_ArtIDToFilename;
        M_ArtIDToFilename m_ArtIDToFilename;
        
        wxHashTable* m_HashTable;
    };

}