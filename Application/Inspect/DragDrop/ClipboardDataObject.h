#pragma once

#include "ReflectClipboardData.h"

#include <wx/dataobj.h>

namespace Inspect
{
  /////////////////////////////////////////////////////////////////////////////
  // Provides conversion from ReflectClipboardData to the wxWidgets clipboard
  // format.
  // 
  class APPLICATION_API ClipboardDataObject : public wxCustomDataObject
  {
  public:
    ClipboardDataObject();
    virtual ~ClipboardDataObject();
    
    // Base class overrides
    virtual void GetAllFormats( wxDataFormat* formats, wxDataObjectBase::Direction dir = wxDataObjectBase::Get ) const NOC_OVERRIDE;
    virtual size_t GetFormatCount( wxDataObjectBase::Direction dir = wxDataObjectBase::Get ) const NOC_OVERRIDE;
    virtual wxDataFormat GetPreferredFormat( wxDataObjectBase::Direction dir = wxDataObjectBase::Get ) const NOC_OVERRIDE;
    virtual bool SetData( size_t size, const void* buf ) NOC_OVERRIDE;
    virtual bool SetData( const wxDataFormat& format, size_t len, const void* buf ) NOC_OVERRIDE;

    // Member functions
    ReflectClipboardDataPtr FromBuffer();
    bool ToBuffer( ReflectClipboardData* data );
  };
}
