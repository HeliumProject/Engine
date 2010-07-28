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
    namespace ArtIDs
    {
        static const wxChar* Unknown = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_UNKNOWN );
        static const wxChar* Null = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_NULL );

        static const wxChar* Open = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_OPEN );
        static const wxChar* Save = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_SAVE );
        static const wxChar* SaveAs = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_SAVE_AS );
        static const wxChar* SaveAll = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_SAVE_ALL );

        static const wxChar* SelectAll = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_SELECT_ALL );

        static const wxChar* Cut = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_CUT );
        static const wxChar* Copy = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_COPY );
        static const wxChar* Paste = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_PASTE );
        static const wxChar* Delete = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_DELETE );

        static const wxChar* Undo = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_UNDO );
        static const wxChar* Redo = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_REDO );

        static const wxChar* Back = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_BACK );
        static const wxChar* Forward = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_FORWARD );
        
        static const wxChar* Options = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_OPTIONS );

        static const wxChar* RCSCheckedOutByMe = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_RCS_CHECKED_OUT_BY_ME );
        static const wxChar* RCSCheckedOutBySomeoneElse = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_RCS_CHECKED_OUT_BY_SOMEONE_ELSE );
        static const wxChar* RCSOutOfDate = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_RCSOUTOFDATE );
        static const wxChar* RCSUnknown = wxART_MAKE_ART_ID( NOCTURNAL_ART_ID_RCSUNKNOWN );
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
    
    protected:
        // delayed initialization
        virtual void Create();

    protected:
        typedef std::map< wxArtID, tstring > M_ArtIDToFilename;
        M_ArtIDToFilename m_ArtIDToFilename;
        
        ArtProviderCache *m_ArtProviderCache;    
    };

}