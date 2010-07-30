#include "Log.h"

#include "Platform/Assert.h"
#include "Platform/Mutex.h"
#include "Platform/Windows/Windows.h"
#include "Platform/Windows/Console.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <crtdbg.h>
#include <io.h>
#include <map>
#include <shlobj.h>
#include <sys/timeb.h>
#include <time.h>

using namespace Log;

#define NTFS_PATH_MAX (0x7FFF)

u32 g_LogFileCount = 20;

Helium::Mutex g_Mutex;

DWORD g_MainThread = GetCurrentThreadId();

typedef std::map<tstring, FILE*> M_Files;

class FileManager
{
private:
    M_Files m_Files;

public:
    FileManager()
    {

    }

    ~FileManager()
    {
        M_Files::const_iterator itr = m_Files.begin();
        M_Files::const_iterator end = m_Files.end();
        for ( ; itr != end; ++itr )
        {
            fclose( itr->second );
        }

        m_Files.clear();
    }

    bool Opened(const tstring& fileName, FILE* handle)
    {
        return m_Files.insert( M_Files::value_type (fileName, handle) ).second;
    }

    FILE* Find(const tstring& fileName)
    {
        M_Files::const_iterator found = m_Files.find(fileName.c_str());
        if (found != m_Files.end())
        {
            return found->second;
        }
        else
        {
            return NULL;
        }
    }

    void Close(const tstring& fileName)
    {
        M_Files::iterator found = m_Files.find( fileName );
        if (found != m_Files.end())
        {
            fclose( found->second );
            m_Files.erase( found );
        }
        else
        {
            HELIUM_BREAK();
        }
    }
};

FileManager g_FileManager;

struct OutputFile
{
    Stream      m_StreamType;
    int         m_RefCount;
    u32         m_ThreadId;

    OutputFile()
        : m_StreamType( Streams::Normal )
        , m_RefCount( 0 )
        , m_ThreadId( -1 )
    {

    }
};

typedef std::map< tstring, OutputFile > M_OutputFile;
M_OutputFile g_TraceFiles;

u32 g_Streams = Streams::Normal | Streams::Warning | Streams::Error;
Level g_Level = Levels::Default;
int g_WarningCount = 0;
int g_ErrorCount = 0;
int g_Indent = 0;

PrintingSignature::Event g_PrintingEvent;
PrintedSignature::Event g_PrintedEvent;

void Log::Statement::ApplyIndent( const tchar* string, tstring& output )
{
    if ( m_Indent > 0 )
    {
        tchar m_IndentString[64] = TXT( "" );
        if(m_Indent >= sizeof(m_IndentString))
        {
            m_Indent = sizeof(m_IndentString)-1;
        }

        for (int i=0; i<m_Indent; i++)
        {
            m_IndentString[i] = ' ';
        }
        m_IndentString[m_Indent] = '\0';

        // insert the indtent string after newlines and before non-newlines
        const tchar* pos = string;
        tchar previous = '\n';
        for ( ; *pos != '\0'; previous = *pos++ )
        {
            if ( *pos == '\r' )
            {
                continue;
            }

            if ( previous == '\n' && *pos != '\n' )
            {
                // start of a new line, apply m_Indent
                output += m_IndentString;
            }

            // copy the tchar to the statement
            output += *pos;
        }
    }
    else
    {
        output = string;
    }
}

void Log::AddPrintingListener(const PrintingSignature::Delegate& listener)
{
    Helium::TakeMutex mutex (g_Mutex);

    g_PrintingEvent.Add(listener);
}

void Log::RemovePrintingListener(const PrintingSignature::Delegate& listener)
{
    Helium::TakeMutex mutex (g_Mutex);

    g_PrintingEvent.Remove(listener);
}

void Log::AddPrintedListener(const PrintedSignature::Delegate& listener)
{
    Helium::TakeMutex mutex (g_Mutex);

    g_PrintedEvent.Add(listener);
}

void Log::RemovePrintedListener(const PrintedSignature::Delegate& listener)
{
    Helium::TakeMutex mutex (g_Mutex);

    g_PrintedEvent.Remove(listener);
}

void Redirect(const tstring& fileName, const tchar* str, bool stampNewLine = true )
{
    FILE* f = g_FileManager.Find(fileName);
    if (f)
    {
        if ( stampNewLine )
        {
            _timeb currentTime;
            _ftime( &currentTime );

            u32 time = (u32) currentTime.time;
            u32 milli = currentTime.millitm;
            u32 sec = time % 60; time /= 60;
            u32 min = time % 60; time -= currentTime.timezone; time /= 60;
            time += currentTime.dstflag ? 1 : 0;
            u32 hour = time % 24;

            fprintf( f, "[%02d:%02d:%02d.%03d TID:%d] %s", hour, min, sec, milli, GetCurrentThreadId(), str );
        }
        else
        {
            fprintf( f, "%s", str );
        }

        fflush( f );
    }
}

bool AddFile( M_OutputFile& files, const tstring& fileName, Stream stream, u32 threadId, bool append )
{
    Helium::TakeMutex mutex (g_Mutex);

    M_OutputFile::iterator found = files.find( fileName );
    if ( found != files.end() )
    {
        if ( found->second.m_StreamType != stream )
        {
            HELIUM_BREAK(); // trying to add the same file, but with a different stream type
        }

        if ( found->second.m_ThreadId != threadId )
        {
            HELIUM_BREAK(); // trying to add the same file with a different thread id
        }

        found->second.m_RefCount++; // another reference
    }
    else
    {
        FILE* f = NULL;

        if (fileName != TXT( "" ))
        {
            tchar *mode = append ? TXT( "at+" ) : TXT( "wt+" );

            f = _tfopen( fileName.c_str(), mode );
        }

        if ( f )
        {
            g_FileManager.Opened( fileName, f );
            OutputFile info;
            info.m_StreamType = stream;
            info.m_RefCount = 1;
            info.m_ThreadId = threadId;
            files[ fileName ] = info;
            return true;
        }
    }

    return false;
}

void RemoveFile( M_OutputFile& files, const tstring& fileName )
{
    Helium::TakeMutex mutex (g_Mutex);

    M_OutputFile::iterator found = files.find( fileName );
    if ( found != files.end() )
    {
        found->second.m_RefCount--;

        if ( found->second.m_RefCount == 0 )
        {
            g_FileManager.Close( fileName );
            files.erase( found );
        }
    }
}

bool Log::AddTraceFile( const tstring& fileName, Stream stream, u32 threadId, bool append )
{
    return AddFile( g_TraceFiles, fileName, stream, threadId, append );
}

void Log::RemoveTraceFile( const tstring& fileName )
{
    RemoveFile( g_TraceFiles, fileName );
}

void Log::Indent(int col)
{
    if (g_MainThread == GetCurrentThreadId())
    {
        g_Indent += (col < 0 ? 2 : col);
    }
}

void Log::UnIndent(int col)
{
    if (g_MainThread == GetCurrentThreadId())
    {
        g_Indent -= (col < 0 ? 2 : col);
        if (g_Indent < 0)
        {
            g_Indent = 0;
        }
    }
}

Level Log::GetLevel()
{
    return g_Level;
}

void Log::SetLevel(Level level)
{
    g_Level = level;
}

bool Log::IsStreamEnabled( Stream stream )
{
    return ( g_Streams & stream ) == stream;
}

void Log::EnableStream( Stream stream, bool enable )
{
    if ( enable )
    {
        g_Streams |= stream;
    }
    else
    {
        g_Streams &= ~stream;
    }
}

Log::Color Log::GetStreamColor( Log::Stream stream )
{
    switch (stream)
    {
    case Streams::Normal:
        return Log::Colors::None;

    case Streams::Debug:
        return Log::Colors::Aqua;

    case Streams::Profile:
        return Log::Colors::Green;

    case Streams::Warning:
        return Log::Colors::Yellow;

    case Streams::Error:
        return Log::Colors::Red;

    default:
        HELIUM_BREAK();
        break;
    }

    return Log::Colors::None;
}

int Log::GetErrorCount()
{
    return g_ErrorCount;
}

int Log::GetWarningCount()
{
    return g_WarningCount;
}

void Log::ResetErrorCount()
{
    g_ErrorCount = 0;
}

void Log::ResetWarningCount()
{
    g_WarningCount = 0;
}

void Log::LockMutex()
{
    g_Mutex.Lock();
}

void Log::UnlockMutex()
{
    g_Mutex.Unlock();
}

void Log::PrintString(const tchar* string, Stream stream, Level level, Color color, int indent, tchar* output, u32 outputSize)
{
    Helium::TakeMutex mutex (g_Mutex);

    // check trace files
    bool trace = false;
    M_OutputFile::iterator itr = g_TraceFiles.begin();
    M_OutputFile::iterator end = g_TraceFiles.end();
    for( ; itr != end; ++itr )
    {
        if ( ( (*itr).second.m_StreamType & stream ) == stream
            && ( (*itr).second.m_ThreadId == -1 || (*itr).second.m_ThreadId == GetCurrentThreadId() ) )
        {
            trace = true;
        }
    }

    // determine if we should be displayed
    bool display = ( g_Streams & stream ) == stream && level <= g_Level;

    // check for nothing to do
    if ( trace || display || output )
    {
        if ( indent < 0 )
        {
            indent = g_Indent;
        }

        // the statement
        Statement statement ( string, stream, level, indent );

        // construct the print statement
        PrintingArgs args ( statement );

        // is this statement to be output via normal channels
        if ( display )
        {
            // raise the printing event
            g_PrintingEvent.Raise( args );
        }

        // only process this string if it was not handled by a handler
        if ( !args.m_Skip )
        {
            // apply indentation
            statement.m_String.clear();
            statement.ApplyIndent( string, statement.m_String );

            // output to screen window
            if ( display )
            {
                // deduce the color if we were told to do so
                if ( color == Colors::Auto )
                {
                    color = GetStreamColor( stream );
                }

                // print the statement to the window
                Helium::PrintString((Helium::ConsoleColor)color, stream == Streams::Error ? stderr : stdout, statement.m_String);

                // raise the printed event
                g_PrintedEvent.Raise( PrintedArgs (statement) );
            }

            // send the text to the debugger, if no debugger nothing happens
            OutputDebugString(statement.m_String.c_str());

            // output to trace file(s)
            static bool stampNewLine = true;

            itr = g_TraceFiles.begin();
            end = g_TraceFiles.end();
            for( ; itr != end; ++itr )
            {
                if ( ( (*itr).second.m_StreamType & stream ) == stream
                    && ( (*itr).second.m_ThreadId == -1 || (*itr).second.m_ThreadId == GetCurrentThreadId() ) )
                {
                    Redirect( (*itr).first, statement.m_String.c_str(), stampNewLine );
                }
            }

            // update stampNewLine
            if ( !statement.m_String.empty() )
            {
                stampNewLine = ( *statement.m_String.rbegin() == '\n' ) ? true : false ;
            }

            // output to buffer
            if (output && outputSize > 0)
            {
                _tcsncpy( output, statement.m_String.c_str(), outputSize - 1 );
            }
        }
    }
}

void Log::PrintStatement(const Statement& statement)
{
    Helium::TakeMutex mutex (g_Mutex);

    PrintString( statement.m_String.c_str(), statement.m_Stream, statement.m_Level, GetStreamColor( statement.m_Stream ), statement.m_Indent );
}

void Log::PrintStatements(const V_Statement& statements, u32 streamFilter)
{
    Helium::TakeMutex mutex (g_Mutex);

    V_Statement::const_iterator itr = statements.begin();
    V_Statement::const_iterator end = statements.end();
    for ( ; itr != end; ++itr )
    {
        if ( itr->m_Stream & streamFilter )
        {
            PrintStatement( *itr );
        }
    }
}

void Log::PrintColor(Log::Color color, const tchar* fmt, ...)
{
    Helium::TakeMutex mutex (g_Mutex);

    va_list args;
    va_start(args, fmt); 
    static tchar string[MAX_PRINT_SIZE];
    int size = _vsntprintf(string, sizeof(string), fmt, args);
    string[ sizeof(string) - 1] = 0; 
    HELIUM_ASSERT(size >= 0);
    PrintString(string, Streams::Normal, Levels::Default, color); 
    va_end(args); 
}

void Log::Print(const tchar *fmt,...) 
{
    Helium::TakeMutex mutex (g_Mutex);

    va_list args;
    va_start(args, fmt); 
    static tchar string[MAX_PRINT_SIZE];
    int size = _vsntprintf(string, sizeof(string), fmt, args);
    string[ sizeof(string) - 1] = 0; 
    HELIUM_ASSERT(size >= 0);

    PrintString(string, Streams::Normal, Levels::Default, Log::GetStreamColor( Streams::Normal ));
    va_end(args);      
}

void Log::Print(Level level, const tchar *fmt,...) 
{
    Helium::TakeMutex mutex (g_Mutex);

    va_list args;
    va_start(args, fmt); 
    static tchar string[MAX_PRINT_SIZE];
    int size = _vsntprintf(string, sizeof(string), fmt, args);
    string[ sizeof(string) - 1] = 0; 
    HELIUM_ASSERT(size >= 0);

    PrintString(string, Streams::Normal, level, Log::GetStreamColor( Streams::Normal ));
    va_end(args);       
}

void Log::Debug(const tchar *fmt,...) 
{
    Helium::TakeMutex mutex (g_Mutex);

    va_list args;
    va_start(args, fmt); 
    static tchar string[MAX_PRINT_SIZE];
    int size = _vsntprintf(string, sizeof(string), fmt, args);
    string[ sizeof(string) - 1] = 0; 
    HELIUM_ASSERT(size >= 0);

    PrintString(string, Streams::Debug, Levels::Default, Log::GetStreamColor( Streams::Debug ), 0);
    va_end(args);
}

void Log::Debug(Level level, const tchar *fmt,...) 
{
    Helium::TakeMutex mutex (g_Mutex);

    va_list args;
    va_start(args, fmt); 
    static tchar string[MAX_PRINT_SIZE];
    int size = _vsntprintf(string, sizeof(string), fmt, args);
    string[ sizeof(string) - 1] = 0; 
    HELIUM_ASSERT(size >= 0);

    PrintString(string, Streams::Debug, level, Log::GetStreamColor( Streams::Debug ), 0);
    va_end(args);
}

void Log::Profile(const tchar *fmt,...) 
{
    Helium::TakeMutex mutex (g_Mutex);

    va_list args;
    va_start(args, fmt); 
    static tchar string[MAX_PRINT_SIZE];
    int size = _vsntprintf(string, sizeof(string), fmt, args);
    string[ sizeof(string) - 1] = 0; 
    HELIUM_ASSERT(size >= 0);

    PrintString(string, Streams::Profile, Levels::Default, Log::GetStreamColor( Streams::Profile ), 0);
    va_end(args);
}

void Log::Profile(Level level, const tchar *fmt,...) 
{
    Helium::TakeMutex mutex (g_Mutex);

    va_list args;
    va_start(args, fmt); 
    static tchar string[MAX_PRINT_SIZE];
    int size = _vsntprintf(string, sizeof(string), fmt, args);
    string[ sizeof(string) - 1] = 0; 
    HELIUM_ASSERT(size >= 0);

    PrintString(string, Streams::Profile, level, Log::GetStreamColor( Streams::Profile ), 0);
    va_end(args);
}

void Log::Warning(const tchar *fmt,...) 
{
    Helium::TakeMutex mutex (g_Mutex);

    static tchar format[MAX_PRINT_SIZE];
    _stprintf(format, TXT( "Warning (%d): " ), ++g_WarningCount);
    _tcscat(format, fmt);

    va_list args;
    va_start(args, fmt); 
    static tchar string[MAX_PRINT_SIZE];
    int size = _vsntprintf(string, sizeof(string), format, args);
    string[ sizeof(string) - 1] = 0; 
    HELIUM_ASSERT(size >= 0);

    PrintString(string, Streams::Warning, Levels::Default, Log::GetStreamColor( Streams::Warning ), 0);
    va_end(args);      
}

void Log::Warning(Level level, const tchar *fmt,...) 
{
    Helium::TakeMutex mutex (g_Mutex);

    static tchar format[MAX_PRINT_SIZE];
    if (level == Levels::Default)
    {
        _stprintf(format, TXT( "Warning (%d): " ), ++g_WarningCount);
    }
    else
    {
        format[0] = '\0';
    }
    _tcscat(format, fmt);

    va_list args;
    va_start(args, fmt); 
    static tchar string[MAX_PRINT_SIZE];
    int size = _vsntprintf(string, sizeof(string), format, args);
    string[ sizeof(string) - 1] = 0; 
    HELIUM_ASSERT(size >= 0);

    PrintString(string, Streams::Warning, level, Log::GetStreamColor( Streams::Warning ), 0);
    va_end(args);      
}

void Log::Error(const tchar *fmt,...) 
{
    Helium::TakeMutex mutex (g_Mutex);

    static tchar format[MAX_PRINT_SIZE];
    _stprintf(format, TXT( "Error (%d): " ), ++g_ErrorCount);
    _tcscat(format, fmt);

    va_list args;
    va_start(args, fmt); 
    static tchar string[MAX_PRINT_SIZE];
    int size = _vsntprintf(string, sizeof(string), format, args);
    string[ sizeof(string) - 1] = 0; 
    HELIUM_ASSERT(size >= 0);

    PrintString(string, Streams::Error, Levels::Default, Log::GetStreamColor( Streams::Error ), 0);
    va_end(args);
}

void Log::Error(Level level, const tchar *fmt,...) 
{
    Helium::TakeMutex mutex (g_Mutex);

    static tchar format[MAX_PRINT_SIZE];
    if (level == Levels::Default)
    {
        _stprintf(format, TXT( "Error (%d): " ), ++g_ErrorCount);
    }
    else
    {
        format[0] = '\0';
    }
    _tcscat(format, fmt);

    va_list args;
    va_start(args, fmt); 
    static tchar string[MAX_PRINT_SIZE];
    int size = _vsntprintf(string, sizeof(string), format, args);
    string[ sizeof(string) - 1] = 0; 
    HELIUM_ASSERT(size >= 0);

    PrintString(string, Streams::Error, level, Log::GetStreamColor( Streams::Error ), 0);
    va_end(args);
}

Log::Heading::Heading(const tchar *fmt, ...)
{
    Helium::TakeMutex mutex (g_Mutex);

    // do a basic print
    va_list args;
    va_start(args, fmt); 
    static tchar string[MAX_PRINT_SIZE];
    int size = _vsntprintf(string, sizeof(string), fmt, args);
    string[ sizeof(string) - 1] = 0; 
    HELIUM_ASSERT(size >= 0);

    PrintString(string, Streams::Normal, Levels::Default, Log::GetStreamColor( Streams::Normal ));
    va_end(args);      

    // now indent
    Indent();
}

Log::Heading::~Heading()
{
    Helium::TakeMutex mutex (g_Mutex);

    // unindent
    UnIndent();
}

std::vector<tstring> g_OutlineState;

Log::Bullet::Bullet(const tchar *fmt, ...)
: m_Stream( Streams::Normal )
, m_Level( Log::Levels::Default )
, m_Valid( fmt != NULL )
{
    if (m_Valid)
    {
        Helium::TakeMutex mutex (g_Mutex);

        va_list args;
        va_start(args, fmt); 
        CreateBullet( fmt, args );
        va_end(args);
    }
}

Log::Bullet::Bullet(Stream stream, Log::Level level, const tchar *fmt, ...)
: m_Stream( stream )
, m_Level( level )
, m_Valid( fmt != NULL )
{
    if (m_Valid)
    {
        Helium::TakeMutex mutex (g_Mutex);

        va_list args;
        va_start(args, fmt); 
        CreateBullet( fmt, args );
        va_end(args);
    }
}

Log::Bullet::~Bullet()
{
    if (m_Valid)
    {
        Helium::TakeMutex mutex (g_Mutex);

        // this gates the output to the console for streams and levels that the user did not elect to see on in the console
        bool print = ( ( g_Streams & m_Stream ) == m_Stream ) && ( m_Level <= g_Level );

        if ( print )
        {
            // unindent
            UnIndent();

            if (g_Indent == 1)
            {
                UnIndent(1);
            }
            else
            {
                Indent(1);
            }
        }

        g_OutlineState.pop_back();
    }
}

void Log::Bullet::CreateBullet(const tchar *fmt, va_list args)
{
    static tchar delims[] = { 'o', '*', '>', '-' };

    // this gates the output to the console for streams and levels that the user did not elect to see on in the console
    bool print = ( ( g_Streams & m_Stream ) == m_Stream ) && ( m_Level <= g_Level );

    if ( print )
    {
        if (g_Indent == 0)
        {
            Indent(1);
        }
        else
        {
            UnIndent(1);
        }
    }

    // build the format string
    static tchar format[MAX_PRINT_SIZE];
    if (g_Indent == 1)
    {
        format[0] = delims[ 0 ];
        format[1] = ' ';
        format[2] = '\0';
    }
    else
    {
        format[0] = delims[ (g_Indent-1 + sizeof(delims)) % sizeof(delims) ];
        format[1] = ' ';
        format[2] = '\0';
    }
    _tcscat(format, fmt);

    // format the bullet string
    static tchar string[MAX_PRINT_SIZE];
    int size = _vsntprintf(string, sizeof(string), format, args);
    string[ sizeof(string) - 1] = 0; 
    HELIUM_ASSERT(size >= 0);

    // do the print and capture the output
    static tchar output[MAX_PRINT_SIZE];
    if (g_Indent == 1)
    {
        PrintString( TXT( "\n" ), m_Stream, m_Level, Log::GetStreamColor( m_Stream ), -1, output, sizeof( output ) );
    }
    PrintString( string, m_Stream, m_Level, Log::GetStreamColor( m_Stream ), -1, output, sizeof( output ) );

    // push state
    g_OutlineState.push_back( output );

    if ( print )
    {
        // now indent
        Indent();
    }
}

tstring Log::GetOutlineState()
{
    Helium::TakeMutex mutex (g_Mutex);

    tstring state;

    std::vector<tstring>::const_iterator itr = g_OutlineState.begin();
    std::vector<tstring>::const_iterator end = g_OutlineState.end();
    for ( ; itr != end; ++itr )
    {
        if ( itr == g_OutlineState.begin() )
        {
            state = *itr;
        }
        else
        {
            state += *itr;
        }
    }

    return state;
}

Listener::Listener( u32 throttle, u32* errorCount, u32* warningCount, Log::V_Statement* consoleOutput )
: m_Thread( GetCurrentThreadId() )
, m_Throttle( throttle )
, m_WarningCount( warningCount )
, m_ErrorCount( errorCount )
, m_LogOutput( consoleOutput )
{
    Start();
}

Listener::~Listener()
{
    Stop();
}

void Listener::Start()
{
    Log::AddPrintingListener( Log::PrintingSignature::Delegate( this, &Listener::Print ) );
}

void Listener::Stop()
{
    Log::RemovePrintingListener( Log::PrintingSignature::Delegate( this, &Listener::Print ) );
}

void Listener::Dump(bool stop)
{
    if ( stop )
    {
        Stop();
    }

    if ( m_LogOutput )
    {
        Log::PrintStatements( *m_LogOutput );
    }
}

u32 Listener::GetWarningCount()
{
    return *m_WarningCount;
}

u32 Listener::GetErrorCount()
{
    return *m_ErrorCount;
}

void Listener::Print( Log::PrintingArgs& args )
{
    if ( m_Thread == GetCurrentThreadId() )
    {
        if ( args.m_Statement.m_Stream == Log::Streams::Warning && m_WarningCount )
        {
            (*m_WarningCount)++;
        }

        if ( args.m_Statement.m_Stream == Log::Streams::Error && m_ErrorCount )
        {
            (*m_ErrorCount)++;
        }

        if ( m_LogOutput )
        {
            m_LogOutput->push_back( args.m_Statement );
        }

        if ( m_Throttle & args.m_Statement.m_Stream )
        {
            args.m_Skip = true;
        }
    }
}