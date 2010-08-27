#include "FileDialogButton.h"

#include "Application/Inspect/Controls/InspectCanvas.h"
#include "Foundation/Container/Insert.h"
#include "Application/UI/FileDialog.h"

using namespace Helium;
using namespace Helium::Inspect;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
FileDialogButton::FileDialogButton( const tstring& path )
: m_Title( TXT( "Open" ) )
{
    Helium::Insert<std::set< tstring >>::Result inserted = m_Filters.insert( std::set< tstring >::value_type( TXT( "All files (*.*)|*.*" ) ) );

    SetPath( path );
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void FileDialogButton::Realize( Inspect::Container* parent )
{
    PROFILE_SCOPE_ACCUM( g_RealizeAccumulator );

    if ( !IsRealized() )
    {
        __super::Realize( parent );

        if ( IsBound() )
        {
            tstring path;
            ReadPathData( path );
            m_Path.Set( path );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void FileDialogButton::Read()
{
    tstring path = GetPath();
}

///////////////////////////////////////////////////////////////////////////////
// Wrapper around Control::ReadStringData that cleans the file path returned
//
void FileDialogButton::ReadPathData( tstring& path ) const
{
    ReadStringData( path );
    if ( !path.empty() )
    {
        Helium::Path::Normalize( path );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Shows the file browser dialog.  If the user selects a file, the cleaned
// path is written back into the control.
// 
bool FileDialogButton::Write()
{
    bool result = true;

    // Show the file dialog and write the data back to the control.
    if ( IsBound() )
    {
        tstring filterStr = TXT( "All files (*.*)|*.*" );
        if ( !m_Filters.empty() )
        {
            filterStr = TXT( "" );
            std::set< tstring >::iterator it = m_Filters.begin();
            std::set< tstring >::iterator itEnd = m_Filters.end();
            for ( ; it != itEnd ; ++it )
            {
                filterStr += (*it);
                filterStr += TXT( "|" );
            }
            filterStr.erase( filterStr.size() - 1 );
        }

        FileDialogArgs args( FileDialogTypes::OpenFile, m_Title, filterStr, m_Path );
        m_Path = d_OpenFileBrowser.Invoke( args );
        result = WriteStringData( m_Path.Get() );
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void FileDialogButton::SetTitleBar( const tstring& title )
{
    m_Title = title;
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void FileDialogButton::SetFilter( const tstring& filter )
{
    m_Filters.clear();
    Helium::Insert<std::set< tstring >>::Result inserted = m_Filters.insert( std::set< tstring >::value_type( filter ) );
}


///////////////////////////////////////////////////////////////////////////////
// 
// 
void FileDialogButton::SetFilter( const std::vector< tstring >& filter )
{
    m_Filters.clear();

    std::vector< tstring >::const_iterator it = filter.begin();
    std::vector< tstring >::const_iterator itEnd = filter.end();
    for ( ; it != itEnd ; ++it )
    {
        Helium::Insert<std::set< tstring >>::Result inserted = m_Filters.insert( std::set< tstring >::value_type( *it ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void FileDialogButton::AddFilter( const tstring& filter )
{
    Helium::Insert<std::set< tstring >>::Result inserted = m_Filters.insert( std::set< tstring >::value_type( filter ) );
}


///////////////////////////////////////////////////////////////////////////////
// 
// 
tstring FileDialogButton::GetPath()
{
    if ( IsBound() )
    {
        tstring path;
        ReadPathData( path );
        m_Path.Set( path );
    }

    return m_Path.Get();
}

///////////////////////////////////////////////////////////////////////////////
// Sets the path that the file browser will initally start at.  Can be a full
// path to a file, or just a file.
// 
void FileDialogButton::SetPath( const tstring& path )
{
    m_Path.Set( path );
    if ( IsBound() )
    {
        WriteStringData( m_Path.Get() );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Called before the button is realized.  Stores the attributes specific to 
// this button.
// 
bool FileDialogButton::Process(const tstring& key, const tstring& value)
{
    bool wasHandled = false;

    if ( key == BUTTON_FILEDIALOG_ATTR_FILTER )
    {
        SetFilter( value );
        wasHandled = true;
    }
    else if ( key == BUTTON_FILEDIALOG_ATTR_PATH )
    {
        SetPath( value );
        wasHandled = true;
    }
    else if ( key == BUTTON_FILEDIALOG_ATTR_TITLE )
    {
        SetTitleBar( value );
        wasHandled = true;
    }
    else
    {
        wasHandled = __super::Process( key, value );
    }

    return wasHandled;
}
