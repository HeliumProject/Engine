#include "Platform/Windows/Windows.h"
#include "Foundation/Exception.h"
#undef CreateEvent

#include "EventSystem.h"

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <sys/timeb.h>
#include <time.h>
#include <algorithm>
#include <sstream>
#include <set>

#include "Application/Application.h"
#include "Foundation/Boost/Regex.h" 
#include "Platform/Types.h"
#include "Foundation/File/Directory.h"
#include "Foundation/String/Utilities.h"
#include "Foundation/Environment.h"
#include "Finder/ExtensionSpecs.h"
#include "Finder/ProjectSpecs.h"
#include "Finder/Finder.h"
#include "Foundation/Log.h"

using namespace Nocturnal::ES;


#define TIME_SIZE 32


typedef i32 RecordCount;

static const char* s_HandledEventsFilename = "handled_events.txt";


struct SortEvents
{
    bool operator()( const EventPtr& lhs, const EventPtr& rhs ) const
    {
        return lhs->m_Created < rhs->m_Created;
    }
};

/////////////////////////////////////////////////////////////////////////////
// Default constructor
//
EventSystem::EventSystem( const std::string &rootDirPath, bool writeBinaryFormat )
: m_RootDirPath( rootDirPath )
, m_WriteBinaryFormat( writeBinaryFormat )
{
    EVENTSYSTEM_SCOPE_TIMER((""));

    m_RootDirPath.MakePath();
    m_HandledEventsFile.Set( m_RootDirPath.Get() + '/' + s_HandledEventsFilename );
}


void EventSystem::CreateEventsFilePath( std::string& eventsFilePath )
{
    EVENTSYSTEM_SCOPE_TIMER((""));

    std::string fileName = std::string( getenv("USERNAME") ) + '-' + getenv("COMPUTERNAME") + ".event." + ( m_WriteBinaryFormat ? "dat" : "txt" );

    eventsFilePath = m_RootDirPath.Get() + fileName;
    Nocturnal::Path::Normalize( eventsFilePath );
}


/////////////////////////////////////////////////////////////////////////////
// Default constructor
//
EventSystem::~EventSystem( )
{
}


/////////////////////////////////////////////////////////////////////////////
// Creates a new event record
//
EventPtr EventSystem::CreateEvent( const std::string &eventData, const std::string& username )
{
    EVENTSYSTEM_SCOPE_TIMER((""));

    // get the current time
    __timeb64 now;
    _ftime64_s( &now );
    u64 createdTime = ( now.time * 1000 ) + now.millitm;

  return new Event( TUID::Generate(), createdTime, username, eventData ); 
}


/////////////////////////////////////////////////////////////////////////////
// Used to by GetUnhandledEvents (remove_if) to remove Events from a vector where
// Event Id is in the set of already handled Event ids.
//
struct FilterHandledEvents
{
    const S_tuid& m_HandledEventIds; // Set of already handled event ids

    // () operater overloaded to be used by vector's remove_if
    //param event Event to look for in the set of handled events
    //return bool, true if the given Event's id is in the list of handled Events
    bool operator()( const EventPtr& event )
    {
        return m_HandledEventIds.find( event->m_Id ) != m_HandledEventIds.end();
    }

    // Constructor is used populate the m_HandledEventIds
    FilterHandledEvents( const S_tuid& eventIds )
        : m_HandledEventIds( eventIds ) {}
};

/////////////////////////////////////////////////////////////////////////////
// Gets the list of all events in eventsDirPath.
//
void EventSystem::GetUnhandledEvents( V_EventPtr &listOfEvents )
{
    EVENTSYSTEM_SCOPE_TIMER((""));

    S_tuid handledEventIDs;
    if ( m_HandledEventsFile.Exists() )
    {
        std::ifstream handledEventsFile( m_HandledEventsFile.c_str() );
        if ( !handledEventsFile.is_open() )
        {
            throw Exception( "Could not open handled events file for read: %s", m_HandledEventsFile.c_str() );
        }

        tuid tempEventId;
        while ( handledEventsFile.good() )
        {
            handledEventsFile >> tempEventId;
            handledEventIDs.insert( tempEventId );
        }
        handledEventsFile.close();
    }

    GetUnhandledEvents( listOfEvents, handledEventIDs );
}

/////////////////////////////////////////////////////////////////////////////
void EventSystem::GetUnhandledEvents( V_EventPtr& listOfEvents, S_tuid& handledEventIDs )
{
    EVENTSYSTEM_SCOPE_TIMER((""));

    // Get the list of events
    GetEvents( listOfEvents );

    if ( !handledEventIDs.empty() )
    {
        // Filter the list of events on the list of already handled events
        V_EventPtr::iterator it = std::remove_if( listOfEvents.begin(), listOfEvents.end(), FilterHandledEvents( handledEventIDs ) );
        listOfEvents.erase( it, listOfEvents.end() );
    }

    std::sort( listOfEvents.begin(), listOfEvents.end(), SortEvents() );
}

/////////////////////////////////////////////////////////////////////////////
// Gets the list of all events in eventsDirPath.
//
void EventSystem::GetEvents( V_EventPtr& listOfEvents, bool sorted )
{
    EVENTSYSTEM_SCOPE_TIMER((""));

    // Binary events
    Nocturnal::S_Path datEventFiles;
    Nocturnal::Directory::GetFiles( m_RootDirPath, datEventFiles, "*.event.dat", true );

    Nocturnal::S_Path::iterator itr = datEventFiles.begin();
    Nocturnal::S_Path::iterator end = datEventFiles.end();
    for( ; itr != end; ++itr )
    {
        const Nocturnal::Path& filePath = (*itr);
        if ( filePath.IsFile() )
            ReadBinaryEventsFile( filePath.Get(), listOfEvents, false );
    }

    // Text events
    Nocturnal::S_Path txtEventFiles;
    Nocturnal::Directory::GetFiles( m_RootDirPath, txtEventFiles, "*.event.txt", true );

    itr = txtEventFiles.begin();
    end = txtEventFiles.end();
    for( ; itr != end; ++itr )
    {
        const Nocturnal::Path& filePath = (*itr);
        if ( filePath.IsFile() )
            ReadTextEventsFile( filePath.Get(), listOfEvents, false );
    }

    if (sorted)
    {
        std::sort( listOfEvents.begin(), listOfEvents.end(), SortEvents() );
    }
}


void EventSystem::ReadEventsFile( const std::string& eventsFile, V_EventPtr& listOfEvents, bool sorted )
{
    EVENTSYSTEM_SCOPE_TIMER((""));

    Nocturnal::Path path( eventsFile );
    if ( path.Extension() == FinderSpecs::Extension::DATA.GetExtension() )
    {
        ReadBinaryEventsFile( eventsFile, listOfEvents, sorted );
    }
    else if ( path.Extension() == FinderSpecs::Extension::TXT.GetExtension() )
    {
        ReadTextEventsFile( eventsFile, listOfEvents, sorted );
    }
    else
    {
        throw Exception( "Unknown file type of file: %s", eventsFile.c_str() );
    }
}

/////////////////////////////////////////////////////////////////////////////
// Gets the list of all events in eventsDirPath.
//
void EventSystem::ReadBinaryEventsFile( const std::string& eventsFile, V_EventPtr& listOfEvents, bool sorted )
{
    EVENTSYSTEM_SCOPE_TIMER((""));

    std::ifstream recordsFile( eventsFile.c_str(), std::ios::in | std::ios::binary ); 
    if ( !recordsFile.is_open() )
    {
        throw Exception( "Could not open events file for reading: %s", eventsFile.c_str() );
    }

    // read in the recordCount for this file
    int recordCount = 0;
    if ( recordsFile.read( ( char * ) &recordCount, sizeof( RecordCount ) ).fail() )
    {
        recordsFile.close();
        throw Exception( "Could not read events file: %s", eventsFile.c_str() );
    }

    // resize the listOfEvents first and then insert into it to save time
    size_t loeOffset = listOfEvents.size();
    listOfEvents.resize( loeOffset + recordCount );

    // read the events
    V_EventPtr::iterator itCurrentEvent = listOfEvents.begin() + loeOffset;
    V_EventPtr::iterator itEndEvents = listOfEvents.end();
    for (  ; itCurrentEvent != itEndEvents ; ++itCurrentEvent )
    {
        bool isReadOk = true;

        tuid id;
        isReadOk = isReadOk && !recordsFile.read( ( char * ) &id, sizeof( id ) ).fail();

        u64 created;
        isReadOk = isReadOk && !recordsFile.read( ( char * ) &created, sizeof( created ) ).fail();

        // no errors yet, read the event computerName
        std::string username;
        int userClientNameLength;
        isReadOk = isReadOk && !recordsFile.read( ( char * ) &userClientNameLength, sizeof( userClientNameLength ) ).fail();
        if ( isReadOk && userClientNameLength > 0 )
        {
            username.resize( userClientNameLength );
            std::string::iterator eventComputerNameBegin = username.begin();
            isReadOk = isReadOk && !recordsFile.read( ( char * ) &(*eventComputerNameBegin), userClientNameLength ).fail();
        }

        // no errors yet, read the event data
        std::string data;
        int eventLength;
        isReadOk = isReadOk && !recordsFile.read( ( char * ) &eventLength, sizeof( eventLength ) ).fail();
        if ( isReadOk && eventLength > 0 )
        {
            data.resize( eventLength );
            std::string::iterator eventDataBegin = data.begin();
            isReadOk = isReadOk && !recordsFile.read( ( char * ) &(*eventDataBegin), eventLength ).fail();
        }

        // read errors occurred
        if ( !isReadOk )
        {
            recordsFile.close();
            // remove extra elements added to listOfEvents in the above resize
            listOfEvents.resize( itCurrentEvent - listOfEvents.begin() - 1 ); // FIXME test for obo (-1)

            throw Exception( "Errors occurred while reading events file: %s", eventsFile.c_str() );
        }

        (*itCurrentEvent) = new Event( id, created, username, data );
    }

    recordsFile.close();

    if (sorted)
    {
        std::sort( listOfEvents.begin(), listOfEvents.end(), SortEvents() );
    }
}

/////////////////////////////////////////////////////////////////////////////
// Gets the list of all events in eventsDirPath.
//
void EventSystem::ReadTextEventsFile( const std::string& eventsFile, V_EventPtr& listOfEvents, bool sorted )
{
    EVENTSYSTEM_SCOPE_TIMER((""));

    // Open the file for read
    std::ifstream recordsFile( eventsFile.c_str(), std::ios::in );
    if ( !recordsFile.is_open() )
    {
        throw Exception( "Could not open events file for reading: %s", eventsFile.c_str() );
        return;
    }

    // read in the recordCount for this file
    //int recordCount = 0;
    //std::string recordCountStr;
    //if ( std::getline( recordsFile, recordCountStr ).fail() )
    //{
    //  recordsFile.close();
    //  throw Exception( "Could not read events file: %s", eventsFile.c_str() );
    //}
    //recordCount = atoi( recordCountStr.c_str() );

    // resize the listOfEvents first and then insert into it to save time
    //size_t loeOffset = listOfEvents.size();
    //listOfEvents.resize( loeOffset + recordCount );

    // read the events
    bool isReadOk = true;
    std::string line;
    //V_EventPtr::iterator itCurrentEvent = listOfEvents.begin() + loeOffset;
    //V_EventPtr::iterator itEndEvents = listOfEvents.end();
    //for (  ; itCurrentEvent != itEndEvents ; ++itCurrentEvent )
    while ( isReadOk && recordsFile.good() && !std::getline( recordsFile, line ).fail() )
    {
        if ( line.empty() )
        {
            continue;
        }

        // ignore the header and comments
        if ( *line.begin() == '#' )
        {
            continue;
        }

        /////////////////////////////////////////////
        // parse the event string
        // Event: <string created>|<i64 id>|<i64 created>|<string username>|<int data string size>
        //
        // Example:
        // "Event: Thu Mar 27 14:16:19 2008|7301583699290099363|1206652579300|rachel-RMARK_PC|35"
        const boost::regex s_EventTextFileStringPattern( "^\\s*Event\\:\\s*.*?\\|(\\d+)\\|(\\d*)\\|(.*?)\\|(\\d+)\\s*$" );

        boost::cmatch eventResultAttr; 
        if ( !boost::regex_match(line.c_str(), eventResultAttr, s_EventTextFileStringPattern))
        {
            // remove extra elements added to listOfEvents in the above resize
            //listOfEvents.resize( itCurrentEvent - listOfEvents.begin() - 1 );
            recordsFile.close();
            throw Exception( "Could not parse text event: %s", line.c_str() );
        }

        tuid id = ( tuid ) Nocturnal::BoostMatchResult<tuid>(eventResultAttr, 1); 
        u64 created = ( u64 ) Nocturnal::BoostMatchResult<u64>(eventResultAttr, 2); 
        std::string username = Nocturnal::BoostMatchResultAsString(eventResultAttr, 3); 
        i32 eventLength = ( i32 ) Nocturnal::BoostMatchResult<i32>(eventResultAttr, 4); 

        /////////////////////////////////////////////
        // parse the event data
        // <data string>

        // no errors yet, read the event data
        std::string data;
        if ( isReadOk && eventLength > 0 )
        {
            data.resize( eventLength );
            std::string::iterator eventDataBegin = data.begin();
            isReadOk = isReadOk && !recordsFile.read( ( char * ) &(*eventDataBegin), eventLength ).fail();
        }

        if ( !isReadOk )
        {
            // remove extra elements added to listOfEvents in the above resize
            //listOfEvents.resize( itCurrentEvent - listOfEvents.begin() - 1 );
            recordsFile.close();
            throw Exception( "Errors occurred while reading events file: %s", eventsFile.c_str() );
        }

        //(*itCurrentEvent) = new Event( id, created, username, data );
        listOfEvents.push_back( new Event( id, created, username, data ) );
    }

    /////////////////////////////////////////////
    // parse the event data
    // <data string>

    if ( !isReadOk )
    {
        recordsFile.close();
        throw Exception( "Errors occurred while reading events file: %s", eventsFile.c_str() );
    }

    if (sorted)
    {
        std::sort( listOfEvents.begin(), listOfEvents.end(), SortEvents() );
    }
}





/////////////////////////////////////////////////////////////////////////////
// Writes a list of events to the event file.
//
void EventSystem::WriteEventsFile( const std::string& eventsFile, const V_EventPtr& listOfEvents )
{
    EVENTSYSTEM_SCOPE_TIMER((""));

    Nocturnal::Path path( eventsFile );
    if ( path.Extension() == FinderSpecs::Extension::DATA.GetExtension() )
    {
        WriteBinaryEventsFile( eventsFile, listOfEvents );
    }
    else if ( path.Extension() == FinderSpecs::Extension::TXT.GetExtension() )
    {
        WriteTextEventsFile( eventsFile, listOfEvents );
    }
    else
    {
        throw Exception( "Unknown file type of file: %s", eventsFile.c_str() );
    }
}


void EventSystem::WriteBinaryEventsFile( const std::string& eventsFile, const V_EventPtr& listOfEvents )
{
    EVENTSYSTEM_SCOPE_TIMER((""));

    if ( listOfEvents.empty() )
    {
        return;
    }

    Nocturnal::Path path( eventsFile );
    // make sure it's not read only
    if ( path.Exists() && !path.Writable() )
    {
        throw Exception( "[%s] is read-only!", eventsFile.c_str() );
    }

    // Open the record file for append and add the new record
    RecordCount recordCount = 0;
    std::fstream recordsFile( eventsFile.c_str(), std::ios::in | std::ios::out |std::ios::binary );
    if ( !recordsFile.fail() )
    {
        // read and increment the recordCount
        recordsFile.seekg( 0, std::ios::beg );
        recordsFile.read( ( char * ) &recordCount, sizeof( RecordCount ) );
        recordsFile.clear();
    }
    else
    {
        recordsFile.close();
        recordsFile.clear();
        recordsFile.open( eventsFile.c_str(), std::ios::out | std::ios::binary );
    }

    // add the new events the the record count
    recordCount += (RecordCount) listOfEvents.size();  

    bool isWriteOk = true;

    // write the recordCount
    isWriteOk = isWriteOk && !recordsFile.seekp( 0, std::ios::beg ).fail();
    isWriteOk = isWriteOk && !recordsFile.write( ( char * ) &recordCount, sizeof( RecordCount ) ).fail();

    // Append the event to the end of the file
    isWriteOk = isWriteOk && !recordsFile.seekp( 0, std::ios::end ).fail();
    for each ( const EventPtr& event in listOfEvents )
    {
        if ( !isWriteOk )
        {
            break;
        }

        isWriteOk = isWriteOk && !recordsFile.write( ( char * ) &event->m_Id, sizeof( event->m_Id ) ).fail();
        isWriteOk = isWriteOk && !recordsFile.write( ( char * ) &event->m_Created, sizeof( event->m_Created ) ).fail();

        int userClientNameLength = ( int ) event->m_Username.length();
        isWriteOk = isWriteOk && !recordsFile.write( ( char * ) &userClientNameLength, sizeof( userClientNameLength ) ).fail();
        isWriteOk = isWriteOk && !recordsFile.write( event->m_Username.c_str(), userClientNameLength ).fail();

        int dataLength = ( int ) event->m_Data.length();
        isWriteOk = isWriteOk && !recordsFile.write( ( char * ) &dataLength, sizeof( dataLength ) ).fail();
        isWriteOk = isWriteOk && !recordsFile.write( event->m_Data.c_str(), dataLength ).fail();
    }

    // write errors occurred
    if ( !isWriteOk )
    {
        throw Exception( "Could not write to file: %s", eventsFile.c_str() );
    }
}


void EventSystem::WriteTextEventsFile( const std::string& eventsFile, const V_EventPtr& listOfEvents )
{
    EVENTSYSTEM_SCOPE_TIMER((""));

    if ( listOfEvents.empty() )
    {
        return;
    }

    Nocturnal::Path path( eventsFile );
    if ( path.Exists() && !path.Writable() )
    {
        throw Exception( "[%s] is read-only!", eventsFile.c_str() );
    }

    // open file for write, 
    std::ofstream recordsFile( eventsFile.c_str(), std::ios::out );
    if ( !recordsFile.is_open() )
    {
        throw Exception( "Could not write to text file: %s", eventsFile.c_str() );
    }

    // if this is a new file, write the header
    if ( path.Size() == 0 )
    {
        std::stringstream headerTextStr;
        headerTextStr << "#------------------------------------------------------------" << std::endl;
        headerTextStr << "# " << std::endl;
        headerTextStr << "#                 *** DO NOT EDIT THIS FILE ***              " << std::endl;
        headerTextStr << "# " << std::endl;
        headerTextStr << "# This file was generated by the EventSystem and should NOT " << std::endl;
        headerTextStr << "# be edited by hand. " << std::endl;
        headerTextStr << "# " << std::endl;
        headerTextStr << "# Doing so may result in data loss and/or file corruption!" << std::endl;
        headerTextStr << "# " << std::endl;
        headerTextStr << "#------------------------------------------------------------" << std::endl;

        int headerTextStrLength = ( int ) headerTextStr.str().length();

        if ( recordsFile.write( headerTextStr.str().c_str(), headerTextStrLength ).fail() )
        {
            throw Exception( "Could not write to file: %s", eventsFile.c_str() );
        }
    }

    char timePrint[TIME_SIZE];

    // Write out the events
    bool isWriteOk = true;
    for each ( const ES::EventPtr& event in listOfEvents )
    {
        if ( !isWriteOk )
        {
            recordsFile.close();
            throw Exception( "Could not write to file: %s", eventsFile.c_str() );
        }

        // build the event string
        std::stringstream eventTextStr;

        // Event: <string created>|<i64 id>|<i64 created>|<string username>|<int data string size>
        eventTextStr << "Event: " ;

        // try to get a printer friendly version of the dates
        __time64_t eventCreated  = ( __time64_t ) ( event->m_Created / 1000 );
        if ( _ctime64_s( timePrint, TIME_SIZE, &eventCreated ) == 0 )
        {
            std::string timePrintStr( timePrint );
            timePrintStr.erase( 24, 2 );
            eventTextStr << timePrintStr;
        }
        else
        {
            eventTextStr << eventCreated;
        }

        eventTextStr << "|" << event->m_Id << "|" << event->m_Created << "|" << event->m_Username << "|" << event->m_Data.length() << std::endl;

        // <data string>
        eventTextStr << event->m_Data << std::endl;

        eventTextStr << "#------------------------------------------------------------" << std::endl;

        // write out the event
        int eventTextStrLength = ( int ) eventTextStr.str().length();
        isWriteOk = isWriteOk && !recordsFile.write( eventTextStr.str().c_str(), eventTextStrLength ).fail();
    }
    recordsFile.close();

    // write errors occurred
    if ( !isWriteOk )
    {
        throw Exception( "Could not write to file: %s", eventsFile.c_str() );
    }
}


/////////////////////////////////////////////////////////////////////////////
// Records a list of TUIDs in the handledEventsFileName, newline seperated list.
//
void EventSystem::WriteHandledEvents( const V_EventPtr& listOfEvents )
{
    EVENTSYSTEM_SCOPE_TIMER((""));

    if ( listOfEvents.empty() )
    {
        return;
    }

    if ( m_HandledEventsFile.Exists() && !m_HandledEventsFile.Writable() )
    {
        throw Exception( "[%s] is read-only!", m_HandledEventsFile.c_str() );
    }

    std::ofstream handledEventsFile( m_HandledEventsFile.c_str(), std::ios::app );
    if ( !handledEventsFile.is_open() )
    {
        throw Exception( "Could not open file: %s", m_HandledEventsFile.c_str() );
    }

    for each ( const EventPtr& event in listOfEvents )
    {
        handledEventsFile << event->m_Id << "\n";
    }

    handledEventsFile.close();
}

/////////////////////////////////////////////////////////////////////////////
// Clears the list of handled events
//
void EventSystem::FlushHandledEvents()
{
    EVENTSYSTEM_SCOPE_TIMER((""));

    if ( m_HandledEventsFile.Exists() )
    {
        if ( !m_HandledEventsFile.Delete() )
        {
            throw PlatformException( "Could not delete handled events file: %s", m_HandledEventsFile.c_str() );
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
// Records a TUID in the handledEventsFileName, newline seperated list.
//
bool EventSystem::HandleEventsFileExists()
{
    EVENTSYSTEM_SCOPE_TIMER((""));

    return m_HandledEventsFile.Exists();
}


/////////////////////////////////////////////////////////////////////////////
// Dumps the event.dat file to a human readable event.txt file
// 
void EventSystem::DumpEventsToTextFile( const std::string& datFile, const std::string& txtFile )
{
    EVENTSYSTEM_SCOPE_TIMER((""));

    Nocturnal::Path path( datFile );
    // setup the input file
    if ( !path.Exists() )
    {
        throw Exception( "Input events file [%s] doesn't exist!", datFile.c_str() );
    }

    // set up the output file
    std::string cleanTextFile = txtFile;
    if ( cleanTextFile.empty() )
    {
        cleanTextFile = datFile;
        FinderSpecs::Extension::TXT.Modify( cleanTextFile );
    }
    Nocturnal::Path cleanTextFilePath( cleanTextFile );
    cleanTextFilePath.Create();

    // get this user's events
    ES::V_EventPtr listOfEvents;
    ReadBinaryEventsFile( datFile, listOfEvents, true );
    WriteTextEventsFile( cleanTextFile, listOfEvents );
}


/////////////////////////////////////////////////////////////////////////////
// Loads the event.dat file from the event.txt file
//
void EventSystem::LoadEventsFromTextFile( const std::string& txtFile, const std::string& datFile )
{
    EVENTSYSTEM_SCOPE_TIMER((""));

    Nocturnal::Path path( txtFile );
    // setup the input file
    if ( !path.Exists() )
    {
        throw Exception( "Input text events file [%s] doesn't exist!", txtFile.c_str() );
    }

    // setup the output file
    Nocturnal::Path cleanEventsFile( datFile );
    if ( cleanEventsFile.Get().empty() )
    {
        cleanEventsFile.Set( txtFile );
        cleanEventsFile.ReplaceExtension( "event.dat" );
    }
    cleanEventsFile.Create();

    if ( !cleanEventsFile.Writable() )
    {
        throw Exception( "Output events file [%s] is read-only!", cleanEventsFile.c_str() );
    }

    // read the events
    ES::V_EventPtr listOfEvents;
    ReadTextEventsFile( txtFile, listOfEvents, true );

    StompEventsFile( cleanEventsFile.Get(), listOfEvents );
}


/////////////////////////////////////////////////////////////////////////////
// Deletes the user's current event's file and re-populates it with 
// the given list of events.
//
void EventSystem::StompEventsFile( const std::string& eventsFile, const V_EventPtr& listOfEvents )
{
    EVENTSYSTEM_SCOPE_TIMER((""));

    Nocturnal::Path path( eventsFile );
    // make sure it's not read only
    if ( !path.Writable() )
    {
        throw Exception( "Could not write to events, file is read-only: %s", eventsFile.c_str() );
    }

    // Open the record file and truncate to clear its contents
    {
        std::ofstream recordsFile( eventsFile.c_str(), std::ios::out | std::ios::binary | std::ios::trunc );
        if ( !recordsFile.is_open() )
        {
            throw Exception( "Could not open events file for write: %s", eventsFile.c_str() );
        }

        bool isWriteOk = true;

        // write the recordCount 0 
        RecordCount recordCount = 0;
        isWriteOk = isWriteOk && !recordsFile.seekp( 0, std::ios::beg ).fail();
        isWriteOk = isWriteOk && !recordsFile.write( ( char * ) &recordCount, sizeof( RecordCount ) ).fail();

        // write errors occurred
        if ( !isWriteOk )
        {
            recordsFile.close();

            throw Exception( "Could not write to events file: %s", eventsFile.c_str() );
        }

        recordsFile.close();
    }

    // now populate the event.dat file with the new list of events
    WriteEventsFile( eventsFile, listOfEvents );
}
