#include "Platform/Windows/Windows.h"
#include "Foundation/Exception.h"

#include "Directory.h"

#include "Platform/Assert.h"

using namespace Nocturnal;

Directory::Directory()
: m_Done( true )
, m_Handle ( INVALID_HANDLE_VALUE )
{

}

Directory::Directory(const std::string &path, const std::string &spec, u32 flags)
: m_Done( true )
, m_Handle ( INVALID_HANDLE_VALUE )
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
        throw Nocturnal::Exception("The file iterator is invalid!");
    }

    return Find();
}

const DirectoryItem& Directory::GetItem()
{
    if ( m_Done )
    {
        throw Nocturnal::Exception("The file iterator is invalid!");
    }

    return m_Item;
}

void Directory::Reset()
{
    Close();

    Find(m_Path + m_Spec);
}

bool Directory::Open(const std::string &path, const std::string &spec /* = "" */, u32 flags /* = DirectoryFlags::Default */)
{
    Close();

    // Clean the path string and append a trailing slash.
    m_Path = path;
    m_Spec = spec;
    m_Flags = flags;

    std::string query = m_Path + m_Spec;

    // check that the input is not larger than allowed
    if ( query.size() > MAX_PATH )
    {
        throw Nocturnal::Exception( "Query string is too long (max buffer length is %d): %s", ( int ) MAX_PATH, query.c_str() );
    }

    return Find(query);
}

void Directory::GetFiles( const std::string& path, std::set< Nocturnal::Path >& paths, const std::string& spec, bool recursive )
{
    for ( Directory dir( path, spec, DirectoryFlags::SkipDirectories ); !dir.IsDone(); dir.Next() )
    {
        const DirectoryItem& item = dir.GetItem();
        paths.insert( Nocturnal::Path( item.m_Path ) );
    }

    if ( recursive )
    {
        for ( Directory dir ( path, "*", DirectoryFlags::SkipFiles ); !dir.IsDone(); dir.Next() )
        {
            GetFiles( dir.GetItem().m_Path, paths, spec, recursive );
        }
    }
}

void Directory::GetFiles( std::set< Nocturnal::Path >& paths, const std::string& spec, bool recursive )
{
    GetFiles( m_Path, paths, spec, recursive );
}

bool Directory::Find(const std::string& query)
{
    DWORD error = 0x0;
    WIN32_FIND_DATA foundFile;

    m_Done = false;

    if (!query.empty())
    {
        if ( (m_Handle = ::FindFirstFile( query.c_str(), &foundFile )) == INVALID_HANDLE_VALUE )
        {
            m_Done = true;

            Close();

            if ( (error = GetLastError()) != ERROR_FILE_NOT_FOUND && error != ERROR_PATH_NOT_FOUND ) 
            {
                throw PlatformException( error, "Error calling FindFirstFile" );
            }
        }
    }
    else
    {
        if ( ::FindNextFile( m_Handle, &foundFile ) == 0 )
        {      
            m_Done = true;

            Close();

            if ( (error = GetLastError()) != ERROR_NO_MORE_FILES ) 
            {
                throw PlatformException( error, "Error calling FindNextFile" );
            }
        }
    }

    // while our current item isn't what we are looking for
    while (!m_Done)
    {
        bool ok = true;

        // skip relative path directories if fileName is "." or ".."
        if ( ( std::strcmp( foundFile.cFileName , "." ) == 0 ) || ( std::strcmp( foundFile.cFileName , ".." ) == 0 ) )
        {
            ok = false;
        }
        else
        {
            // directory...
            if ( foundFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
            {
                // SkipDirectories: skip directory files, also skip hidden/system directories, so we don't try to access "System Volume Information"
                if ( m_Flags & DirectoryFlags::SkipDirectories  
                    || foundFile.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN
                    || foundFile.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM )
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

        if (ok)
        {
            // It's a keeper! store the data and format the file name
            m_Item.m_Path = foundFile.cFileName;
            m_Item.m_CreateTime = foundFile.ftCreationTime.dwHighDateTime;
            m_Item.m_CreateTime = m_Item.m_CreateTime << 32;
            m_Item.m_CreateTime |= foundFile.ftCreationTime.dwLowDateTime;
            m_Item.m_ModTime = foundFile.ftLastWriteTime.dwHighDateTime;
            m_Item.m_ModTime = m_Item.m_ModTime << 32;
            m_Item.m_ModTime |= foundFile.ftLastWriteTime.dwLowDateTime;
            m_Item.m_Size = foundFile.nFileSizeHigh;
            m_Item.m_Size = m_Item.m_Size << 32;
            m_Item.m_Size |= foundFile.nFileSizeLow;

            // flag this item as a directory if it is one
            if ( foundFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
            {
                m_Item.m_Flags |= DirectoryItemFlags::Directory;
                m_Item.m_Path += "/";
            }

            // add the path path to the fileName
            if ( !(m_Flags & DirectoryFlags::RelativePath) )
            {
                m_Item.m_Path.insert( 0, m_Path );
            }

            break;
        }
        else
        {
            m_Item.Clear();

            // this pumps the windows file iterator to the next element (into foundFile) for our next do...while loop
            if ( ::FindNextFile( m_Handle, &foundFile ) == 0 )
            {      
                m_Done = true;

                Close();

                if ( (error = GetLastError()) != ERROR_NO_MORE_FILES ) 
                {
                    throw PlatformException( error, "Error calling FindNextFile" );
                }
            }
        }
    }

    return !m_Done;
}

void Directory::Close()
{
    if ( m_Handle != INVALID_HANDLE_VALUE )
    {
        BOOL result = ::FindClose( m_Handle );
        m_Handle = INVALID_HANDLE_VALUE;
        NOC_ASSERT(result);
    }

    m_Done = true;
    m_Item.Clear(); 
}

void Nocturnal::RecurseDirectories( DirectoryItemSignature::Delegate delegate, const std::string &path, const std::string &spec, u32 flags )
{
    // contents
    for ( Directory dir ( path, spec, flags ); !dir.IsDone(); dir.Next() )
    {
        delegate.Invoke( dir.GetItem() );
    }

    // recurse
    for ( Directory dir ( path, "*.*", DirectoryFlags::SkipFiles ); !dir.IsDone(); dir.Next() )
    {
        if ( dir.GetItem().m_Flags & DirectoryItemFlags::Directory )
        {
            RecurseDirectories( delegate, dir.GetItem().m_Path, spec, flags );
        }
    }
}