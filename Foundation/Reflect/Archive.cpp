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
: m_Progress( 0 )
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

ElementPtr Archive::FromFile( const tstring& file, int searchType )
{
    if (searchType == Reflect::ReservedTypes::Any)
    {
        searchType = Reflect::GetType<Element>();
    }

    REFLECT_SCOPE_TIMER(("%s", file.c_str()));

    tchar print[512];
    _sntprintf(print, sizeof(print), TXT( "Parsing '%s'" ), file.c_str());
#pragma TODO("Profiler support for wide strings")
    PROFILE_SCOPE_ACCUM_VERBOSE(g_ParseAccum, ""/*print*/);

    std::auto_ptr<Archive> archive( GetArchive( file ) );
    archive->m_SearchType = searchType;
    StatusInfo info( *archive, ArchiveStates::Starting, StatusTypes::Debug );
    info.m_Info = print;
    archive->e_Status.Raise( info );

    if ( Helium::IsDebuggerPresent() )
    {
        archive->OpenFile( file );
        archive->Read();
        archive->Close(); 
    }
    else
    {
        try
        {
            archive->OpenFile( file );

            try
            {
                archive->Read();
            }
            catch (...)
            {
                archive->Close(); 
                throw;
            }

            archive->Close(); 
        }
        catch ( Helium::Exception& ex )
        {
            tstringstream str;
            str << "While reading '" << file << "': " << ex.Get();
            ex.Set( str.str() );
            throw;
        }
    }

    V_Element::iterator itr = archive->m_Spool.begin();
    V_Element::iterator end = archive->m_Spool.end();
    for ( ; itr != end; ++itr )
    {
        if ((*itr)->HasType(searchType))
        {
            return *itr;
        }
    }

    return NULL;
}

void Archive::Save()
{
    HELIUM_ASSERT( !m_Path.empty() );

    REFLECT_SCOPE_TIMER(("%s", path.c_str()));

    StatusInfo info( *this, ArchiveStates::Publishing, StatusTypes::Debug );
    e_Status.Raise( info );

    m_Path.MakePath();

    // build a path to a unique file for this process
    Helium::Path safetyPath( m_Path.Directory() + Helium::GetProcessString() );
    safetyPath.ReplaceExtension( m_Path.Extension() );

    // generate the file to the safety location
    if ( Helium::IsDebuggerPresent() )
    {
        OpenFile( safetyPath.Get(), true );
        Write();
        Close(); 
    }
    else
    {
        bool open = false;

        try
        {
            OpenFile( safetyPath.Get(), true );
            open = true;
            Write();
            Close(); 
        }
        catch ( Helium::Exception& ex )
        {
            tstringstream str;
            str << "While writing '" << m_Path.c_str() << "': " << ex.Get();
            ex.Set( str.str() );

            if ( open )
            {
                Close();
            }

            safetyPath.Delete();
            throw;
        }
        catch ( ... )
        {
            if ( open )
            {
                Close();
            }

            safetyPath.Delete();
            throw;
        }
    }

    try
    {
        info.m_ArchiveState = ArchiveStates::Publishing;
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

void Archive::FromFile(const tstring& file, V_Element& elements )
{
    REFLECT_SCOPE_TIMER(("%s", file.c_str()));

    tchar print[512];
    _sntprintf(print, sizeof(print), TXT( "Parsing '%s'" ), file.c_str());
#pragma TODO("Profiler support for wide strings")
    PROFILE_SCOPE_ACCUM_VERBOSE(g_ParseAccum, ""/*print*/);

    std::auto_ptr<Archive> archive ( GetArchive( file ) );

    StatusInfo info( *archive.get(), ArchiveStates::Starting, StatusTypes::Debug );
    info.m_Info = print;
    archive->e_Status.Raise( info );

    if ( Helium::IsDebuggerPresent() )
    {
        archive->OpenFile( file );
        archive->Read();
        archive->Close(); 
    }
    else
    {
        try
        {
            archive->OpenFile( file );

            try
            {
                archive->Read();
            }
            catch (...)
            {
                archive->Close(); 
                throw;
            }

            archive->Close(); 
        }
        catch (Helium::Exception& ex)
        {
            tstringstream str;
            str << "While reading '" << file << "': " << ex.Get();
            ex.Set( str.str() );
            throw;
        }
    }

    elements = archive->m_Spool;
}
