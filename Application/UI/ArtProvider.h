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
#define NOCTURNAL_UNKNOWN_ART_ID wxART_MAKE_ART_ID( NOCTURNAL_UNKNOWN_ART_ID )

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