#include "Precompile.h"
#include "FileDropTarget.h"

#include "Foundation/String/Tokenize.h"
#include "Platform/Exception.h"

using namespace Helium;
using namespace Helium::Editor;

///////////////////////////////////////////////////////////////////////////////
FileDropTarget::FileDropTarget( const std::set< tstring >& extensions )
{
    m_FileExtensions = extensions;
}

///////////////////////////////////////////////////////////////////////////////
FileDropTarget::FileDropTarget( const tstring& extensions, const tstring& delims )
{
    Tokenize( extensions, m_FileExtensions, delims );
}

///////////////////////////////////////////////////////////////////////////////
// Allows you to set the callback for when a drag enters the target.
// 
//void FileDropTarget::AddDragEnterListener( const FileDragEnterSignature::Delegate& listener )
//{
//    if ( m_DragEnterEvent.Count() == 0 )
//    {
//        m_DragEnterEvent.Add( listener );
//    }
//    else
//    {
//        throw Helium::Exception( TXT( "Only one callback for 'drag enter' events is valid in FileDropTarget." ) );
//    }
//}

///////////////////////////////////////////////////////////////////////////////
// Allows you to set the callback for when a drag event over the target occurs.
// 
void FileDropTarget::AddDragOverListener( const FileDragOverSignature::Delegate& listener )
{
    if ( m_DragOverEvent.Count() == 0 )
    {
        m_DragOverEvent.Add( listener );
    }
    else
    {
        throw Helium::Exception( TXT( "Only one callback for 'drag over' events is valid in FileDropTarget." ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Allows you to set the callback for when a drag leaves the target.
// 
//void FileDropTarget::AddDragLeaveListener( const FileDragLeaveSignature::Delegate& listener )
//{
//    if ( m_DragLeaveEvent.Count() == 0 )
//    {
//        m_DragLeaveEvent.Add( listener );
//    }
//    else
//    {
//        throw Helium::Exception( TXT( "Only one callback for 'drag leave' events is valid in FileDropTarget." ) );
//    }
//}

///////////////////////////////////////////////////////////////////////////////
// Sets the callback to occur during a drop operation (can only be called once).
//
void FileDropTarget::AddDroppedListener( FileDroppedSignature::Delegate& listener )
{
    m_DroppedEvent.Add( listener );
}

///////////////////////////////////////////////////////////////////////////////
bool FileDropTarget::TestExtension( const tchar_t* testExt )
{
    if ( !m_FileExtensions.empty() )
    {
        for ( std::set< tstring >::const_iterator itr = m_FileExtensions.begin(), end = m_FileExtensions.end();
            itr != end; ++itr )
        {
            if ( ( _tcsicmp( testExt, itr->c_str() ) == 0 ) )
            {
                return true;
            }
        }

        return false;
    }

    // if there are no extensions always take the drop
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Notifies listener that a drag event has entered the target.
// 
//wxDragResult FileDropTarget::OnEnter( wxCoord x, wxCoord y, wxDragResult def )
//{
//    if ( m_DragEnterEvent.Count() > 0 )
//    {
//        GetData();
//
//        wxFileDataObject *fileDataObj = static_cast< wxFileDataObject* >( GetDataObject() );
//        const wxArrayString& filenames = fileDataObj->GetFilenames();
//
//        FileDroppedArgs args( (const wxChar*)filenames[ 0 ].c_str() ); //x, y, def );
//        if ( TestExtension( args.m_Path.Extension().c_str() ) )
//        {
//            m_DragEnterEvent.Raise( args );
//        }
//    }
//
//    return __super::OnEnter( x, y, def );
//}

///////////////////////////////////////////////////////////////////////////////
// Notifies listener that a drag over event has occurred.
// 
wxDragResult FileDropTarget::OnDragOver( wxCoord x, wxCoord y, wxDragResult def )
{
    wxDragResult result = def; //wxDragNone;
    //if ( m_DragOverEvent.Count() > 0 )
    {
        GetData();

#pragma TODO( "Get and return result OnDragOver" )
        //std::vector< wxDragResult > results( m_DragOverEvent.Count() );

        wxFileDataObject *fileDataObj = static_cast< wxFileDataObject* >( GetDataObject() );
        const wxArrayString& filenames = fileDataObj->GetFilenames();

        FileDroppedArgs args( (const wxChar*)filenames[ 0 ].c_str(), x, y, def );
        if ( TestExtension( args.m_Path.Extension().c_str() ) )
        {
            m_DragOverEvent.Raise( args ); //, &results.front(), (uint32_t)results.size() );
            //if ( results.size() > 0 )
            //{
            //  result = results.front();
            //}
            result = args.m_DragResult;
        }

    }
    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Notifies listener that a drop event has occurred.
// 
//wxDragResult FileDropTarget::OnData( wxCoord x, wxCoord y, wxDragResult def )
//{
//    if ( !GetData() )
//    {
//        return wxDragNone;
//    }
//
//    wxDragResult result = def;
//    if ( m_DropEvent.Count() > 0 )
//    {
//        GetData();
//
//#pragma TODO( "Get and return result OnData" )
//        //std::vector< wxDragResult > results( m_DropEvent.Count() );
//
//        wxFileDataObject *fileDataObj = static_cast< wxFileDataObject* >( GetDataObject() );
//        const wxArrayString& filenames = fileDataObj->GetFilenames();
//
//        FileDroppedArgs args( (const wxChar*)filenames[ 0 ].c_str() ); //x, y, def );
//        if ( TestExtension( args.m_Path.Extension().c_str() ) )
//        {
//            m_DropEvent.Raise( args ); //, &results.front(), (uint32_t)results.size() );
//            //if ( results.size() > 0 )
//            //{
//            //  result = results.front();
//            //}
//        }
//    }
//    return result;
//}

///////////////////////////////////////////////////////////////////////////////
bool FileDropTarget::OnDropFiles( wxCoord x, wxCoord y, const wxArrayString& filenames )
{
    if ( filenames.size() != 1 )
    {
        return false;
    }

    FileDroppedArgs args( (const wxChar*)filenames[ 0 ].c_str(), x, y );
    if ( TestExtension( args.m_Path.Extension().c_str() ) )
    {
        m_DroppedEvent.Raise( args );
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Notifies listener that the drag has left the target area.
// 
//void FileDropTarget::OnLeave()
//{
//    if ( m_DragLeaveEvent.Count() > 0 )
//    {
//        m_DragLeaveEvent.Raise( Helium::Void() );
//    }
//}
