#include "ReflectPch.h"
#include "Reflect/Archive.h"

#include "Platform/Locks.h"
#include "Platform/Process.h"
#include "Platform/Exception.h"

#include "Foundation/Log.h"
#include "Foundation/Profile.h"
#include "Reflect/Object.h"
#include "Reflect/Data/DataDeduction.h"
#include "Reflect/Version.h"
#include "Reflect/Registry.h"
#include "Reflect/ArchiveXML.h"
#include "Reflect/ArchiveBinary.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <memory>

using namespace Helium;
using namespace Helium::Reflect;

Archive::Archive( const FilePath& path, ByteOrder byteOrder )
: m_Path( path )
, m_ByteOrder( byteOrder )
, m_Progress( 0 )
, m_SearchClass( NULL )
, m_Abort( false )
, m_Mode( ArchiveModes::Read )
{
    HELIUM_ASSERT( !m_Path.empty() );
}

Archive::Archive()
: m_ByteOrder( ByteOrders::LittleEndian )
, m_Progress( 0 )
, m_SearchClass( NULL )
, m_Abort( false )
, m_Mode( ArchiveModes::Read )
{
}

Archive::~Archive()
{
}

#pragma TODO( "Add support for writing objects piecemeal into the archive in Put" )

void Archive::Put( Object* object )
{
    m_Objects.push_back( object );
}

void Archive::Put( const std::vector< ObjectPtr >& objects )
{
    m_Objects.reserve( m_Objects.size() + objects.size() );
    m_Objects.insert( m_Objects.end(), objects.begin(), objects.end() );
}

ObjectPtr Archive::Get( const Class* searchClass )
{
    REFLECT_SCOPE_TIMER( ( "%s", m_Path.c_str() ) );

    std::vector< ObjectPtr > objects;
    Get( objects );

    if ( searchClass == NULL )
    {
        searchClass = Reflect::GetClass< Object >();
    }

    ObjectPtr result = NULL;
    std::vector< ObjectPtr >::iterator itr = objects.begin();
    std::vector< ObjectPtr >::iterator end = objects.end();
    for ( ; itr != end; ++itr )
    {
        if ( (*itr)->IsClass( searchClass ) )
        {
            return *itr;
        }
    }

    return NULL;
}

void Archive::Get( std::vector< ObjectPtr >& objects )
{
    REFLECT_SCOPE_TIMER( ( "%s", m_Path.c_str() ) );

    Log::Debug( TXT( "Parsing '%s'\n" ), m_Path.c_str() );

    if ( Helium::IsDebuggerPresent() )
    {
        Open();
        Read();
        Close(); 
    }
    else
    {
        try
        {
            Open();

            try
            {
                Read();
            }
            catch (...)
            {
                Close();
                throw;
            }

            Close(); 
        }
        catch (Helium::Exception& ex)
        {
            tstringstream str;
            str << "While reading '" << m_Path.c_str() << "': " << ex.Get();
            ex.Set( str.str() );
            throw;
        }
    }

    objects = m_Objects;
}

ArchivePtr Reflect::GetArchive( const FilePath& path, ArchiveType archiveType, ByteOrder byteOrder )
{
    switch ( archiveType )
    {
    case ArchiveTypes::Auto:
        if ( path.Exists() )
        {
#pragma TODO( "Check the file's existing type and return it." )
        }
        // fall through to binary if the file doesn't exist
    case ArchiveTypes::Binary:
        return new ArchiveBinary( path, byteOrder );

    case ArchiveTypes::XML:
        return new ArchiveXML( path, byteOrder );

    default:
        throw Reflect::StreamException( TXT( "Unknown archive type" ) );
    }

    return NULL;
}

bool Reflect::ToArchive( const FilePath& path, ObjectPtr object, ArchiveType archiveType, tstring* error, ByteOrder byteOrder )
{
    std::vector< ObjectPtr > objects;
    objects.push_back( object );
    return ToArchive( path, objects, archiveType, error, byteOrder );
}

bool Reflect::ToArchive( const FilePath& path, const std::vector< ObjectPtr >& objects, ArchiveType archiveType, tstring* error, ByteOrder byteOrder )
{
    HELIUM_ASSERT( !path.empty() );
    HELIUM_ASSERT( objects.size() > 0 );
    HELIUM_ASSERT( byteOrder == ByteOrders::BigEndian || byteOrder == ByteOrders::LittleEndian ); // should be a known byteorder

    REFLECT_SCOPE_TIMER( ( "%s", path.c_str() ) );

    path.MakePath();

    // build a path to a unique file for this process
    FilePath safetyPath( path.Directory() + Helium::GetProcessString() );
    safetyPath.ReplaceExtension( path.Extension() );

    ArchivePtr archive = GetArchive( safetyPath, archiveType, byteOrder );
    archive->Put( objects );

    // generate the file to the safety location
    if ( Helium::IsDebuggerPresent() )
    {
        archive->Open( true );
        archive->Write();
        archive->Close();
    }
    else
    {
        bool open = false;

        try
        {
            archive->Open( true );
            open = true;
            archive->Write();
            archive->Close(); 
        }
        catch ( Helium::Exception& ex )
        {
            tstringstream str;
            str << "While writing '" << path.c_str() << "': " << ex.Get();
            
            if ( error )
            {
                *error = str.str();
            }

            if ( open )
            {
                archive->Close();
            }

            safetyPath.Delete();
            return false;
        }
    }

    try
    {
        // delete the destination file
        path.Delete();

        // move the written file to the destination location
        safetyPath.Move( path );
    }
    catch ( Helium::Exception& ex )
    {
        tstringstream str;
        str << "While moving '" << safetyPath.c_str() << "' to '" << path.c_str() << "': " << ex.Get();
        
        if ( error )
        {
            *error = str.str();
        }

        safetyPath.Delete();
        return false;
    }

    return true;
}
