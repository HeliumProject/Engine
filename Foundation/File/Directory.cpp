#include "Platform/Exception.h"

#include "Directory.h"

#include "Platform/Assert.h"

using namespace Helium;

Directory::Directory()
: m_Done( true )
{

}

Directory::Directory( const Path& path, uint32_t flags )
: m_Done( true )
{
    Open( path, flags );
}

Directory::~Directory()
{
    Close();
}

bool Directory::IsDone()
{
    return m_Done;
}

bool Directory::Next()
{
    if ( m_Done )
    {
        return false;
    }

    return Find();
}

const DirectoryItem& Directory::GetItem()
{
    if ( m_Done )
    {
        throw Helium::Exception( TXT( "The file iterator is invalid!" ) );
    }

    return m_Item;
}

void Directory::Reset()
{
    Close();
    Find();
}

bool Directory::Open( const Path& path, uint32_t flags )
{
    Close();

    m_Path = path;
    m_Handle.m_Path = m_Path.Get();
    m_Flags = flags;

    return Find();
}

void Directory::GetFiles( const Path& path, std::set< Helium::Path >& paths, bool recursive )
{
    for ( Directory dir( path, DirectoryFlags::SkipDirectories ); !dir.IsDone(); dir.Next() )
    {
        const DirectoryItem& item = dir.GetItem();
        paths.insert( item.m_Path );
    }

    if ( recursive )
    {
        for ( Directory dir( path, DirectoryFlags::SkipFiles ); !dir.IsDone(); dir.Next() )
        {
            GetFiles( dir.GetItem().m_Path, paths, recursive );
        }
    }
}

void Directory::GetFiles( std::set< Helium::Path >& paths, bool recursive )
{
    GetFiles( m_Path, paths, recursive );
}

bool Directory::Find()
{
    FileFindData foundFile;
    m_Done = false;

    HELIUM_ASSERT( m_Handle.m_Path.length() > 0 );

    bool findResult =
        m_Handle.m_Handle == InvalidHandleValue()
        ? FindFirst( m_Handle, foundFile )
        : FindNext( m_Handle, foundFile );

    if ( findResult == 0 )
    {      
        m_Done = true;
        Close();
    }

    // while our current item isn't what we are looking for
    while( !m_Done )
    {
        bool ok = true;

        // skip relative path directories if fileName is "." or ".."
        if ( ( _tcscmp( foundFile.m_Filename.c_str(), TXT( "." ) ) == 0 ) || ( _tcscmp( foundFile.m_Filename.c_str(), TXT( ".." ) ) == 0 ) )
        {
            ok = false;
        }
        else
        {
            // directory...
            if ( foundFile.m_Stat.m_Mode & FileModeFlags::Directory )
            {
                // SkipDirectories: skip directory files, also skip hidden/system directories, so we don't try to access "System Volume Information"
                if ( m_Flags & DirectoryFlags::SkipDirectories  
                    || foundFile.m_Stat.m_Mode & FileModeFlags::Special )
                {
                    m_Item.Clear();
                    ok = false;
                }
            }
            // file...
            else
            {
                // SkipFiles: skip files
                if ( m_Flags & DirectoryFlags::SkipFiles )
                {
                    m_Item.Clear();
                    ok = false;
                }
            }
        }

        if ( ok )
        {
            // It's a keeper! store the data and format the file name
            // add the path path to the fileName
            if ( m_Flags & DirectoryFlags::RelativePath )
            {
                m_Item.m_Path.Set( foundFile.m_Filename );
            }
            else
            {
                m_Item.m_Path = m_Path;
                m_Item.m_Path += foundFile.m_Filename;
            }
            m_Item.m_CreateTime = foundFile.m_Stat.m_CreatedTime;
            m_Item.m_ModTime = foundFile.m_Stat.m_ModifiedTime;
            m_Item.m_Size = foundFile.m_Stat.m_Size;

            break;
        }
        else
        {
            m_Item.Clear();

            // this pumps the file iterator to the next element (into foundFile) for our next do...while loop
            if ( !FindNext( m_Handle, foundFile ) )
            {      
                m_Done = true;
                Close();
            }
        }
    }

    return !m_Done;
}

void Directory::Close()
{
    if ( m_Handle.m_Handle != InvalidHandleValue() )
    {
        bool result = CloseFind( m_Handle );
        HELIUM_ASSERT( result );
        if ( result )
        {
            m_Handle.m_Handle = InvalidHandleValue();
        }
    }

    m_Done = true;
    m_Item.Clear(); 
}

void Helium::RecurseDirectories( DirectoryItemSignature::Delegate delegate, const Path& path, uint32_t flags )
{
    // contents
    for ( Directory dir( path, flags ); !dir.IsDone(); dir.Next() )
    {
        delegate.Invoke( dir.GetItem() );
    }

    // recurse
    for ( Directory dir( path, DirectoryFlags::SkipFiles ); !dir.IsDone(); dir.Next() )
    {
        if ( dir.GetItem().m_Path.IsDirectory() )
        {
            RecurseDirectories( delegate, dir.GetItem().m_Path, flags );
        }
    }
}