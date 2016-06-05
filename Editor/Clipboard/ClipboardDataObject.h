#pragma once

#include "ReflectClipboardData.h"

#include <wx/dataobj.h>

namespace Helium
{
    namespace Editor
    {
        /////////////////////////////////////////////////////////////////////////////
        // Provides conversion from ReflectClipboardData to the wxWidgets clipboard
        // format.
        // 
        class ClipboardDataObject : public wxCustomDataObject
        {
        public:
            ClipboardDataObject();
            virtual ~ClipboardDataObject();

            // Base class overrides
            virtual void GetAllFormats( wxDataFormat* formats, wxDataObjectBase::Direction dir = wxDataObjectBase::Get ) const override;
            virtual size_t GetFormatCount( wxDataObjectBase::Direction dir = wxDataObjectBase::Get ) const override;
            virtual wxDataFormat GetPreferredFormat( wxDataObjectBase::Direction dir = wxDataObjectBase::Get ) const override;
            virtual bool SetData( size_t size, const void* buf ) override;
            virtual bool SetData( const wxDataFormat& format, size_t len, const void* buf ) override;

            // Member functions
            ReflectClipboardDataPtr FromBuffer();
            bool ToBuffer( ReflectClipboardData* data );
        };
    }
}