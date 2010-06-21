#include "stdafx.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <memory>

#include "Element.h"
#include "Version.h"
#include "Registry.h"

#include "ArchiveXML.h"
#include "ArchiveBinary.h"
#include "FileStream.h" 

#include "Platform/Mutex.h"
#include "Platform/Process.h"
#include "Foundation/Log.h"
#include "Foundation/Profile.h"
#include "Foundation/CommandLine.h"

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

void StatusHandler::ArchiveWarning(const std::string& warning)
{
    Log::Warning("%s", warning.c_str());
}

void StatusHandler::ArchiveDebug(const std::string& debug)
{
    Log::Debug("%s", debug.c_str());
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

            if (info.m_Archive.GetFile().empty())
            {
                m_Bullet.reset( new Log::Bullet ("%s %s stream\n", verb, type) );
            }
            else
            {
                m_Bullet.reset( new Log::Bullet ("%s %s file '%s'\n", verb, type, info.m_Archive.GetFile().c_str()) );

                if (info.m_Archive.GetMode() == ArchiveModes::Read)
                {
                    struct stat st;
                    stat(info.m_Archive.GetFile().c_str(), &st);

                    if (st.st_size > 1000)
                    {
                        Log::Bullet bullet ("Size: %dk\n", st.st_size / 1000);
                    }
                    else
                    {
                        Log::Bullet bullet ("Size: %d\n", st.st_size);
                    }
                }
            }

            m_Start = true;

            break;
        }

    case Reflect::ArchiveStates::PostProcessing:
        {
            Log::Bullet bullet ("Processing...\n");
            break;
        }

    case Reflect::ArchiveStates::Complete:
        {
            Log::Bullet bullet ("Completed in %.2f ms\n", Platform::CyclesToMillis(Platform::TimerGetClock() - m_Timer));
            break;
        }

    case Reflect::ArchiveStates::Publishing:
        {
            Log::Bullet bullet ("Publishing to %s\n", info.m_DestinationFile.c_str());
            break;
        }
    }
}

void PrintStatus::ArchiveWarning(const std::string& warning)
{
    if (m_Progress >= 0 && !m_Start)
    {
        Log::Warning("\n");
        m_Start = true;
    }
    __super::ArchiveWarning( warning );
}

void PrintStatus::ArchiveDebug(const std::string& debug)
{
    if (m_Progress >= 0 && !m_Start)
    {
        Log::Debug("\n");
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
, m_Stream (NULL)
{

}

Archive::~Archive()
{

}

bool Archive::GetFileType( const std::string& file, ArchiveType& type )
{
    Nocturnal::Path filePath( file );
    std::string ext = filePath.Extension();

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

void Archive::Warning(const char* fmt, ...)
{
    static char buff[512];

    va_list args;
    va_start(args, fmt); 
    int size = _vsnprintf(buff, sizeof(buff), fmt, args);
    buff[ sizeof(buff) - 1] = 0; 
    va_end(args);      

    if (m_Status)
    {
        m_Status->ArchiveWarning( buff );
    }
    else
    {
        Log::Warning("%s", buff);
    }
}

void Archive::Debug(const char* fmt, ...)
{
    static char buff[512];

    va_list args;
    va_start(args, fmt); 
    int size = _vsnprintf(buff, sizeof(buff), fmt, args);
    buff[ sizeof(buff)-1] = 0; 
    va_end(args);      

    if (m_Status)
    {
        m_Status->ArchiveDebug( buff );
    }
    else
    {
        Log::Debug("%s", buff);
    }
}

Archive* Archive::GetArchive(const std::string& file, StatusHandler* handler)
{
    if ( file.empty() )
    {
        throw Reflect::StreamException( "File path is empty" );
    }

    Reflect::ArchiveType archiveType;
    if ( GetFileType( file, archiveType ) )
    {
        return GetArchive( archiveType, handler);
    }

    return NULL;
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
        throw Reflect::StreamException( "Unknown archive type" );
    }

    return NULL;
}

void Archive::OpenFile( const std::string& file, bool write )
{
    m_File = file;

#ifdef REFLECT_ARCHIVE_VERBOSE
    Debug("Opening file '%s'\n", file.c_str());
#endif

    Reflect::StreamPtr stream = new FileStream(file, write); 
    OpenStream( stream, write );
}

void Archive::OpenStream( const StreamPtr& stream, bool write )
{
    // save the mode here, so that we safely refer to it later.
    m_Mode = (write) ? ArchiveModes::Write : ArchiveModes::Read; 

    // open the stream, this is "our interface" 
    stream->Open(); 

    // Set precision
    stream->SetPrecision(32);

    // Setup stream
    m_Stream = stream; 

    // Header
    if (write)
    {
        Start();
    }
}

void Archive::Close()
{
    if (m_Mode == ArchiveModes::Write)
    {
        Finish(); 
    }

    m_Stream->Close(); 
    m_Stream = NULL; 
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
    try
    {
        (element->*callback)();
    }
    catch ( const Nocturnal::Exception& exception )
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

    return true;
}

void Archive::ToXML(const ElementPtr& element, std::string& xml, StatusHandler* status)
{
    V_Element elements(1);
    elements[0] = element;
    return ToXML( elements, xml, status );
}

ElementPtr Archive::FromXML(const std::string& xml, int searchType, StatusHandler* status)
{
    if (searchType == Reflect::ReservedTypes::Any)
    {
        searchType = Reflect::GetType<Element>();
    }

    ArchiveXML archive (status);
    archive.m_SearchType = searchType;

    std::stringstream strStream;
    strStream << "<?xml version=\"1.0\"?><Reflect FileFormatVersion=\""<<ArchiveXML::CURRENT_VERSION<<"\">" << xml << "</Reflect>";
    archive.m_Stream = new Reflect::Stream(&strStream); 
    archive.Read();

    V_Element::iterator itr = archive.m_Spool.begin();
    V_Element::iterator end = archive.m_Spool.end();
    for ( ; itr != end; ++itr )
    {
        if ((*itr)->HasType(searchType))
        {
            return *itr;
        }
    }

    return NULL;
}

void Archive::ToXML(const V_Element& elements, std::string& xml, StatusHandler* status)
{
    ArchiveXML archive (status);
    std::stringstream strStream;

    archive.m_Stream = new Reflect::Stream(&strStream); 
    archive.m_Spool  = elements;
    archive.Write();

    xml = strStream.str();
}

void Archive::FromXML(const std::string& xml, V_Element& elements, StatusHandler* status)
{
    ArchiveXML archive (status);
    std::stringstream strStream;
    strStream << "<?xml version=\"1.0\"?><Reflect FileFormatVersion=\""<<ArchiveXML::CURRENT_VERSION<<"\">" << xml << "</Reflect>";

    archive.m_Stream = new Reflect::Stream(&strStream); 
    archive.Read();

    elements = archive.m_Spool;
}

void Archive::ToStream(const ElementPtr& element, std::iostream& stream, ArchiveType type, StatusHandler* status)
{
    V_Element elements(1);
    elements[0] = element;
    ToStream( elements, stream, type, status );
}

ElementPtr Archive::FromStream(std::iostream& stream, ArchiveType type, int searchType, StatusHandler* status)
{
    if (searchType == Reflect::ReservedTypes::Any)
    {
        searchType = Reflect::GetType<Element>();
    }

    std::auto_ptr<Archive> archive (GetArchive(type, status));
    archive->m_SearchType = searchType;

    Reflect::StreamPtr reflectStream = new Reflect::Stream(&stream); 
    archive->OpenStream( reflectStream, false );
    archive->Read();
    archive->Close(); 

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

void Archive::ToStream(const V_Element& elements, std::iostream& stream, ArchiveType type, StatusHandler* status)
{
    std::auto_ptr<Archive> archive (GetArchive(type, status));

    // fix the spool
    archive->m_Spool.clear();
    archive->m_Spool.reserve( elements.size() );

    V_Element::const_iterator iter = elements.begin();
    V_Element::const_iterator end  = elements.end();
    for ( ; iter != end; ++iter )
    {
        if ( !(*iter)->HasType(Reflect::GetType<Version>()) )
        {
            archive->m_Spool.push_back( (*iter) );
        }
    }

    Reflect::StreamPtr reflectStream = new Reflect::Stream(&stream); 

    archive->OpenStream( reflectStream, true );
    archive->Write();   
    archive->Close(); 
}

void Archive::FromStream(std::iostream& stream, ArchiveType type, V_Element& elements, StatusHandler* status)
{
    std::auto_ptr<Archive> archive (GetArchive(type, status));

    Reflect::StreamPtr reflectStream = new Reflect::Stream(&stream); 

    archive->OpenStream( reflectStream, false );
    archive->Read();
    archive->Close(); 

    elements = archive->m_Spool;
}

void Archive::ToFile(const ElementPtr& element, const std::string& file)
{
    ToFile( element, file, NULL, NULL );
}

void Archive::ToFile(const ElementPtr& element, const std::string& file, VersionPtr version, StatusHandler* status)
{
    V_Element elements(1);
    elements[0] = element;
    ToFile( elements, file, version, status );
}

ElementPtr Archive::FromFile(const std::string& file, int searchType, StatusHandler* status)
{
    if (searchType == Reflect::ReservedTypes::Any)
    {
        searchType = Reflect::GetType<Element>();
    }

    REFLECT_SCOPE_TIMER(("%s", file.c_str()));

    char print[512];
    _snprintf(print, sizeof(print), "Parsing '%s'", file.c_str());
    PROFILE_SCOPE_ACCUM_VERBOSE(g_ParseAccum, print);

    std::auto_ptr<Archive> archive (GetArchive(file, status));
    archive->m_SearchType = searchType;
    archive->Debug("%s\n", print);

    s_FileAccess.Raise( FileAccessArgs( file, FileOperations::PreRead ) );

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
    catch (Nocturnal::Exception& ex)
    {
        std::stringstream str;
        str << "While reading '" << file << "': " << ex.Get();
        ex.Set( str.str() );
        throw;
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

void Archive::ToFile(const V_Element& elements, const std::string& file)
{
    ToFile( elements, file, NULL, NULL );
}

void Archive::ToFile(const V_Element& elements, const std::string& file, VersionPtr version, StatusHandler* status)
{
    REFLECT_SCOPE_TIMER(("%s", file.c_str()));

    char print[512];
    _snprintf(print, sizeof(print), "Authoring '%s'", file.c_str());
    PROFILE_SCOPE_ACCUM_VERBOSE(g_AuthorAccum, print);

    std::auto_ptr<Archive> archive (GetArchive(file, status));
    archive->Debug("%s\n", print);

    s_FileAccess.Raise( FileAccessArgs( file, FileOperations::PreWrite ) );
    Nocturnal::Path outputPath( file );
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
    Nocturnal::Path safetyPath( outputPath.Directory() + Platform::GetProcessString() );
    safetyPath.ReplaceExtension( outputPath.Extension() );

    // generate the file to the safety location
    try
    {
        archive->OpenFile( safetyPath.Get(), true );

        try
        {
            archive->Write();
        }
        catch (...)
        {
            archive->Close(); 
            throw;
        }

        archive->Close(); 
    }
    catch ( Nocturnal::Exception& ex )
    {
        std::stringstream str;
        str << "While writing '" << file << "': " << ex.Get();
        ex.Set( str.str() );

        safetyPath.Delete();

        throw;
    }
    catch ( ... )
    {
        safetyPath.Delete();
        throw;
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
    catch ( Nocturnal::Exception& ex )
    {
        std::stringstream str;
        str << "While moving '" << safetyPath.c_str() << "' to '" << file << "': " << ex.Get();
        ex.Set( str.str() );

        safetyPath.Delete();

        throw;
    }

    s_FileAccess.Raise( FileAccessArgs( file, FileOperations::PostWrite ) );
}

void Archive::FromFile(const std::string& file, V_Element& elements, StatusHandler* status)
{
    REFLECT_SCOPE_TIMER(("%s", file.c_str()));

    char print[512];
    _snprintf(print, sizeof(print), "Parsing '%s'", file.c_str());
    PROFILE_SCOPE_ACCUM_VERBOSE(g_ParseAccum, print);

    std::auto_ptr<Archive> archive (GetArchive(file, status));
    archive->Debug("%s\n", print);

    s_FileAccess.Raise( FileAccessArgs( file, FileOperations::PreRead ) );

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
    catch (Nocturnal::Exception& ex)
    {
        std::stringstream str;
        str << "While reading '" << file << "': " << ex.Get();
        ex.Set( str.str() );
        throw;
    }

    s_FileAccess.Raise( FileAccessArgs( file, FileOperations::PostRead ) );

    elements = archive->m_Spool;
}
