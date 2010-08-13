#include "FilteredDropTarget.h"

#include "Foundation/String/Wildcard.h"
#include "Foundation/String/Tokenize.h"
#include "Application/Inspect/Clipboard/ClipboardDataObject.h"
#include "Application/Inspect/Clipboard/ClipboardFileList.h"

using namespace Helium;
using namespace Helium::Inspect;

FilteredDropTarget::FilteredDropTarget( const tstring& filter ) 
: Inspect::DropTarget()
, m_FileFilter( filter )
{
    SetDragOverCallback( Inspect::DragOverCallback::Delegate( this, &FilteredDropTarget::DragOver ) );
    SetDropCallback( Inspect::DropCallback::Delegate( this, &FilteredDropTarget::Drop ) );
}

FilteredDropTarget::~FilteredDropTarget()
{
}

bool FilteredDropTarget::ValidateDrag( const Inspect::DragArgs& args )
{
    Inspect::ClipboardFileListPtr fileList = Reflect::ObjectCast< Inspect::ClipboardFileList >( args.m_ClipboardData->FromBuffer() );
    if ( !fileList )
    {
        return false;
    }

    if ( !m_FileFilter.empty() )
    {
        return true;
    }

    std::vector< tstring > extensions;
    Tokenize( m_FileFilter, extensions, TXT( ";" ) );
    if ( extensions.empty() )
    {
        return true;
    }

    for ( std::set< tstring >::const_iterator fileItr = fileList->GetFilePaths().begin(), fileEnd = fileList->GetFilePaths().end(); fileItr != fileEnd; ++fileItr )
    {
        Helium::Path path( *fileItr );
        if ( path.Get().empty() )
        {
            continue;
        }

        if ( !path.Exists() )
        {
            continue;
        }

        for ( std::vector< tstring >::const_iterator itr = extensions.begin(), end = extensions.end(); itr != end; ++itr )
        {
            const tstring& extension = *itr;
            if ( WildcardMatch( extension.c_str(), path.c_str() ) )
            {
                return true;
            }
        }
    }

    return false;
}

wxDragResult FilteredDropTarget::DragOver( const Inspect::DragArgs& args )
{
    if ( ValidateDrag( args ) )
    {
        return args.m_Default;
    }

    return wxDragNone;
}

wxDragResult FilteredDropTarget::Drop( const Inspect::DragArgs& args )
{
    std::vector< tstring > validPaths;

    if ( !ValidateDrag( args ) )
    {
        return args.m_Default;
    }

    Inspect::ClipboardFileListPtr fileList = Reflect::ObjectCast< Inspect::ClipboardFileList >( args.m_ClipboardData->FromBuffer() );
    if ( !fileList )
    {
        return args.m_Default;
    }

    std::vector< tstring > extensions;
    if ( !m_FileFilter.empty() )
    {
        Tokenize( m_FileFilter, extensions, TXT( ";" ) );
    }

    for ( std::set< tstring >::const_iterator fileItr = fileList->GetFilePaths().begin(), fileEnd = fileList->GetFilePaths().end(); fileItr != fileEnd; ++fileItr )
    {
        Helium::Path path( *fileItr );
        if ( path.Get().empty() )
        {
            continue;
        }

        if ( !path.Exists() )
        {
            continue;
        }

        if ( extensions.empty() )
        {
            validPaths.push_back( path.Get() );
        }
        else
        {
            for ( std::vector< tstring >::const_iterator itr = extensions.begin(), end = extensions.end(); itr != end; ++itr )
            {
                const tstring& extension = *itr;
                if ( WildcardMatch( extension.c_str(), path.c_str() ) )
                {
                    validPaths.push_back( path );
                    break;
                }
            }
        }
    }

    if ( validPaths.size() )
    {
        m_Dropped.Raise( FilteredDropTargetArgs( validPaths ) );
    }

    return args.m_Default;
}

void FilteredDropTarget::AddDroppedListener( const FilteredDropTargetSignature::Delegate& d )
{
    m_Dropped.Add( d );
}

void FilteredDropTarget::RemoveDroppedListener( const FilteredDropTargetSignature::Delegate& d )
{
    m_Dropped.Remove( d );
}
