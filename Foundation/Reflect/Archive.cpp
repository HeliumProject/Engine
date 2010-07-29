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

using namespace Reflect;

//
// Standard
//

void StatusHandler::ArchiveStatus( StatusInfo& info )
{
    // by default we don't care...
}

void StatusHandler::ArchiveException( ExceptionInfo& info )
{
    // by default we cannot handle any exception
}

void StatusHandler::ArchiveWarning(const tstring& warning)
{
    Log::Warning( TXT( "%s" ), warning.c_str());
}

void StatusHandler::ArchiveDebug(const tstring& debug)
{
    Log::Debug( TXT( "%s" ), debug.c_str());
}

void PrintStatus::ArchiveStatus(StatusInfo& info)
{
    switch (info.m_Action)
    {
    case Reflect::ArchiveStates::Starting:
        {
            m_Timer = Platform::TimerGetClock();

            const char* verb = info.m_Archive.GetMode() == ArchiveModes::Read ? "Reading" : "Writing";
            const char* type = info.m_Archive.GetType() == ArchiveTypes::XML ? "XML" : "Binary";

            if (info.m_Archive.GetPath().empty())
            {
                m_Bullet.reset( new Log::Bullet( TXT( "%s %s stream\n" ), verb, type) );
            }
            else
            {
                m_Bullet.reset( new Log::Bullet( TXT( "%s %s file '%s'\n" ), verb, type, info.m_Archive.GetPath().c_str()) );

                if (info.m_Archive.GetMode() == ArchiveModes::Read)
                {
                    u64 size = info.m_Archive.GetPath().Size();
                    if ( size > 1000)
                    {
                        Log::Bullet bullet( TXT( "Size: %dk\n" ),  size / 1000);
                    }
                    else
                    {
                        Log::Bullet bullet( TXT( "Size: %d\n" ), size );
                    }
                }
            }

            m_Start = true;

            break;
        }

    case Reflect::ArchiveStates::PostProcessing:
        {
            Log::Bullet bullet( TXT( "Processing...\n" ) );
            break;
        }

    case Reflect::ArchiveStates::Complete:
        {
            Log::Bullet bullet( TXT( "Completed in %.2f ms\n" ), Platform::CyclesToMillis(Platform::TimerGetClock() - m_Timer));
            break;
        }

    case Reflect::ArchiveStates::Publishing:
        {
            Log::Bullet bullet( TXT( "Publishing to %s\n" ), info.m_DestinationFile.c_str());
            break;
        }
    }
}

void PrintStatus::ArchiveWarning(const tstring& warning)
{
    if (m_Progress >= 0 && !m_Start)
    {
        Log::Warning( TXT( "\n" ) );
        m_Start = true;
    }
    __super::ArchiveWarning( warning );
}

void PrintStatus::ArchiveDebug(const tstring& debug)
{
    if (m_Progress >= 0 && !m_Start)
    {
        Log::Debug( TXT( "\n" ) );
        m_Start = true;
    }
    __super::ArchiveDebug( debug );
}


//
// Archive is a std set of static and non-static archive related functions
//

FileAccessSignature::Event Archive::s_FileAccess;
SerializeSignature::Event Archive::s_Serialize;
DeserializeSignature::Event Archive::s_Deserialize;

Archive::Archive(StatusHandler* status)
: m_Progress (0)
, m_Status (status)
, m_SearchType (Reflect::ReservedTypes::Invalid)
, m_Abort (false)
, m_Mode (ArchiveModes::Read)
{

}

Archive::~Archive()
{

}

bool Archive::GetFileType( const tstring& file, ArchiveType& type )
{
    Helium::Path filePath( file );
    tstring ext = filePath.Extension();

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

void Archive::Warning(const tchar* fmt, ...)
{
    static tchar buff[512];

    va_list args;
    va_start(args, fmt); 
    int size = _vsntprintf(buff, sizeof(buff), fmt, args);
    buff[ sizeof(buff) - 1] = 0; 
    va_end(args);      

    if (m_Status)
    {
        m_Status->ArchiveWarning( buff );
    }
    else
    {
        Log::Warning( TXT( "%s" ), buff);
    }
}

void Archive::Debug(const tchar* fmt, ...)
{
    static tchar buff[512];

    va_list args;
    va_start(args, fmt); 
    int size = _vsntprintf(buff, sizeof(buff), fmt, args);
    buff[ sizeof(buff)-1] = 0; 
    va_end(args);      

    if (m_Status)
    {
        m_Status->ArchiveDebug( buff );
    }
    else
    {
        Log::Debug( TXT( "%s" ), buff);
    }
}

Archive* Archive::GetArchive(ArchiveType type, StatusHandler* handler)
{
    switch (type)
    {
    case ArchiveTypes::Binary:
        return new ArchiveBinary (handler);

    case ArchiveTypes::XML:
        return new ArchiveXML (handler);

    default:
        throw Reflect::StreamException( TXT( "Unknown archive type" ) );
    }

    return NULL;
}

Archive* Archive::GetArchive(const tstring& file, StatusHandler* handler)
{
    if ( file.empty() )
    {
        throw Reflect::StreamException( TXT( "File path is empty" ) );
    }

    Reflect::ArchiveType archiveType;
    if ( GetFileType( file, archiveType ) )
    {
        return GetArchive( archiveType, handler);
    }

    return NULL;
}

void Archive::PreSerialize()
{
    if (m_Status != NULL)
    {
        m_Status->ArchiveStatus( StatusInfo (*this, ArchiveStates::PreProcessing) );
    }

    {
        PROFILE_SCOPE_ACCUM(g_PreSerializeAccum); 

        s_Serialize.Raise( SerializeArgs (m_Visitors) );
    }

    if (m_Status != NULL)
    {
        StatusInfo info (*this, ArchiveStates::ArchiveStarting);
        info.m_Progress = m_Progress = 0;
        m_Status->ArchiveStatus(info);
    }
}

void Archive::PostSerialize(V_Element& append)
{
    if (m_Status != NULL)
    {
        StatusInfo info (*this, ArchiveStates::ArchiveComplete);
        info.m_Progress = m_Progress = 100;
        m_Status->ArchiveStatus(info);
    }

    if (m_Status != NULL)
    {
        m_Status->ArchiveStatus( StatusInfo (*this, ArchiveStates::PostProcessing) );
    }

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
    if (m_Status != NULL)
    {
        m_Status->ArchiveStatus( StatusInfo (*this, ArchiveStates::PreProcessing) );
    }

    {
        PROFILE_SCOPE_ACCUM(g_PreDeserializeAccum); 

        s_Deserialize.Raise( DeserializeArgs (m_Visitors) );
    }

    if (m_Status != NULL)
    {
        StatusInfo info (*this, ArchiveStates::ArchiveStarting);
        info.m_Progress = m_Progress = 0;
        m_Status->ArchiveStatus(info);
    }
}

void Archive::PostDeserialize(V_Element& append)
{
    if (m_Status != NULL)
    {
        StatusInfo info (*this, ArchiveStates::ArchiveComplete);
        info.m_Progress = m_Progress = 100;
        m_Status->ArchiveStatus(info);
    }

    if (m_Status != NULL)
    {
        m_Status->ArchiveStatus( StatusInfo (*this, ArchiveStates::PostProcessing) );
    }

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
    if ( Platform::IsDebuggerPresent() )
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
            if ( m_Status )
            {
                ExceptionInfo info ( *this, element, callback, exception );

                m_Status->ArchiveException( info );

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
            else
            {
                throw;
            }
        }
    }

    return true;
}

void Archive::ToFile(const ElementPtr& element, const tstring& file)
{
    ToFile( element, file, NULL, NULL );
}

void Archive::ToFile(const ElementPtr& element, const tstring& file, VersionPtr version, StatusHandler* status)
{
    V_Element elements(1);
    elements[0] = element;
    ToFile( elements, file, version, status );
}

ElementPtr Archive::FromFile(const tstring& file, int searchType, StatusHandler* status)
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

    std::auto_ptr<Archive> archive (GetArchive(file, status));
    archive->m_SearchType = searchType;
    archive->Debug( TXT( "%s\n" ), print);

    s_FileAccess.Raise( FileAccessArgs( file, FileOperations::PreRead ) );

    if ( Platform::IsDebuggerPresent() )
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

    s_FileAccess.Raise( FileAccessArgs( file, FileOperations::PostRead ) );

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

void Archive::ToFile(const V_Element& elements, const tstring& file)
{
    ToFile( elements, file, NULL, NULL );
}

void Archive::ToFile(const V_Element& elements, const tstring& file, VersionPtr version, StatusHandler* status)
{
    REFLECT_SCOPE_TIMER(("%s", file.c_str()));

    tchar print[512];
    _sntprintf(print, sizeof(print), TXT( "Authoring '%s'" ), file.c_str());
#pragma TODO("Profiler support for wide strings")
    PROFILE_SCOPE_ACCUM_VERBOSE(g_AuthorAccum, ""/*print*/);

    std::auto_ptr<Archive> archive (GetArchive(file, status));
    archive->Debug( TXT( "%s\n" ), print);

    s_FileAccess.Raise( FileAccessArgs( file, FileOperations::PreWrite ) );
    Helium::Path outputPath( file );
    outputPath.MakePath();

    // alloc a version object if we don't have one
    if ( !version.ReferencesObject() )
    {
        version = new Version();
    }

    // fix the spool
    archive->m_Spool.clear();
    archive->m_Spool.reserve( 1 + elements.size() );
    archive->m_Spool.push_back( version );

    V_Element::const_iterator iter = elements.begin();
    V_Element::const_iterator end  = elements.end();
    for ( ; iter != end; ++iter )
    {
        if ( !(*iter)->HasType(Reflect::GetType<Version>()) )
        {
            archive->m_Spool.push_back( (*iter) );
        }
    }

    // build a path to a unique file for this process
    Helium::Path safetyPath( outputPath.Directory() + Platform::GetProcessString() );
    safetyPath.ReplaceExtension( outputPath.Extension() );

    // generate the file to the safety location
    if ( Platform::IsDebuggerPresent() )
    {
        archive->OpenFile( safetyPath.Get(), true );
        archive->Write();
        archive->Close(); 
    }
    else
     {
        bool open = false;

        try
        {
            archive->OpenFile( safetyPath.Get(), true );
            open = true;
            archive->Write();
            archive->Close(); 
        }
        catch ( Helium::Exception& ex )
        {
            tstringstream str;
            str << "While writing '" << file << "': " << ex.Get();
            ex.Set( str.str() );

            if ( open )
            {
                archive->Close();
            }

            safetyPath.Delete();
            throw;
        }
        catch ( ... )
        {
            if ( open )
            {
                archive->Close();
            }

            safetyPath.Delete();
            throw;
        }
    }

    try
    {
        if ( status )
        {
            StatusInfo info ( *archive.get(), ArchiveStates::Publishing );
            info.m_DestinationFile = file;
            status->ArchiveStatus( info );
        }

        // delete the destination file
        outputPath.Delete();

        // move the written file to the destination location
        safetyPath.Move( outputPath );
    }
    catch ( Helium::Exception& ex )
    {
        tstringstream str;
        str << "While moving '" << safetyPath.c_str() << "' to '" << file << "': " << ex.Get();
        ex.Set( str.str() );

        safetyPath.Delete();

        throw;
    }

    s_FileAccess.Raise( FileAccessArgs( file, FileOperations::PostWrite ) );
}

void Archive::FromFile(const tstring& file, V_Element& elements, StatusHandler* status)
{
    REFLECT_SCOPE_TIMER(("%s", file.c_str()));

    tchar print[512];
    _sntprintf(print, sizeof(print), TXT( "Parsing '%s'" ), file.c_str());
#pragma TODO("Profiler support for wide strings")
    PROFILE_SCOPE_ACCUM_VERBOSE(g_ParseAccum, ""/*print*/);

    std::auto_ptr<Archive> archive (GetArchive(file, status));
    archive->Debug( TXT( "%s\n" ), print);

    s_FileAccess.Raise( FileAccessArgs( file, FileOperations::PreRead ) );

    if ( Platform::IsDebuggerPresent() )
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

    s_FileAccess.Raise( FileAccessArgs( file, FileOperations::PostRead ) );

    elements = archive->m_Spool;
}
