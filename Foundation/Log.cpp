#include "FoundationPch.h"
#include "Log.h"

#include "Platform/Assert.h"
#include "Platform/Locks.h"
#include "Platform/Thread.h"
#include "Platform/File.h"
#include "Platform/Console.h"

#include "Foundation/String.h"

#include <stdio.h>
#include <io.h>
#include <time.h>
#include <sys/timeb.h>

#include <fstream>
#include <iostream>
#include <map>

#include <crtdbg.h>
#include <shlobj.h>

using namespace Helium;
using namespace Helium::Log;

#define NTFS_PATH_MAX (0x7FFF)

uint32_t g_LogFileCount = 20;

Helium::Mutex g_Mutex;

typedef std::map<tstring, File*> M_Files;

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
			itr->second->Close();
			delete itr->second;
		}

		m_Files.clear();
	}

	bool Opened(const tstring& fileName, File* f)
	{
		return m_Files.insert( M_Files::value_type (fileName, f) ).second;
	}

	File* Find(const tstring& fileName)
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
			found->second->Close();
			delete found->second;
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
	uint32_t    m_ThreadId;

	OutputFile()
		: m_StreamType( Streams::Normal )
		, m_RefCount( 0 )
		, m_ThreadId( -1 )
	{

	}
};

typedef std::map< tstring, OutputFile > M_OutputFile;
M_OutputFile g_TraceFiles;

uint32_t g_Streams = Streams::Normal | Streams::Warning | Streams::Error;
Level g_Level = Levels::Default;
int g_WarningCount = 0;
int g_ErrorCount = 0;
int g_Indent = 0;

PrintingSignature::Event g_PrintingEvent;
PrintedSignature::Event g_PrintedEvent;

void Log::Statement::ApplyIndent( const tchar_t* string, tstring& output )
{
	if ( m_Indent > 0 )
	{
		tchar_t m_IndentString[64] = TXT( "" );
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
		const tchar_t* pos = string;
		tchar_t previous = '\n';
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

			// copy the tchar_t to the statement
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
	Helium::MutexScopeLock mutex (g_Mutex);

	g_PrintingEvent.Add(listener);
}

void Log::RemovePrintingListener(const PrintingSignature::Delegate& listener)
{
	Helium::MutexScopeLock mutex (g_Mutex);

	g_PrintingEvent.Remove(listener);
}

void Log::AddPrintedListener(const PrintedSignature::Delegate& listener)
{
	Helium::MutexScopeLock mutex (g_Mutex);

	g_PrintedEvent.Add(listener);
}

void Log::RemovePrintedListener(const PrintedSignature::Delegate& listener)
{
	Helium::MutexScopeLock mutex (g_Mutex);

	g_PrintedEvent.Remove(listener);
}

void Redirect(const tstring& fileName, const tchar_t* str, bool stampNewLine = true )
{
	File* f = g_FileManager.Find(fileName);
	if (f)
	{
		size_t length = 0;
		size_t count = ( StringLength( str ) + 128 );
		tchar_t* temp = (tchar_t*)alloca( sizeof(tchar_t) * count );
		if ( stampNewLine )
		{
			_timeb currentTime;
			_ftime( &currentTime );

			uint32_t time = (uint32_t) currentTime.time;
			uint32_t milli = currentTime.millitm;
			uint32_t sec = time % 60; time /= 60;
			uint32_t min = time % 60; time -= currentTime.timezone; time /= 60;
			time += currentTime.dstflag ? 1 : 0;
			uint32_t hour = time % 24;

			length = StringPrint( temp, count, TXT("[%02d:%02d:%02d.%03d TID:%d] %s"), hour, min, sec, milli, GetCurrentThreadId(), str );
		}
		else
		{
			length = StringPrint( temp, count, TXT("%s"), str );
		}

		f->Write( temp, length );
		f->Flush();
	}
}

bool AddFile( M_OutputFile& files, const tstring& fileName, Stream stream, uint32_t threadId, bool append )
{
	Helium::MutexScopeLock mutex (g_Mutex);

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
		if (fileName != TXT( "" ))
		{
			File* f = new File;
			if ( f->Open( fileName.c_str(), FileModes::MODE_WRITE ) )
			{
				if ( append )
				{
					f->Seek( 0, SeekOrigins::SEEK_ORIGIN_END );
				}

				g_FileManager.Opened( fileName, f );
				OutputFile info;
				info.m_StreamType = stream;
				info.m_RefCount = 1;
				info.m_ThreadId = threadId;
				files[ fileName ] = info;
				return true;
			}
			else
			{
				delete f;
				f = NULL;
			}
		}
	}

	return false;
}

void RemoveFile( M_OutputFile& files, const tstring& fileName )
{
	Helium::MutexScopeLock mutex (g_Mutex);

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

bool Log::AddTraceFile( const tstring& fileName, Stream stream, uint32_t threadId, bool append )
{
	return AddFile( g_TraceFiles, fileName, stream, threadId, append );
}

void Log::RemoveTraceFile( const tstring& fileName )
{
	RemoveFile( g_TraceFiles, fileName );
}

void Log::Indent(int col)
{
	if ( IsMainThread() )
	{
		g_Indent += (col < 0 ? 2 : col);
	}
}

void Log::UnIndent(int col)
{
	if ( IsMainThread() )
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

ConsoleColor Log::GetStreamColor( Log::Stream stream )
{
	switch (stream)
	{
	case Streams::Normal:
		return ConsoleColors::None;

	case Streams::Debug:
		return ConsoleColors::Aqua;

	case Streams::Profile:
		return ConsoleColors::Green;

	case Streams::Warning:
		return ConsoleColors::Yellow;

	case Streams::Error:
		return ConsoleColors::Red;

	default:
		HELIUM_BREAK();
		break;
	}

	return ConsoleColors::None;
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

void Log::PrintString(const tchar_t* string, Stream stream, Level level, ConsoleColor color, int indent, tchar_t* output, uint32_t outputSize)
{
	Helium::MutexScopeLock mutex (g_Mutex);

	// check trace files
	bool trace = false;
	M_OutputFile::iterator itr = g_TraceFiles.begin();
	M_OutputFile::iterator end = g_TraceFiles.end();
	for( ; itr != end; ++itr )
	{
		if ( ( (*itr).second.m_StreamType & stream ) == stream
			&& ( (*itr).second.m_ThreadId == -1 || (*itr).second.m_ThreadId == GetCurrentThreadID() ) )
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
				if ( color == ConsoleColors::None )
				{
					color = GetStreamColor( stream );
				}

				// print the statement to the window
				Helium::PrintString((Helium::ConsoleColor)color, stream == Streams::Error ? stderr : stdout, statement.m_String);

				// raise the printed event
				g_PrintedEvent.Raise( PrintedArgs (statement) );
			}

			// send the text to the debugger, if no debugger nothing happens
#if HELIUM_OS_WIN
#if HELIUM_WCHAR_T
			OutputDebugStringW(statement.m_String.c_str());
#else
			OutputDebugStringA(statement.m_String.c_str());
#endif
#endif
			// output to trace file(s)
			static bool stampNewLine = true;

			itr = g_TraceFiles.begin();
			end = g_TraceFiles.end();
			for( ; itr != end; ++itr )
			{
				if ( ( (*itr).second.m_StreamType & stream ) == stream
					&& ( (*itr).second.m_ThreadId == -1 || (*itr).second.m_ThreadId == GetCurrentThreadID() ) )
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
				CopyString( output, outputSize - 1, statement.m_String.c_str() );
			}
		}
	}
}

void Log::PrintStatement(const Statement& statement)
{
	Helium::MutexScopeLock mutex (g_Mutex);

	PrintString( statement.m_String.c_str(), statement.m_Stream, statement.m_Level, GetStreamColor( statement.m_Stream ), statement.m_Indent );
}

void Log::PrintStatements(const V_Statement& statements, uint32_t streamFilter)
{
	Helium::MutexScopeLock mutex (g_Mutex);

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

void Log::PrintColor(ConsoleColor color, const tchar_t* fmt, ...)
{
	Helium::MutexScopeLock mutex (g_Mutex);

	va_list args;
	va_start(args, fmt); 
	static tchar_t string[MAX_PRINT_SIZE];
	int size = StringPrintArgs(string, fmt, args);
	string[ sizeof(string)/sizeof(string[0]) - 1] = 0; 
	HELIUM_ASSERT(size >= 0);
	PrintString(string, Streams::Normal, Levels::Default, color); 
	va_end(args); 
}

void Log::Print(const tchar_t *fmt,...) 
{
	Helium::MutexScopeLock mutex (g_Mutex);

	va_list args;
	va_start(args, fmt); 
	static tchar_t string[MAX_PRINT_SIZE];
	int size = StringPrintArgs(string, fmt, args);
	string[ sizeof(string)/sizeof(string[0]) - 1] = 0; 
	HELIUM_ASSERT(size >= 0);

	PrintString(string, Streams::Normal, Levels::Default, Log::GetStreamColor( Streams::Normal ));
	va_end(args);      
}

void Log::Print(Level level, const tchar_t *fmt,...) 
{
	Helium::MutexScopeLock mutex (g_Mutex);

	va_list args;
	va_start(args, fmt); 
	static tchar_t string[MAX_PRINT_SIZE];
	int size = StringPrintArgs(string, fmt, args);
	string[ sizeof(string)/sizeof(string[0]) - 1] = 0; 
	HELIUM_ASSERT(size >= 0);

	PrintString(string, Streams::Normal, level, Log::GetStreamColor( Streams::Normal ));
	va_end(args);       
}

void Log::Debug(const tchar_t *fmt,...) 
{
	Helium::MutexScopeLock mutex (g_Mutex);

	va_list args;
	va_start(args, fmt); 
	static tchar_t string[MAX_PRINT_SIZE];
	int size = StringPrintArgs(string, fmt, args);
	string[ sizeof(string)/sizeof(string[0]) - 1] = 0; 
	HELIUM_ASSERT(size >= 0);

	PrintString(string, Streams::Debug, Levels::Default, Log::GetStreamColor( Streams::Debug ), 0);
	va_end(args);
}

void Log::Debug(Level level, const tchar_t *fmt,...) 
{
	Helium::MutexScopeLock mutex (g_Mutex);

	va_list args;
	va_start(args, fmt); 
	static tchar_t string[MAX_PRINT_SIZE];
	int size = StringPrintArgs(string, fmt, args);
	string[ sizeof(string)/sizeof(string[0]) - 1] = 0; 
	HELIUM_ASSERT(size >= 0);

	PrintString(string, Streams::Debug, level, Log::GetStreamColor( Streams::Debug ), 0);
	va_end(args);
}

void Log::Profile(const tchar_t *fmt,...) 
{
	Helium::MutexScopeLock mutex (g_Mutex);

	va_list args;
	va_start(args, fmt); 
	static tchar_t string[MAX_PRINT_SIZE];
	int size = StringPrintArgs(string, fmt, args);
	string[ sizeof(string)/sizeof(string[0]) - 1] = 0; 
	HELIUM_ASSERT(size >= 0);

	PrintString(string, Streams::Profile, Levels::Default, Log::GetStreamColor( Streams::Profile ), 0);
	va_end(args);
}

void Log::Profile(Level level, const tchar_t *fmt,...) 
{
	Helium::MutexScopeLock mutex (g_Mutex);

	va_list args;
	va_start(args, fmt); 
	static tchar_t string[MAX_PRINT_SIZE];
	int size = StringPrintArgs(string, fmt, args);
	string[ sizeof(string)/sizeof(string[0]) - 1] = 0; 
	HELIUM_ASSERT(size >= 0);

	PrintString(string, Streams::Profile, level, Log::GetStreamColor( Streams::Profile ), 0);
	va_end(args);
}

void Log::Warning(const tchar_t *fmt,...) 
{
	Helium::MutexScopeLock mutex (g_Mutex);

	static tchar_t format[MAX_PRINT_SIZE];
	StringPrint(format, TXT( "Warning (%d): " ), ++g_WarningCount);
	AppendString(format, fmt);

	va_list args;
	va_start(args, fmt); 
	static tchar_t string[MAX_PRINT_SIZE];
	int size = StringPrint(string, format, args);
	string[ sizeof(string)/sizeof(string[0]) - 1] = 0; 
	HELIUM_ASSERT(size >= 0);

	PrintString(string, Streams::Warning, Levels::Default, Log::GetStreamColor( Streams::Warning ), 0);
	va_end(args);      
}

void Log::Warning(Level level, const tchar_t *fmt,...) 
{
	Helium::MutexScopeLock mutex (g_Mutex);

	static tchar_t format[MAX_PRINT_SIZE];
	if (level == Levels::Default)
	{
		StringPrint(format, TXT( "Warning (%d): " ), ++g_WarningCount);
	}
	else
	{
		format[0] = '\0';
	}
	AppendString(format, fmt);

	va_list args;
	va_start(args, fmt); 
	static tchar_t string[MAX_PRINT_SIZE];
	int size = StringPrint(string, format, args);
	string[ sizeof(string)/sizeof(string[0]) - 1] = 0; 
	HELIUM_ASSERT(size >= 0);

	PrintString(string, Streams::Warning, level, Log::GetStreamColor( Streams::Warning ), 0);
	va_end(args);      
}

void Log::Error(const tchar_t *fmt,...) 
{
	Helium::MutexScopeLock mutex (g_Mutex);

	static tchar_t format[MAX_PRINT_SIZE];
	StringPrint(format, TXT( "Error (%d): " ), ++g_ErrorCount);
	AppendString(format, fmt);

	va_list args;
	va_start(args, fmt); 
	static tchar_t string[MAX_PRINT_SIZE];
	int size = StringPrint(string, format, args);
	string[ sizeof(string)/sizeof(string[0]) - 1] = 0; 
	HELIUM_ASSERT(size >= 0);

	PrintString(string, Streams::Error, Levels::Default, Log::GetStreamColor( Streams::Error ), 0);
	va_end(args);
}

void Log::Error(Level level, const tchar_t *fmt,...) 
{
	Helium::MutexScopeLock mutex (g_Mutex);

	static tchar_t format[MAX_PRINT_SIZE];
	if (level == Levels::Default)
	{
		StringPrint(format, TXT( "Error (%d): " ), ++g_ErrorCount);
	}
	else
	{
		format[0] = '\0';
	}
	AppendString(format, fmt);

	va_list args;
	va_start(args, fmt); 
	static tchar_t string[MAX_PRINT_SIZE];
	int size = StringPrint(string, format, args);
	string[ sizeof(string)/sizeof(string[0]) - 1] = 0; 
	HELIUM_ASSERT(size >= 0);

	PrintString(string, Streams::Error, level, Log::GetStreamColor( Streams::Error ), 0);
	va_end(args);
}

Log::Heading::Heading(const tchar_t *fmt, ...)
{
	Helium::MutexScopeLock mutex (g_Mutex);

	// do a basic print
	va_list args;
	va_start(args, fmt); 
	static tchar_t string[MAX_PRINT_SIZE];
	int size = StringPrintArgs(string, fmt, args);
	string[ sizeof(string)/sizeof(string[0]) - 1] = 0; 
	HELIUM_ASSERT(size >= 0);

	PrintString(string, Streams::Normal, Levels::Default, Log::GetStreamColor( Streams::Normal ));
	va_end(args);      

	// now indent
	Indent();
}

Log::Heading::~Heading()
{
	Helium::MutexScopeLock mutex (g_Mutex);

	// unindent
	UnIndent();
}

std::vector<tstring> g_OutlineState;

Log::Bullet::Bullet(const tchar_t *fmt, ...)
: m_Stream( Streams::Normal )
, m_Level( Log::Levels::Default )
, m_Valid( fmt != NULL )
{
	if (m_Valid)
	{
		Helium::MutexScopeLock mutex (g_Mutex);

		va_list args;
		va_start(args, fmt); 
		CreateBullet( fmt, args );
		va_end(args);
	}
}

Log::Bullet::Bullet(Stream stream, Log::Level level, const tchar_t *fmt, ...)
: m_Stream( stream )
, m_Level( level )
, m_Valid( fmt != NULL )
{
	if (m_Valid)
	{
		Helium::MutexScopeLock mutex (g_Mutex);

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
		Helium::MutexScopeLock mutex (g_Mutex);

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

void Log::Bullet::CreateBullet(const tchar_t *fmt, va_list args)
{
	static tchar_t delims[] = { 'o', '*', '>', '-' };

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
	static tchar_t format[MAX_PRINT_SIZE];
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
	AppendString(format, fmt);

	// format the bullet string
	static tchar_t string[MAX_PRINT_SIZE];
	int size = StringPrint(string, format, args);
	string[ sizeof(string)/sizeof(string[0]) - 1] = 0; 
	HELIUM_ASSERT(size >= 0);

	// do the print and capture the output
	static tchar_t output[MAX_PRINT_SIZE];
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
	Helium::MutexScopeLock mutex (g_Mutex);

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

Listener::Listener( uint32_t throttle, uint32_t* errorCount, uint32_t* warningCount, Log::V_Statement* consoleOutput )
: m_Thread( GetCurrentThreadID() )
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

uint32_t Listener::GetWarningCount()
{
	return *m_WarningCount;
}

uint32_t Listener::GetErrorCount()
{
	return *m_ErrorCount;
}

void Listener::Print( Log::PrintingArgs& args )
{
	if ( m_Thread == GetCurrentThreadID() )
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