#include "ClipboardDataObject.h"
#include "Foundation/Reflect/ArchiveXML.h"
#include "Application/Inspect/DragDrop/ClipboardDataWrapper.h"
#include "Application/Inspect/DragDrop/ClipboardFileList.h"

using namespace Inspect;

// Unique identifier for this type of clipboard data.
static const tchar* s_Format = TXT( "Luna/ClipboardData" );

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ClipboardDataObject::ClipboardDataObject()
: wxCustomDataObject( s_Format )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ClipboardDataObject::~ClipboardDataObject()
{
}

///////////////////////////////////////////////////////////////////////////////
// All data is converted in the Luna clipboard format.  This class can also
// convert file name lists into Luna's format.
// 
void ClipboardDataObject::GetAllFormats( wxDataFormat* formats, wxDataObjectBase::Direction dir ) const
{
    if ( dir == wxDataObjectBase::Get )
    {
        *formats = GetFormat();
    }
    else
    {
        formats[0] = GetFormat();
        formats[1] = wxDataFormat( wxDF_FILENAME );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Returns the number of supported formats.
// 
size_t ClipboardDataObject::GetFormatCount( wxDataObjectBase::Direction dir ) const
{
    if ( dir ==  wxDataObjectBase::Get )
    {
        // We only provide data in the Luna format
        return 1;
    }

    // Data can be set from multiple formats.
    return 2;
}

///////////////////////////////////////////////////////////////////////////////
// The Luna clipboard format is preferred.
// 
wxDataFormat ClipboardDataObject::GetPreferredFormat( wxDataObjectBase::Direction dir ) const
{
    return GetFormat();
}

///////////////////////////////////////////////////////////////////////////////
// Required NOC_OVERRIDE to prevent function hiding.
// 
bool ClipboardDataObject::SetData( size_t size, const void* buf ) 
{ 
    return __super::SetData( size, buf ); 
}

///////////////////////////////////////////////////////////////////////////////
// Provides support for multiple clipboard formats.
// 
bool ClipboardDataObject::SetData( const wxDataFormat& format, size_t len, const void* buf )
{
    bool result = false;

    if ( format == GetFormat() )
    {
        // Luna clipboard format
        result = __super::SetData( format, len, buf );
    }
    else if ( format.IsStandard() && format.GetFormatId() == wxDF_FILENAME )
    {
        // File name list format.  Convert to our own type of filename list.
        wxFileDataObject fileData;
        fileData.SetData( len, buf );

        if ( fileData.GetFilenames().size() > 0 )
        {
            ClipboardFileListPtr fileList = new ClipboardFileList();

            wxArrayString::const_iterator fileItr = fileData.GetFilenames().begin();
            wxArrayString::const_iterator fileEnd = fileData.GetFilenames().end();
            for ( ; fileItr != fileEnd; ++fileItr )
            {
                const wxChar* file = fileItr->c_str();
                fileList->AddFilePath( file );
            }

            result = ToBuffer( fileList );
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Checks the internal buffer for data and pulls out the information there, 
// returning it in a smart pointer.
// 
ReflectClipboardDataPtr ClipboardDataObject::FromBuffer()
{
    ReflectClipboardDataPtr data;

    if ( GetDataSize() > 0 )
    {
        ClipboardDataWrapperPtr wrapper = Reflect::ObjectCast< ClipboardDataWrapper >( Reflect::ArchiveXML::FromString( (const tchar*)GetData(), Reflect::GetType< ClipboardDataWrapper >() ) );
        if ( wrapper.ReferencesObject() )
        {
            data = wrapper->m_Data;
        }
    }

    return data;
}

///////////////////////////////////////////////////////////////////////////////
// Takes the specified data object and serializes it into an internal buffer.
// This data can be fetched again by calling FromBuffer.
// 
bool ClipboardDataObject::ToBuffer( ReflectClipboardData* data )
{
    ClipboardDataWrapperPtr wrapper = new ClipboardDataWrapper();
    wrapper->m_Data = data;

    tstring xml;
    Reflect::ArchiveXML::ToString( wrapper, xml );

    return SetData( xml.size(), (const tchar*)( xml.c_str() ) );
}
