#include "Platform/Windows/Windows.h"
#include "Platform/Exception.h"
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
#include "Foundation/Regex.h" 
#include "Platform/Types.h"
#include "Foundation/File/Directory.h"
#include "Foundation/String/Utilities.h"
#include "Foundation/Log.h"

using namespace Helium::ES;


#define TIME_SIZE 32


typedef i32 RecordCount;

static const tchar* s_HandledEventsFilename = TXT( "handled_events.txt" );


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
EventSystem::EventSystem( const tstring &rootDirPath, bool writeBinaryFormat )
: m_RootDirPath( rootDirPath )
, m_WriteBinaryFormat( writeBinaryFormat )
{
    m_RootDirPath.MakePath();
    m_HandledEventsFile.Set( m_RootDirPath.Get() + TXT( "/" ) + s_HandledEventsFilename );
}


void EventSystem::CreateEventsFilePath( tstring& eventsFilePath )
{
    tstring fileName = tstring( _tgetenv( TXT( "USERNAME" ) ) ) + TXT( '-' ) + _tgetenv( TXT( "COMPUTERNAME" ) ) + TXT( ".event." ) + ( m_WriteBinaryFormat ? TXT( "dat" ) : TXT( "txt" ) );

    eventsFilePath = m_RootDirPath.Get() + fileName;
    Helium::Path::Normalize( eventsFilePath );
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
EventPtr EventSystem::CreateEvent( const tstring &eventData, const tstring& username )
{
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
    S_tuid handledEventIDs;
    if ( m_HandledEventsFile.Exists() )
    {
        std::ifstream handledEventsFile( m_HandledEventsFile.c_str() );
        if ( !handledEventsFile.is_open() )
        {
            throw Exception( TXT( "Could not open handled events file for read: %s" ), m_HandledEventsFile.c_str() );
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
    // Binary events
    std::set< Helium::Path > datEventFiles;
    Helium::Directory::GetFiles( m_RootDirPath, datEventFiles, TXT( "*.event.dat" ), true );

    std::set< Helium::Path >::iterator itr = datEventFiles.begin();
    std::set< Helium::Path >::iterator end = datEventFiles.end();
    for( ; itr != end; ++itr )
    {
        const Helium::Path& filePath = (*itr);
        if ( filePath.IsFile() )
            ReadBinaryEventsFile( filePath.Get(), listOfEvents, false );
    }

    // Text events
    std::set< Helium::Path > txtEventFiles;
    Helium::Directory::GetFiles( m_RootDirPath, txtEventFiles, TXT( "*.event.txt" ), true );

    itr = txtEventFiles.begin();
    end = txtEventFiles.end();
    for( ; itr != end; ++itr )
    {
        const Helium::Path& filePath = (*itr);
        if ( filePath.IsFile() )
            ReadTextEventsFile( filePath.Get(), listOfEvents, false );
    }

    if (sorted)
    {
        std::sort( listOfEvents.begin(), listOfEvents.end(), SortEvents() );
    }
}


void EventSystem::ReadEventsFile( const tstring& eventsFile, V_EventPtr& listOfEvents, bool sorted )
{
    Helium::Path path( eventsFile );
    if ( path.Extension() == TXT( "dat" ) )
    {
        ReadBinaryEventsFile( eventsFile, listOfEvents, sorted );
    }
    else if ( path.Extension() == TXT( "txt" ) )
    {
        ReadTextEventsFile( eventsFile, listOfEvents, sorted );
    }
    else
    {
        throw Exception( TXT( "Unknown file type of file: %s" ), eventsFile.c_str() );
    }
}

/////////////////////////////////////////////////////////////////////////////
// Gets the list of all events in eventsDirPath.
//
void EventSystem::ReadBinaryEventsFile( const tstring& eventsFile, V_EventPtr& listOfEvents, bool sorted )
{
    std::ifstream recordsFile( eventsFile.c_str(), std::ios::in | std::ios::binary ); 
    if ( !recordsFile.is_open() )
    {
        throw Exception( TXT( "Could not open events file for reading: %s" ), eventsFile.c_str() );
    }

    // read in the recordCount for this file
    int recordCount = 0;
    if ( recordsFile.read( ( char * ) &recordCount, sizeof( RecordCount ) ).fail() )
    {
        recordsFile.close();
        throw Exception( TXT( "Could not read events file: %s" ), eventsFile.c_str() );
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
        tstring username;
        int userClientNameLength;
        isReadOk = isReadOk && !recordsFile.read( ( char * ) &userClientNameLength, sizeof( userClientNameLength ) ).fail();
        if ( isReadOk && userClientNameLength > 0 )
        {
            username.resize( userClientNameLength );
            tstring::iterator eventComputerNameBegin = username.begin();
            isReadOk = isReadOk && !recordsFile.read( ( char * ) &(*eventComputerNameBegin), userClientNameLength ).fail();
        }

        // no errors yet, read the event data
        tstring data;
        int eventLength;
        isReadOk = isReadOk && !recordsFile.read( ( char * ) &eventLength, sizeof( eventLength ) ).fail();
        if ( isReadOk && eventLength > 0 )
        {
            data.resize( eventLength );
            tstring::iterator eventDataBegin = data.begin();
            isReadOk = isReadOk && !recordsFile.read( ( char * ) &(*eventDataBegin), eventLength ).fail();
        }

        // read errors occurred
        if ( !isReadOk )
        {
            recordsFile.close();
            // remove extra elements added to listOfEvents in the above resize
            listOfEvents.resize( itCurrentEvent - listOfEvents.begin() - 1 ); // FIXME test for obo (-1)

            throw Exception( TXT( "Errors occurred while reading events file: %s" ), eventsFile.c_str() );
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
void EventSystem::ReadTextEventsFile( const tstring& eventsFile, V_EventPtr& listOfEvents, bool sorted )
{
    // Open the file for read
    tifstream recordsFile( eventsFile.c_str(), std::ios::in );
    if ( !recordsFile.is_open() )
    {
        throw Exception( TXT( "Could not open events file for reading: %s" ), eventsFile.c_str() );
        return;
    }

    // read the events
    bool isReadOk = true;
    tstring line;
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
        const tregex s_EventTextFileStringPattern( TXT("^\\s*Event\\:\\s*.*?\\|(\\d+)\\|(\\d*)\\|(.*?)\\|(\\d+)\\s*$") );

        tcmatch eventResultAttr; 
        if ( !std::tr1::regex_match(line.c_str(), eventResultAttr, s_EventTextFileStringPattern))
        {
            // remove extra elements added to listOfEvents in the above resize
            //listOfEvents.resize( itCurrentEvent - listOfEvents.begin() - 1 );
            recordsFile.close();
            throw Exception( TXT( "Could not parse text event: %s" ), line.c_str() );
        }

        tuid id = ( tuid ) Helium::MatchResult<tuid>(eventResultAttr, 1); 
        u64 created = ( u64 ) Helium::MatchResult<u64>(eventResultAttr, 2); 
        tstring username = Helium::MatchResultAsString(eventResultAttr, 3); 
        i32 eventLength = ( i32 ) Helium::MatchResult<i32>(eventResultAttr, 4); 

        /////////////////////////////////////////////
        // parse the event data
        // <data string>

        // no errors yet, read the event data
        tstring data;
        if ( isReadOk && eventLength > 0 )
        {
            data.resize( eventLength );
            tstring::iterator eventDataBegin = data.begin();
            isReadOk = isReadOk && !recordsFile.read( ( tchar * ) &(*eventDataBegin), eventLength ).fail();
        }

        if ( !isReadOk )
        {
            // remove extra elements added to listOfEvents in the above resize
            //listOfEvents.resize( itCurrentEvent - listOfEvents.begin() - 1 );
            recordsFile.close();
            throw Exception( TXT( "Errors occurred while reading events file: %s" ), eventsFile.c_str() );
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
        throw Exception( TXT( "Errors occurred while reading events file: %s" ), eventsFile.c_str() );
    }

    if (sorted)
    {
        std::sort( listOfEvents.begin(), listOfEvents.end(), SortEvents() );
    }
}





/////////////////////////////////////////////////////////////////////////////
// Writes a list of events to the event file.
//
void EventSystem::WriteEventsFile( const tstring& eventsFile, const V_EventPtr& listOfEvents )
{
    Helium::Path path( eventsFile );
    if ( path.Extension() == TXT( "dat" ) )
    {
        WriteBinaryEventsFile( eventsFile, listOfEvents );
    }
    else if ( path.Extension() == TXT( "txt" ) )
    {
        WriteTextEventsFile( eventsFile, listOfEvents );
    }
    else
    {
        throw Exception( TXT( "Unknown file type of file: %s" ), eventsFile.c_str() );
    }
}


void EventSystem::WriteBinaryEventsFile( const tstring& eventsFile, const V_EventPtr& listOfEvents )
{
    if ( listOfEvents.empty() )
    {
        return;
    }

    Helium::Path path( eventsFile );
    // make sure it's not read only
    if ( path.Exists() && !path.Writable() )
    {
        throw Exception( TXT( "[%s] is read-only!" ), eventsFile.c_str() );
    }

    // Open the record file for append and add the new record
    RecordCount recordCount = 0;
    tfstream recordsFile( eventsFile.c_str(), std::ios::in | std::ios::out |std::ios::binary );
    if ( !recordsFile.fail() )
    {
        // read and increment the recordCount
        recordsFile.seekg( 0, std::ios::beg );
        recordsFile.read( ( tchar * ) &recordCount, sizeof( RecordCount ) );
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
    isWriteOk = isWriteOk && !recordsFile.write( ( tchar * ) &recordCount, sizeof( RecordCount ) ).fail();

    // Append the event to the end of the file
    isWriteOk = isWriteOk && !recordsFile.seekp( 0, std::ios::end ).fail();
    for each ( const EventPtr& event in listOfEvents )
    {
        if ( !isWriteOk )
        {
            break;
        }

        isWriteOk = isWriteOk && !recordsFile.write( ( tchar * ) &event->m_Id, sizeof( event->m_Id ) ).fail();
        isWriteOk = isWriteOk && !recordsFile.write( ( tchar * ) &event->m_Created, sizeof( event->m_Created ) ).fail();

        int userClientNameLength = ( int ) event->m_Username.length();
        isWriteOk = isWriteOk && !recordsFile.write( ( tchar * ) &userClientNameLength, sizeof( userClientNameLength ) ).fail();
        isWriteOk = isWriteOk && !recordsFile.write( event->m_Username.c_str(), userClientNameLength ).fail();

        int dataLength = ( int ) event->m_Data.length();
        isWriteOk = isWriteOk && !recordsFile.write( ( tchar * ) &dataLength, sizeof( dataLength ) ).fail();
        isWriteOk = isWriteOk && !recordsFile.write( event->m_Data.c_str(), dataLength ).fail();
    }

    // write errors occurred
    if ( !isWriteOk )
    {
        throw Exception( TXT( "Could not write to file: %s" ), eventsFile.c_str() );
    }
}


void EventSystem::WriteTextEventsFile( const tstring& eventsFile, const V_EventPtr& listOfEvents )
{
    if ( listOfEvents.empty() )
    {
        return;
    }

    Helium::Path path( eventsFile );
    if ( path.Exists() && !path.Writable() )
    {
        throw Exception( TXT( "[%s] is read-only!" ), eventsFile.c_str() );
    }

    // open file for write, 
    tofstream recordsFile( eventsFile.c_str(), std::ios::out );
    if ( !recordsFile.is_open() )
    {
        throw Exception( TXT( "Could not write to text file: %s" ), eventsFile.c_str() );
    }

    // if this is a new file, write the header
    if ( path.Size() == 0 )
    {
        tstringstream headerTextStr;
        headerTextStr << TXT( "#------------------------------------------------------------" ) << std::endl;
        headerTextStr << TXT( "# " ) << std::endl;
        headerTextStr << TXT( "#                 *** DO NOT EDIT THIS FILE ***              " ) << std::endl;
        headerTextStr << TXT( "# " ) << std::endl;
        headerTextStr << TXT( "# This file was generated by the EventSystem and should NOT " ) << std::endl;
        headerTextStr << TXT( "# be edited by hand. " ) << std::endl;
        headerTextStr << TXT( "# " ) << std::endl;
        headerTextStr << TXT( "# Doing so may result in data loss and/or file corruption!" ) << std::endl;
        headerTextStr << TXT( "# " ) << std::endl;
        headerTextStr << TXT( "#------------------------------------------------------------" ) << std::endl;

        int headerTextStrLength = ( int ) headerTextStr.str().length();

        if ( recordsFile.write( headerTextStr.str().c_str(), headerTextStrLength ).fail() )
        {
            throw Exception( TXT( "Could not write to file: %s" ), eventsFile.c_str() );
        }
    }

    tchar timePrint[TIME_SIZE];

    // Write out the events
    bool isWriteOk = true;
    for each ( const ES::EventPtr& event in listOfEvents )
    {
        if ( !isWriteOk )
        {
            recordsFile.close();
            throw Exception( TXT( "Could not write to file: %s" ), eventsFile.c_str() );
        }

        // build the event string
        tstringstream eventTextStr;

        // Event: <string created>|<i64 id>|<i64 created>|<string username>|<int data string size>
        eventTextStr << TXT( "Event: " );

        // try to get a printer friendly version of the dates
        __time64_t eventCreated  = ( __time64_t ) ( event->m_Created / 1000 );
        if ( _tctime64_s( timePrint, TIME_SIZE, &eventCreated ) == 0 )
        {
            tstring timePrintStr( timePrint );
            timePrintStr.erase( 24, 2 );
            eventTextStr << timePrintStr;
        }
        else
        {
            eventTextStr << eventCreated;
        }

        eventTextStr << TXT( "|" ) << event->m_Id << TXT( "|" ) << event->m_Created << TXT( "|" ) << event->m_Username << TXT( "|" ) << event->m_Data.length() << std::endl;

        // <data string>
        eventTextStr << event->m_Data << std::endl;

        eventTextStr << TXT( "#------------------------------------------------------------" ) << std::endl;

        // write out the event
        int eventTextStrLength = ( int ) eventTextStr.str().length();
        isWriteOk = isWriteOk && !recordsFile.write( eventTextStr.str().c_str(), eventTextStrLength ).fail();
    }
    recordsFile.close();

    // write errors occurred
    if ( !isWriteOk )
    {
        throw Exception( TXT( "Could not write to file: %s" ), eventsFile.c_str() );
    }
}


/////////////////////////////////////////////////////////////////////////////
// Records a list of TUIDs in the handledEventsFileName, newline seperated list.
//
void EventSystem::WriteHandledEvents( const V_EventPtr& listOfEvents )
{
    if ( listOfEvents.empty() )
    {
        return;
    }

    if ( m_HandledEventsFile.Exists() && !m_HandledEventsFile.Writable() )
    {
        throw Exception( TXT( "[%s] is read-only!" ), m_HandledEventsFile.c_str() );
    }

    std::ofstream handledEventsFile( m_HandledEventsFile.c_str(), std::ios::app );
    if ( !handledEventsFile.is_open() )
    {
        throw Exception( TXT( "Could not open file: %s" ), m_HandledEventsFile.c_str() );
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
    if ( m_HandledEventsFile.Exists() )
    {
        if ( !m_HandledEventsFile.Delete() )
        {
            throw PlatformException( TXT( "Could not delete handled events file: %s" ), m_HandledEventsFile.c_str() );
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
// Records a TUID in the handledEventsFileName, newline seperated list.
//
bool EventSystem::HandleEventsFileExists()
{
    return m_HandledEventsFile.Exists();
}


/////////////////////////////////////////////////////////////////////////////
// Dumps the event.dat file to a human readable event.txt file
// 
void EventSystem::DumpEventsToTextFile( const tstring& datFile, const tstring& txtFile )
{
    Helium::Path path( datFile );
    // setup the input file
    if ( !path.Exists() )
    {
        throw Exception( TXT( "Input events file [%s] doesn't exist!" ), datFile.c_str() );
    }

    // set up the output file
    Helium::Path outputPath( txtFile );
    if ( outputPath.empty() )
    {
        outputPath.Set( datFile );
        outputPath.ReplaceExtension( TXT( "txt" ) );
    }

    outputPath.Create();

    // get this user's events
    ES::V_EventPtr listOfEvents;
    ReadBinaryEventsFile( datFile, listOfEvents, true );
    WriteTextEventsFile( outputPath.Get(), listOfEvents );
}


/////////////////////////////////////////////////////////////////////////////
// Loads the event.dat file from the event.txt file
//
void EventSystem::LoadEventsFromTextFile( const tstring& txtFile, const tstring& datFile )
{
    Helium::Path path( txtFile );
    // setup the input file
    if ( !path.Exists() )
    {
        throw Exception( TXT( "Input text events file [%s] doesn't exist!" ), txtFile.c_str() );
    }

    // setup the output file
    Helium::Path cleanEventsFile( datFile );
    if ( cleanEventsFile.Get().empty() )
    {
        cleanEventsFile.Set( txtFile );
        cleanEventsFile.ReplaceExtension( TXT( "event.dat" ) );
    }
    cleanEventsFile.Create();

    if ( !cleanEventsFile.Writable() )
    {
        throw Exception( TXT( "Output events file [%s] is read-only!" ), cleanEventsFile.c_str() );
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
void EventSystem::StompEventsFile( const tstring& eventsFile, const V_EventPtr& listOfEvents )
{
    Helium::Path path( eventsFile );
    // make sure it's not read only
    if ( !path.Writable() )
    {
        throw Exception( TXT( "Could not write to events, file is read-only: %s" ), eventsFile.c_str() );
    }

    // Open the record file and truncate to clear its contents
    {
        std::ofstream recordsFile( eventsFile.c_str(), std::ios::out | std::ios::binary | std::ios::trunc );
        if ( !recordsFile.is_open() )
        {
            throw Exception( TXT( "Could not open events file for write: %s" ), eventsFile.c_str() );
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

            throw Exception( TXT( "Could not write to events file: %s" ), eventsFile.c_str() );
        }

        recordsFile.close();
    }

    // now populate the event.dat file with the new list of events
    WriteEventsFile( eventsFile, listOfEvents );
}
