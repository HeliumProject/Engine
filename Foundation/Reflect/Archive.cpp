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

Archive::Archive()
: m_ParsingArchive( NULL )
, m_Progress( 0 )
, m_SearchType( Reflect::ReservedTypes::Invalid )
, m_Abort( false )
, m_Mode( ArchiveModes::Read )
{
}

Archive::Archive( const Path& path )
: m_Path( path )
, m_ParsingArchive( NULL )
, m_Progress( 0 )
, m_SearchType( Reflect::ReservedTypes::Invalid )
, m_Abort( false )
, m_Mode( ArchiveModes::Read )
{
}

Archive::Archive( const Path& path, const ElementPtr& element )
: m_Path( path )
, m_ParsingArchive( NULL )
, m_Progress( 0 )
, m_SearchType( Reflect::ReservedTypes::Invalid )
, m_Abort( false )
, m_Mode( ArchiveModes::Read )
{
    m_Spool.push_back( element );
}

Archive::Archive( const Path& path, const V_Element& elements )
: m_Path( path )
, m_Spool( elements )
, m_Progress( 0 )
, m_SearchType( Reflect::ReservedTypes::Invalid )
, m_Abort( false )
, m_Mode( ArchiveModes::Read )
{
}

Archive::~Archive()
{
}

bool Archive::GetFileType( const Path& path, ArchiveType& type )
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

Archive* Archive::GetArchive( ArchiveType type )
{
    switch (type)
    {
    case ArchiveTypes::Binary:
        return new ArchiveBinary();

    case ArchiveTypes::XML:
        return new ArchiveXML();

    default:
        throw Reflect::StreamException( TXT( "Unknown archive type" ) );
    }

    return NULL;
}

Archive* Archive::GetArchive( const Path& path )
{
    HELIUM_ASSERT( !path.empty() );

    Reflect::ArchiveType archiveType;
    if ( GetFileType( path, archiveType ) )
    {
        return GetArchive( archiveType );
    }

    return NULL;
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

    HELIUM_ASSERT( m_ParsingArchive == NULL );
    m_ParsingArchive = GetArchive( m_Path );
    m_ParsingArchive->e_Status = e_Status;
    m_ParsingArchive->d_Exception = d_Exception;

    if ( Helium::IsDebuggerPresent() )
    {
        m_ParsingArchive->OpenFile( m_Path );
        m_ParsingArchive->Read();
        m_ParsingArchive->Close(); 
    }
    else
    {
        try
        {
            m_ParsingArchive->OpenFile( m_Path );

            try
            {
                m_ParsingArchive->Read();
            }
            catch (...)
            {
                m_ParsingArchive->Close();

                delete m_ParsingArchive;
                m_ParsingArchive = NULL;

                throw;
            }

            m_ParsingArchive->Close(); 
        }
        catch (Helium::Exception& ex)
        {
            delete m_ParsingArchive;
            m_ParsingArchive = NULL;

            tstringstream str;
            str << "While reading '" << m_Path.c_str() << "': " << ex.Get();
            ex.Set( str.str() );
            throw;
        }
    }

    elements = m_ParsingArchive->m_Spool;
    delete m_ParsingArchive;
    m_ParsingArchive = NULL;
}


void Archive::Save()
{
    HELIUM_ASSERT( !m_Path.empty() );

    REFLECT_SCOPE_TIMER(("%s", path.c_str()));

    m_Path.MakePath();

    // build a path to a unique file for this process
    Helium::Path safetyPath( m_Path.Directory() + Helium::GetProcessString() );
    safetyPath.ReplaceExtension( m_Path.Extension() );

    HELIUM_ASSERT( m_ParsingArchive == NULL );
    m_ParsingArchive = GetArchive( m_Path );
    m_ParsingArchive->e_Status = e_Status;
    m_ParsingArchive->d_Exception = d_Exception;

    // eh, this is pretty crappy, causing a copy
    m_ParsingArchive->m_Spool = m_Spool;

    // generate the file to the safety location
    if ( Helium::IsDebuggerPresent() )
    {
        m_ParsingArchive->OpenFile( safetyPath.Get(), true );
        m_ParsingArchive->Write();
        m_ParsingArchive->Close(); 
    }
    else
    {
        bool open = false;

        try
        {
            m_ParsingArchive->OpenFile( safetyPath.Get(), true );
            open = true;
            m_ParsingArchive->Write();
            m_ParsingArchive->Close(); 
        }
        catch ( Helium::Exception& ex )
        {
            tstringstream str;
            str << "While writing '" << m_Path.c_str() << "': " << ex.Get();
            ex.Set( str.str() );

            if ( open )
            {
                m_ParsingArchive->Close();
            }

            delete m_ParsingArchive;
            m_ParsingArchive = NULL;

            safetyPath.Delete();
            throw;
        }
        catch ( ... )
        {
            if ( open )
            {
                m_ParsingArchive->Close();
            }

            delete m_ParsingArchive;
            m_ParsingArchive = NULL;

            safetyPath.Delete();
            throw;
        }
    }

    delete m_ParsingArchive;
    m_ParsingArchive = NULL;

    try
    {
        StatusInfo info( *this, ArchiveStates::Publishing );
        e_Status.Raise( info );

        // delete the destination file
        m_Path.Delete();

        // move the written file to the destination location
        safetyPath.Move( m_Path );
    }
    catch ( Helium::Exception& ex )
    {
        tstringstream str;
        str << "While moving '" << safetyPath.c_str() << "' to '" << m_Path.c_str() << "': " << ex.Get();
        ex.Set( str.str() );

        safetyPath.Delete();

        throw;
    }
}
