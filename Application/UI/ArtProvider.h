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
        static const wxChar* Null = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_NULL );

        static const wxChar* PerspectiveCamera = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_PERSPECTIVE_CAMERA );
        static const wxChar* FrontOrthoCamera = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_FRONT_ORTHO_CAMERA );
        static const wxChar* SideOrthoCamera = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_SIDE_ORTHO_CAMERA );
        static const wxChar* TopOrthoCamera = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_TOP_ORTHO_CAMERA );

        static const wxChar* Select = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_SELECT );
        static const wxChar* Translate = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_TRANSLATE );
        static const wxChar* Rotate = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_ROTATE );
        static const wxChar* Scale = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_SCALE );
        static const wxChar* Duplicate = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_DUPLICATE );

        static const wxChar* Locator = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_LOCATOR );
        static const wxChar* Volume = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_VOLUME );
        static const wxChar* Entity = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_ENTITY );
        static const wxChar* Curve = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_CURVE );
        static const wxChar* CurveEdit = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_CURVEEDIT );
        static const wxChar* NavMesh = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_NAVMESH );

        static const wxChar* NewCollection = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_NEWCOLLECTION );

        static const wxChar* RCSCheckedOutByMe = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_RCS_CHECKED_OUT_BY_ME );
        static const wxChar* RCSCheckedOutBySomeoneElse = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_RCS_CHECKED_OUT_BY_SOMEONE_ELSE );
        static const wxChar* RCSOutOfDate = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_RCSOUTOFDATE );
        static const wxChar* RCSUnknown = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_RCSUNKNOWN );
        
        static const wxChar* TaskWindow = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_TASKWINDOW );
        static const wxChar* SceneEditor = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_SCENEEDITOR );

    }

    /////////////////////////////////////////////////////////////////////////////
    class ArtProviderCache;

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
        
        ArtProviderCache *m_ArtProviderCache;    
    };

}