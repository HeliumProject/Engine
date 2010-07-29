#include "FileBrowserButton.h"

#include "Application/Inspect/Controls/Canvas.h"
#include "Foundation/Container/Insert.h" 

// Using
using namespace Inspect;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
FileBrowserButton::FileBrowserButton( const tstring& startPath )
: m_Title( TXT( "Open" ) )
{
    SetIcon( TXT( "actions/system-search" ) );

    Helium::Insert<std::set< tstring >>::Result inserted = m_Filters.insert( std::set< tstring >::value_type( TXT( "All files (*.*)|*.*" ) ) );

    SetPath( startPath );
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void FileBrowserButton::Realize( Inspect::Container* parent )
{
    PROFILE_SCOPE_ACCUM( g_RealizeAccumulator );

    if ( !IsRealized() )
    {
        __super::Realize( parent );

        if ( IsBound() )
        {
            ReadPathData( m_Path );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void FileBrowserButton::Read()
{
    tstring path = GetPath();
}

///////////////////////////////////////////////////////////////////////////////
// Wrapper around Control::ReadData that cleans the file path returned
//
void FileBrowserButton::ReadPathData( tstring& path ) const
{
    ReadData( path );
    if ( !path.empty() )
    {
        Helium::Path::Normalize( path );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Shows the file browser dialog.  If the user selects a file, the cleaned
// path is written back into the control.
// 
bool FileBrowserButton::Write()
{
    // Let the base class do it's work first.
    bool result = __super::Write();

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

        wxWindow* parent = GetCanvas() ? GetCanvas()->GetControl() : NULL;

        HELIUM_BREAK();
#pragma TODO( "Reimplement to use the Vault" )
        //File::FileBrowser fileBrowser( parent, wxID_ANY, m_Title.c_str() );
        //fileBrowser.SetFilter( filterStr.c_str() );

        //if ( fileBrowser.ShowModal() == wxID_OK )
        //{
        //  tstring path = fileBrowser.GetPath().c_str();
        //  result = WriteData( path );
        //}
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void FileBrowserButton::SetTitleBar( const tstring& title )
{
    m_Title = title;
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void FileBrowserButton::SetFilter( const tstring& filter )
{
    m_Filters.clear();
    Helium::Insert<std::set< tstring >>::Result inserted = m_Filters.insert( std::set< tstring >::value_type( filter ) );
}


///////////////////////////////////////////////////////////////////////////////
// 
// 
void FileBrowserButton::SetFilter( const std::vector< tstring >& filter )
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
void FileBrowserButton::AddFilter( const tstring& filter )
{
    Helium::Insert<std::set< tstring >>::Result inserted = m_Filters.insert( std::set< tstring >::value_type( filter ) );
}


///////////////////////////////////////////////////////////////////////////////
// 
// 
tstring FileBrowserButton::GetPath()
{
    if ( IsBound() )
    {
        ReadPathData( m_Path );
    }

    return m_Path;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the path that the file browser will initally start at.  Can be a full
// path to a file, or just a file.
// 
void FileBrowserButton::SetPath( const tstring& path )
{
    m_Path = path;
    Helium::Path::Normalize( m_Path );
    if ( IsBound() )
    {
        WriteData( m_Path );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Called before the button is realized.  Stores the attributes specific to 
// this button.
// 
bool FileBrowserButton::Process(const tstring& key, const tstring& value)
{
    bool wasHandled = false;

    if ( key == BUTTON_FILEBROWSER_ATTR_FILTER )
    {
        SetFilter( value );
        wasHandled = true;
    }
    else if ( key == BUTTON_FILEBROWSER_ATTR_PATH )
    {
        SetPath( value );
        wasHandled = true;
    }
    else if ( key == BUTTON_FILEBROWSER_ATTR_TITLE )
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
