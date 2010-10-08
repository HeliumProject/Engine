#include <sys/stat.h>
#include <sys/types.h>
#include <memory>

#include "Element.h"
#include "Version.h"
#include "Registry.h"

#include "ArchiveXML.h"
#include "ArchiveBinary.h"

#include "Platform/Mutex.h"
#include "Platform/Process.h"
#include "Platform/Debug.h"
#include "Foundation/Log.h"
#include "Foundation/Profile.h"

using namespace Helium;
using namespace Helium::Reflect;



//
// Archive is a std set of static and non-static archive related functions
//

Archive::Archive( const Path& path )
: m_Path( path )
, m_Progress( 0 )
, m_SearchType( Reflect::ReservedTypes::Invalid )
, m_Abort( false )
, m_Mode( ArchiveModes::Read )
{
}

Archive::Archive()
: m_Path( TXT( "" ) )
, m_Progress( 0 )
, m_SearchType( Reflect::ReservedTypes::Invalid )
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

void Archive::PostSerialize(V_Element& append)
{
    StatusInfo info( *this, ArchiveStates::ArchiveComplete );
    info.m_Progress = m_Progress = 100;
    e_Status.Raise( info );

    info.m_ArchiveState = ArchiveStates::PostProcessing;
    e_Status.Raise( info );

    {
        PROFILE_SCOPE_ACCUM(g_PostSerializeAccum); 

        V_ArchiveVisitor::const_iterator itr = m_Visitors.begin();
        V_ArchiveVisitor::const_iterator end = m_Visitors.end();
        for ( ; itr != end; ++itr )
        {
            (*itr)->CreateAppendElements(append);
        }
    }
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

void Archive::PostDeserialize(V_Element& append)
{
    StatusInfo info( *this, ArchiveStates::ArchiveComplete );
    info.m_Progress = m_Progress = 100;
    e_Status.Raise( info );

    info.m_ArchiveState = ArchiveStates::PostProcessing;
    e_Status.Raise( info );

    {
        PROFILE_SCOPE_ACCUM(g_PostDeserializeAccum); 

        V_ArchiveVisitor::const_iterator itr = m_Visitors.begin();
        V_ArchiveVisitor::const_iterator end = m_Visitors.end();
        for ( ; itr != end; ++itr )
        {
            (*itr)->ProcessAppendElements(append);
        }
    }
}

void Archive::PreSerialize(const ElementPtr& element, const Field* field)
{
    V_ArchiveVisitor::const_iterator itr = m_Visitors.begin();
    V_ArchiveVisitor::const_iterator end = m_Visitors.end();
    for ( ; itr != end; ++itr )
    {
        if (field)
        {
            (*itr)->VisitField(element, field);
        }
        else
        {
            (*itr)->VisitElement(element);
        }
    }

    m_Types.insert(element->GetType());
}

void Archive::PostDeserialize(const ElementPtr& element, const Field* field)
{
    V_ArchiveVisitor::const_iterator itr = m_Visitors.begin();
    V_ArchiveVisitor::const_iterator end = m_Visitors.end();
    for ( ; itr != end; ++itr )
    {
        if (field)
        {
            (*itr)->VisitField(element, field);
        }
        else
        {
            (*itr)->VisitElement(element);
        }
    }

    m_Types.insert(element->GetType());
}

bool Archive::TryElementCallback( Element* element, ElementCallback callback )
{
    if ( Helium::IsDebuggerPresent() )
    {
        (element->*callback)();
    }
    else
    {
        try
        {
            (element->*callback)();
        }
        catch ( const Helium::Exception& exception )
        {
            ExceptionInfo info( *this, element, callback, exception );

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

void Archive::Put( const ElementPtr& element )
{
    m_Spool.push_back( element );
}

void Archive::Put( const V_Element& elements )
{
    m_Spool.reserve( m_Spool.size() + elements.size() );
    m_Spool.insert( m_Spool.end(), elements.begin(), elements.end() );
}


ElementPtr Archive::Get( int searchType )
{
    REFLECT_SCOPE_TIMER( ( "%s", m_Path.c_str() ) );

    V_Element elements;
    Get( elements );

    if ( searchType == Reflect::ReservedTypes::Any )
    {
        searchType = Reflect::GetType< Element >();
    }

    ElementPtr result = NULL;
    V_Element::iterator itr = elements.begin();
    V_Element::iterator end = elements.end();
    for ( ; itr != end; ++itr )
    {
        if ( (*itr)->HasType( searchType ) )
        {
            return *itr;
        }
    }

    return NULL;
}

void Archive::Get( V_Element& elements )
{
    REFLECT_SCOPE_TIMER( ( "%s", m_Path.c_str() ) );

    Log::Debug( TXT( "Parsing '%s'" ), m_Path.c_str() );

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

    elements = m_Spool;
}

bool Reflect::GetFileType( const Path& path, ArchiveType& type )
{
    tstring ext = path.Extension();

    if ( ext == Archive::GetExtension( ArchiveTypes::XML ) )
    {
        type = ArchiveTypes::XML;
        return true;
    }
    else if ( ext == Archive::GetExtension( ArchiveTypes::Binary ) )
    {
        type = ArchiveTypes::Binary;
        return true;
    }

    return false;
}

ArchivePtr Reflect::GetArchive( const Path& path )
{
    HELIUM_ASSERT( !path.empty() );

    Reflect::ArchiveType archiveType;
    if ( GetFileType( path, archiveType ) )
    {
        switch ( archiveType )
        {
        case ArchiveTypes::Binary:
            return new ArchiveBinary( path );

        case ArchiveTypes::XML:
            return new ArchiveXML( path );

        default:
            throw Reflect::StreamException( TXT( "Unknown archive type" ) );
        }
    }

    return NULL;
}

bool Reflect::ToArchive( const Path& path, ElementPtr element, tstring* error )
{
    V_Element elements;
    elements.push_back( element );
    return ToArchive( path, elements, error );
}

bool Reflect::ToArchive( const Path& path, const V_Element& elements, tstring* error )
{
    HELIUM_ASSERT( !path.empty() );
    HELIUM_ASSERT( elements.size() > 0 );

    REFLECT_SCOPE_TIMER( ( "%s", path.c_str() ) );

    path.MakePath();

    // build a path to a unique file for this process
    Path safetyPath( path.Directory() + Helium::GetProcessString() );
    safetyPath.ReplaceExtension( path.Extension() );

    ArchivePtr archive = GetArchive( safetyPath );
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
