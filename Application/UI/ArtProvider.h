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

namespace Helium
{
    namespace ArtIDs
    {
        static const wxChar* Unknown = wxART_MAKE_ART_ID( HELIUM_ART_ID_UNKNOWN );
        static const wxChar* Null = wxART_MAKE_ART_ID( HELIUM_ART_ID_NULL );

        static const wxChar* File = wxART_MAKE_ART_ID( HELIUM_ART_ID_FILE );
        static const wxChar* Folder = wxART_MAKE_ART_ID( HELIUM_ART_ID_FOLDER );

        namespace Verbs
        {
            static const wxChar* Create = wxART_MAKE_ART_ID( HELIUM_ART_ID_VERB_CREATE );
            static const wxChar* Add = wxART_MAKE_ART_ID( HELIUM_ART_ID_VERB_ADD );
            static const wxChar* Delete = wxART_MAKE_ART_ID( HELIUM_ART_ID_VERB_DELETE );
            static const wxChar* Select = wxART_MAKE_ART_ID( HELIUM_ART_ID_VERB_SELECT );
            static const wxChar* Refresh = wxART_MAKE_ART_ID( HELIUM_ART_ID_VERB_REFRESH );
            static const wxChar* Find = wxART_MAKE_ART_ID( HELIUM_ART_ID_VERB_FIND );
        }

        namespace Statuses
        {
            static const wxChar* Lockable = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_LOCKABLE );
            static const wxChar* Locked = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_LOCKED );
            static const wxChar* Busy = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_BUSY );
            static const wxChar* Information = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_INFORMATION );
            static const wxChar* Warning = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_WARNING );
            static const wxChar* Error = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_ERROR );
            static const wxChar* Binary = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_BINARY );
            static const wxChar* Text = wxART_MAKE_ART_ID( HELIUM_ART_ID_STATUS_TEXT );
        }

        static const wxChar* Open = wxART_MAKE_ART_ID( HELIUM_ART_ID_OPEN );
        static const wxChar* Save = wxART_MAKE_ART_ID( HELIUM_ART_ID_SAVE );
        static const wxChar* SaveAs = wxART_MAKE_ART_ID( HELIUM_ART_ID_SAVE_AS );
        static const wxChar* SaveAll = wxART_MAKE_ART_ID( HELIUM_ART_ID_SAVE_ALL );

        static const wxChar* SelectAll = wxART_MAKE_ART_ID( HELIUM_ART_ID_SELECT_ALL );

        static const wxChar* Cut = wxART_MAKE_ART_ID( HELIUM_ART_ID_CUT );
        static const wxChar* Copy = wxART_MAKE_ART_ID( HELIUM_ART_ID_COPY );
        static const wxChar* Paste = wxART_MAKE_ART_ID( HELIUM_ART_ID_PASTE );

        static const wxChar* Create = wxART_MAKE_ART_ID( HELIUM_ART_ID_CREATE );
        static const wxChar* Add = wxART_MAKE_ART_ID( HELIUM_ART_ID_ADD );
        static const wxChar* Delete = wxART_MAKE_ART_ID( HELIUM_ART_ID_DELETE );

        static const wxChar* Undo = wxART_MAKE_ART_ID( HELIUM_ART_ID_UNDO );
        static const wxChar* Redo = wxART_MAKE_ART_ID( HELIUM_ART_ID_REDO );

        static const wxChar* Options = wxART_MAKE_ART_ID( HELIUM_ART_ID_OPTIONS );
        static const wxChar* Preferences = wxART_MAKE_ART_ID( HELIUM_ART_ID_PREFERENCES );

        static const wxChar* Find = wxART_MAKE_ART_ID( HELIUM_ART_ID_FIND );

        namespace RevisionControl
        {
            static const wxChar* Unknown = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_UNKNOWN );
            static const wxChar* Active = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_ACTIVE );
            static const wxChar* CheckedOutByMe = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_CHECKED_OUT_BY_ME );
            static const wxChar* CheckedOutBySomeoneElse = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_CHECKED_OUT_BY_SOMEONE_ELSE );
            static const wxChar* OutOfDate = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_OUTOFDATE );
            static const wxChar* CheckOut = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_CHECKOUT );
            static const wxChar* CheckIn = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_CHECKIN );
            static const wxChar* Submit = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_SUBMIT );
            static const wxChar* Revert = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_REVERT );
            static const wxChar* Merge = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_MERGE );
            static const wxChar* History = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_HISTORY );
            static const wxChar* File = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_FILE );
            static const wxChar* Changelist = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_CHANGELIST );
            static const wxChar* CreateChangelist = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_CREATECHANGELIST );
            static const wxChar* User = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_USER );

            static const wxChar* RevisionControlled = wxART_MAKE_ART_ID( HELIUM_ART_ID_RCS_CONTROLLED );
        }
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
        virtual wxBitmap CreateBitmap( const wxArtID& artId, const wxArtClient& artClient, const wxSize& size ) HELIUM_OVERRIDE;
    
    protected:
        // delayed initialization
        virtual void Create();

    protected:
        typedef std::map< wxArtID, tstring > M_ArtIDToFilename;
        M_ArtIDToFilename m_ArtIDToFilename;
        
        ArtProviderCache *m_ArtProviderCache;    
    };

}