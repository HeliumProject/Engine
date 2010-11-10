#include "Platform/Exception.h"

#include "Directory.h"

#include "Platform/Assert.h"

using namespace Helium;

Directory::Directory()
: m_Done( true )
, m_Handle ( NULL )
{

}

Directory::Directory( const Path& path, const tstring& spec, uint32_t flags )
: m_Done( true )
, m_Handle ( NULL )
{
    Open( path, spec, flags );
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

    Find( m_Path + m_Spec );
}

bool Directory::Open( const Path& path, const tstring& spec, uint32_t flags )
{
    Close();

    m_Path = path;
    m_Spec = spec;
    m_Flags = flags;

    tstring query = m_Path.Get() + m_Spec;
 
    return Find( query );
}

void Directory::GetFiles( const Path& path, std::set< Helium::Path >& paths, const tstring& spec, bool recursive )
{
    for ( Directory dir( path, spec, DirectoryFlags::SkipDirectories ); !dir.IsDone(); dir.Next() )
    {
        const DirectoryItem& item = dir.GetItem();
        paths.insert( item.m_Path );
    }

    if ( recursive )
    {
        for ( Directory dir( path, TXT( "*" ), DirectoryFlags::SkipFiles ); !dir.IsDone(); dir.Next() )
        {
            GetFiles( dir.GetItem().m_Path, paths, spec, recursive );
        }
    }
}

void Directory::GetFiles( std::set< Helium::Path >& paths, const tstring& spec, bool recursive )
{
    GetFiles( m_Path, paths, spec, recursive );
}

bool Directory::Find( const tstring& query )
{
    FileFindData foundFile;
    m_Done = false;

    if ( !query.empty() )
    {
        if ( ( m_Handle = FindFirst( query.c_str(), foundFile ) ) == NULL )
        {
            m_Done = true;
            Close();
        }
    }
    else
    {
        HELIUM_ASSERT( m_Handle );
        if ( FindNext( m_Handle, foundFile ) == 0 )
        {      
            m_Done = true;
            Close();
        }
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
            if ( foundFile.m_FileAttributes & FileAttributes::Directory )
            {
                // SkipDirectories: skip directory files, also skip hidden/system directories, so we don't try to access "System Volume Information"
                if ( m_Flags & DirectoryFlags::SkipDirectories  
                    || foundFile.m_FileAttributes & FileAttributes::Hidden
                    || foundFile.m_FileAttributes & FileAttributes::System )
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
            m_Item.m_CreateTime = foundFile.m_CreationTime;
            m_Item.m_ModTime = foundFile.m_ModificationTime;
            m_Item.m_Size = foundFile.m_FileSize;

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
    if ( m_Handle != NULL )
    {
        bool result = CloseFind( m_Handle );
        HELIUM_ASSERT( result );
        m_Handle = NULL;
    }

    m_Done = true;
    m_Item.Clear(); 
}

void Helium::RecurseDirectories( DirectoryItemSignature::Delegate delegate, const tstring &path, const tstring &spec, uint32_t flags )
{
    // contents
    for ( Directory dir ( path, spec, flags ); !dir.IsDone(); dir.Next() )
    {
        delegate.Invoke( dir.GetItem() );
    }

    // recurse
    for ( Directory dir ( path, TXT( "*.*" ), DirectoryFlags::SkipFiles ); !dir.IsDone(); dir.Next() )
    {
        if ( dir.GetItem().m_Path.IsDirectory() )
        {
            RecurseDirectories( delegate, dir.GetItem().m_Path, spec, flags );
        }
    }
}