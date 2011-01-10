#include "Foundation/Reflect/Archive.h"

#include "Platform/Mutex.h"
#include "Platform/Process.h"
#include "Platform/Debug.h"

#include "Foundation/Log.h"
#include "Foundation/Profile.h"
#include "Foundation/Reflect/Object.h"
#include "Foundation/Reflect/Data/DataDeduction.h"
#include "Foundation/Reflect/Version.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/ArchiveXML.h"
#include "Foundation/Reflect/ArchiveBinary.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <memory>

using namespace Helium;
using namespace Helium::Reflect;

Archive::Archive( const Path& path, ByteOrder byteOrder )
: m_Path( path )
, m_ByteOrder( byteOrder )
, m_Progress( 0 )
, m_SearchClass( NULL )
, m_Abort( false )
, m_Mode( ArchiveModes::Read )
{
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

void Archive::PreSerialize()
{
    StatusInfo info( *this, ArchiveStates::PreProcessing );
    e_Status.Raise( info );

    // we used to raise an event here for serialization, no one used it

    info.m_ArchiveState = ArchiveStates::ArchiveStarting;
    info.m_Progress = m_Progress = 0;
    e_Status.Raise( info );
}

void Archive::PostSerialize(std::vector< ObjectPtr >& append)
{
    StatusInfo info( *this, ArchiveStates::ArchiveComplete );
    info.m_Progress = m_Progress = 100;
    e_Status.Raise( info );

    PROFILE_SCOPE_ACCUM(g_PostSerializeAccum); 
    info.m_ArchiveState = ArchiveStates::PostProcessing;
    e_Status.Raise( info );
}

void Archive::PreDeserialize()
{
    StatusInfo info( *this, ArchiveStates::PreProcessing );
    e_Status.Raise( info );

    // we used to raise an event here for deserialization, no one was using it

    info.m_ArchiveState = ArchiveStates::ArchiveStarting;
    info.m_Progress = m_Progress = 0;
    e_Status.Raise( info );
}

void Archive::PostDeserialize(std::vector< ObjectPtr >& append)
{
    StatusInfo info( *this, ArchiveStates::ArchiveComplete );
    info.m_Progress = m_Progress = 100;
    e_Status.Raise( info );

    PROFILE_SCOPE_ACCUM(g_PostDeserializeAccum); 
    info.m_ArchiveState = ArchiveStates::PostProcessing;
    e_Status.Raise( info );
}

void Archive::PreSerialize(Object* object, const Field* field)
{
    V_ArchiveVisitor::const_iterator itr = m_Visitors.begin();
    V_ArchiveVisitor::const_iterator end = m_Visitors.end();
    for ( ; itr != end; ++itr )
    {
        if (field)
        {
            (*itr)->VisitField(object, field);
        }
        else
        {
            (*itr)->VisitObject(object);
        }
    }
}

void Archive::PostDeserialize(Object* object, const Field* field)
{
    V_ArchiveVisitor::const_iterator itr = m_Visitors.begin();
    V_ArchiveVisitor::const_iterator end = m_Visitors.end();
    for ( ; itr != end; ++itr )
    {
        if (field)
        {
            (*itr)->VisitField(object, field);
        }
        else
        {
            (*itr)->VisitObject(object);
        }
    }
}

bool Archive::TryObjectCallback( Object* object, ObjectCallback callback )
{
    if ( Helium::IsDebuggerPresent() )
    {
        (object->*callback)();
    }
    else
    {
        try
        {
            (object->*callback)();
        }
        catch ( const Helium::Exception& exception )
        {
            ExceptionInfo info( *this, object, callback, exception );

            d_Exception.Invoke( info );

            switch ( info.m_Action )
            {
            case ExceptionActions::Unknown:
                {
                    throw;
                }

            case ExceptionActions::Accept:
                {
                    return true;
                }

            case ExceptionActions::Reject:
                {
                    return false;
                }
            }
        }
    }

    return true;
}

#pragma TODO( "Add support for writing objects piecemeal into the archive in Put" )

void Archive::Put( Object* object )
{
    m_Objects.push_back( object );
}

void Archive::Put( const std::vector< ObjectPtr >& elements )
{
    m_Objects.reserve( m_Objects.size() + elements.size() );
    m_Objects.insert( m_Objects.end(), elements.begin(), elements.end() );
}

ObjectPtr Archive::Get( const Class* searchClass )
{
    REFLECT_SCOPE_TIMER( ( "%s", m_Path.c_str() ) );

    std::vector< ObjectPtr > elements;
    Get( elements );

    if ( searchClass == NULL )
    {
        searchClass = Reflect::GetClass< Object >();
    }

    ObjectPtr result = NULL;
    std::vector< ObjectPtr >::iterator itr = elements.begin();
    std::vector< ObjectPtr >::iterator end = elements.end();
    for ( ; itr != end; ++itr )
    {
        if ( (*itr)->HasType( searchClass ) )
        {
            return *itr;
        }
    }

    return NULL;
}

void Archive::Get( std::vector< ObjectPtr >& elements )
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

    elements = m_Objects;
}

ArchivePtr Reflect::GetArchive( const Path& path, ArchiveType archiveType, ByteOrder byteOrder )
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

bool Reflect::ToArchive( const Path& path, ObjectPtr object, ArchiveType archiveType, tstring* error, ByteOrder byteOrder )
{
    std::vector< ObjectPtr > elements;
    elements.push_back( object );
    return ToArchive( path, elements, archiveType, error, byteOrder );
}

bool Reflect::ToArchive( const Path& path, const std::vector< ObjectPtr >& elements, ArchiveType archiveType, tstring* error, ByteOrder byteOrder )
{
    HELIUM_ASSERT( !path.empty() );
    HELIUM_ASSERT( elements.size() > 0 );
    HELIUM_ASSERT( byteOrder == ByteOrders::BigEndian || byteOrder == ByteOrders::LittleEndian ); // should be a known byteorder

    REFLECT_SCOPE_TIMER( ( "%s", path.c_str() ) );

    path.MakePath();

    // build a path to a unique file for this process
    Path safetyPath( path.Directory() + Helium::GetProcessString() );
    safetyPath.ReplaceExtension( path.Extension() );

    ArchivePtr archive = GetArchive( safetyPath, archiveType, byteOrder );
    archive->Put( elements );

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
