#include "FoundationPch.h"
#include "DirectoryIterator.h"

#include "Platform/Assert.h"
#include "Platform/Exception.h"

using namespace Helium;

DirectoryIterator::DirectoryIterator()
: m_Done( true )
{

}

DirectoryIterator::DirectoryIterator( const FilePath& path, uint32_t flags )
: m_Done( true )
{
    HELIUM_ASSERT( path.IsDirectory() );
    HELIUM_ASSERT( *path.Get().rbegin() == TXT( '/' ) )

    Open( path, flags );
}

DirectoryIterator::~DirectoryIterator()
{
    Close();
}

bool DirectoryIterator::IsDone()
{
    return m_Done;
}

bool DirectoryIterator::Next()
{
    if ( m_Done )
    {
        return false;
    }

    return Find();
}

const DirectoryIteratorItem& DirectoryIterator::GetItem()
{
    if ( m_Done )
    {
        throw Helium::Exception( TXT( "The file iterator is invalid!" ) );
    }

    return m_Item;
}

void DirectoryIterator::Reset()
{
    Close();
    Find();
}

bool DirectoryIterator::Open( const FilePath& path, uint32_t flags )
{
    Close();

    HELIUM_ASSERT( path.IsDirectory() );
    HELIUM_ASSERT( *path.Get().rbegin() == TXT( '/' ) )

    m_Path = path;
    m_Directory.SetPath( m_Path.Get() );
    m_Flags = flags;

    return Find();
}

void DirectoryIterator::GetFiles( const FilePath& path, std::set< Helium::FilePath >& paths, bool recursive )
{
    for ( DirectoryIterator dir( path, DirectoryFlags::SkipDirectories ); !dir.IsDone(); dir.Next() )
    {
        const DirectoryIteratorItem& item = dir.GetItem();
        paths.insert( item.m_Path );
    }

    if ( recursive )
    {
        for ( DirectoryIterator dir( path, DirectoryFlags::SkipFiles ); !dir.IsDone(); dir.Next() )
        {
            GetFiles( dir.GetItem().m_Path, paths, recursive );
        }
    }
}

void DirectoryIterator::GetFiles( std::set< Helium::FilePath >& paths, bool recursive )
{
    GetFiles( m_Path, paths, recursive );
}

bool DirectoryIterator::Find()
{
    m_Done = false;
    HELIUM_ASSERT( m_Directory.GetPath().length() > 0 );

    DirectoryEntry entry;
    bool findResult = !m_Directory.IsOpen() ? m_Directory.FindFirst( entry ) : m_Directory.FindNext( entry );
    if ( findResult == 0 )
    {      
        m_Done = true;
        Close();
    }

    // while our current item isn't what we are looking for
    while( !m_Done )
    {
        bool ok = true;
        bool needStatPath = true;

        tstring absolutePath;

        // skip relative path directories if fileName is "." or ".."
        if ( ( StringCompare( entry.m_Name.c_str(), TXT( "." ) ) == 0 ) || ( StringCompare( entry.m_Name.c_str(), TXT( ".." ) ) == 0 ) )
        {
            ok = false;
        }
        else
        {
            absolutePath = m_Path.Get() + entry.m_Name;

            HELIUM_VERIFY( entry.m_Stat.Read( absolutePath.c_str() ) );
            needStatPath = false;

            // directory...
            if ( entry.m_Stat.m_Mode & StatusModes::Directory )
            {
                // SkipDirectories: skip directory files, also skip hidden/system directories, so we don't try to access "System Volume Information"
                if ( m_Flags & DirectoryFlags::SkipDirectories  
                    || entry.m_Stat.m_Mode & StatusModes::Special )
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
            if ( needStatPath )
            {
                absolutePath = m_Path.Get() + entry.m_Name;
				HELIUM_VERIFY( entry.m_Stat.Read( absolutePath.c_str() ) );
            }

            if ( m_Flags & DirectoryFlags::RelativePath )
            {
                m_Item.m_Path.Set( entry.m_Name );
            }
            else
            {
                if ( entry.m_Stat.m_Mode & StatusModes::Directory )
                {
                    FilePath::GuaranteeSeparator( absolutePath );
                }

                m_Item.m_Path = absolutePath;
            }

            m_Item.m_CreateTime = entry.m_Stat.m_CreatedTime;
            m_Item.m_ModTime = entry.m_Stat.m_ModifiedTime;
            m_Item.m_Size = entry.m_Stat.m_Size;

            break;
        }
        else
        {
            m_Item.Clear();

            // this pumps the file iterator to the next element (into entry) for our next do...while loop
            if ( !m_Directory.FindNext( entry ) )
            {      
                m_Done = true;
                Close();
            }
        }
    }

    return !m_Done;
}

void DirectoryIterator::Close()
{
    if ( m_Directory.IsOpen() )
    {
        bool result = m_Directory.Close();
        HELIUM_ASSERT( result );
    }

    m_Done = true;
    m_Item.Clear(); 
}

void Helium::RecurseDirectories( DirectoryItemSignature::Delegate delegate, const FilePath& path, uint32_t flags )
{
    // contents
    for ( DirectoryIterator dir( path, flags ); !dir.IsDone(); dir.Next() )
    {
        delegate.Invoke( dir.GetItem() );
    }

    // recurse
    for ( DirectoryIterator dir( path, DirectoryFlags::SkipFiles ); !dir.IsDone(); dir.Next() )
    {
        if ( dir.GetItem().m_Path.IsDirectory() )
        {
            RecurseDirectories( delegate, dir.GetItem().m_Path, flags );
        }
    }
}