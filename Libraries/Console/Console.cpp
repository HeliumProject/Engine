#include "Console.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <crtdbg.h>
#include <io.h>
#include <map>
#include <shlobj.h>
#include <sys/timeb.h>
#include <time.h>

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINDOWS 0x0500
#include <windows.h>

#include "Common/Version.h"
#include "Common/Assert.h"

#include "Windows/Console.h"
#include "Windows/Thread.h"

using namespace Console;

#define NTFS_PATH_MAX (0x7FFF)

u32 g_LogFileCount = 20;

Windows::CriticalSection g_Section;

DWORD g_MainThread = GetCurrentThreadId();

typedef std::map<std::string, FILE*> M_Files;

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

  bool Opened(const std::string& fileName, FILE* handle)
  {
    return m_Files.insert( M_Files::value_type (fileName, handle) ).second;
  }

  FILE* Find(const std::string& fileName)
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

  void Close(const std::string& fileName)
  {
    M_Files::iterator found = m_Files.find( fileName );
    if (found != m_Files.end())
    {
      fclose( found->second );
      m_Files.erase( found );
    }
    else
    {
      NOC_BREAK();
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

typedef std::map< std::string, OutputFile > M_OutputFile;
M_OutputFile g_TraceFiles;

u32 g_Streams = Streams::Normal | Streams::Warning | Streams::Error;
Level g_Level = Levels::Default;
int g_WarningCount = 0;
int g_ErrorCount = 0;
int g_Indent = 0;

PrintingSignature::Event g_PrintingEvent;
PrintedSignature::Event g_PrintedEvent;

void Console::Statement::ApplyIndent( const char* string, std::string& output )
{
  if ( m_Indent > 0 )
  {
    char m_IndentString[64] = "";
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
    const char* pos = string;
    char previous = '\n';
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

      // copy the char to the statement
      output += *pos;
    }
  }
  else
  {
    output = string;
  }
}

void Console::AddPrintingListener(const PrintingSignature::Delegate& listener)
{
  Windows::TakeSection section (g_Section);

  g_PrintingEvent.Add(listener);
}

void Console::RemovePrintingListener(const PrintingSignature::Delegate& listener)
{
  Windows::TakeSection section (g_Section);

  g_PrintingEvent.Remove(listener);
}

void Console::AddPrintedListener(const PrintedSignature::Delegate& listener)
{
  Windows::TakeSection section (g_Section);

  g_PrintedEvent.Add(listener);
}

void Console::RemovePrintedListener(const PrintedSignature::Delegate& listener)
{
  Windows::TakeSection section (g_Section);

  g_PrintedEvent.Remove(listener);
}

void Redirect(const std::string& fileName, const char* str, bool stampNewLine = true )
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

void RotateLogs(const std::string& file)
{
  // chop up the full log file path
  char drive[MAX_PATH], dir[MAX_PATH], name[MAX_PATH], ext[MAX_PATH];
  _splitpath( file.c_str(), drive, dir, name, ext );

  // build a path to the dir the log lives in
  std::string logDirectory;
  logDirectory = drive;
  logDirectory += dir;

  // ensure we are a valid dir name
  if ( logDirectory.find( "/" ) == std::string::npos )
  {
    logDirectory = "./";
  }

  // Make sure that the directory exists
  SHCreateDirectoryEx( NULL, logDirectory.c_str(), NULL );

  // get the current list of log files
  V_string fileList;

  // do the folder iteration
  char spec[NTFS_PATH_MAX];
  _snprintf(spec, sizeof(spec), "%s%s%s%s", drive, dir, name, "*.log"); // DT: *.* was including files that were 'not' log files!
  spec[ sizeof(spec) - 1 ] = 0; 

  struct _finddata_t finddata;
  intptr_t handle = _findfirst(spec, &finddata);
  while (handle != -1)
  {
    if(!(finddata.attrib& _A_SUBDIR))
    {
      char itrName[NTFS_PATH_MAX];
      _splitpath( finddata.name, NULL, NULL, itrName, NULL );

      std::string file = itrName;
      std::string number = file.substr(strlen(name));

      // we only want to munge files that are our log or a numbered backup
      if (number.empty() || isdigit(number[0]))
      {
        fileList.push_back( logDirectory + finddata.name );
      }
    }

    if (_findnext(handle, &finddata) != 0)
    {
      _findclose(handle);
      handle = -1;
    }
  }

  // walk the list in reverse and delete excess while rotating each file
  size_t count = fileList.size();
  V_string::reverse_iterator itr = fileList.rbegin();
  V_string::reverse_iterator end = fileList.rend();
  for ( ; itr != end; ++itr, --count )
  {
    const std::string& current = *itr;

    // if we have too many files, delete
    if ( g_LogFileCount > 0 && count >= g_LogFileCount )
    {
      ::DeleteFile( current.c_str() );
    }
    else
    {
      char rotated[NTFS_PATH_MAX];
      _snprintf( rotated, sizeof( rotated ), "%s%s%02d%s", logDirectory.c_str(), name, count, ext );
      rotated[ sizeof(rotated) - 1 ] = 0; 

      ::MoveFileEx( current.c_str(), rotated, MOVEFILE_REPLACE_EXISTING );
    }
  }
}

bool AddFile( M_OutputFile& files, const std::string& fileName, Stream stream, u32 threadId, bool append )
{
  Windows::TakeSection section (g_Section);

  M_OutputFile::iterator found = files.find( fileName );
  if ( found != files.end() )
  {
    if ( found->second.m_StreamType != stream )
    {
      NOC_BREAK(); // trying to add the same file, but with a different stream type
    }

    if ( found->second.m_ThreadId != threadId )
    {
      NOC_BREAK(); // trying to add the same file with a different thread id
    }

    found->second.m_RefCount++; // another reference
  }
  else
  {
    FILE* f = NULL;

    if (fileName != "")
    {
      char *mode = append ? "at+" : "wt+";

      RotateLogs( fileName );

      f = fopen( fileName.c_str(), mode );
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

void RemoveFile( M_OutputFile& files, const std::string& fileName )
{
  Windows::TakeSection section (g_Section);

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

bool Console::AddTraceFile( const std::string& fileName, Stream stream, u32 threadId, bool append )
{
  return AddFile( g_TraceFiles, fileName, stream, threadId, append );
}

void Console::RemoveTraceFile( const std::string& fileName )
{
  RemoveFile( g_TraceFiles, fileName );
}

void Console::Indent(int col)
{
  if (g_MainThread == GetCurrentThreadId())
  {
    g_Indent += (col < 0 ? 2 : col);
  }
}

void Console::UnIndent(int col)
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

Level Console::GetLevel()
{
  return g_Level;
}

void Console::SetLevel(Level level)
{
  g_Level = level;
}

bool Console::IsStreamEnabled( Stream stream )
{
  return ( g_Streams & stream ) == stream;
}

void Console::EnableStream( Stream stream, bool enable )
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

Console::Color Console::GetStreamColor( Console::Stream stream )
{
  switch (stream)
  {
  case Streams::Normal:
    return Console::Colors::None;

  case Streams::Debug:
    return Console::Colors::Aqua;

  case Streams::Profile:
    return Console::Colors::Green;

  case Streams::Warning:
    return Console::Colors::Yellow;

  case Streams::Error:
    return Console::Colors::Red;

  default:
    NOC_BREAK();
    break;
  }

 return Console::Colors::None;
}

int Console::GetErrorCount()
{
  return g_ErrorCount;
}

int Console::GetWarningCount()
{
  return g_WarningCount;
}

void Console::ResetErrorCount()
{
  g_ErrorCount = 0;
}

void Console::ResetWarningCount()
{
  g_WarningCount = 0;
}

void Console::EnterCriticalSection()
{
  g_Section.Enter();
}

void Console::LeaveCriticalSection()
{
  g_Section.Leave();
}

void Console::PrintString(const char* string, Stream stream, Level level, Color color, int indent, char* output, u32 outputSize)
{
  Windows::TakeSection section (g_Section);

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
        Windows::PrintString((Windows::ConsoleColor)color, stream == Streams::Error ? stderr : stdout, statement.m_String);

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
        strncpy( output, statement.m_String.c_str(), outputSize - 1 );
      }
    }
  }
}

void Console::PrintStatement(const Statement& statement)
{
  Windows::TakeSection section (g_Section);

  PrintString( statement.m_String.c_str(), statement.m_Stream, statement.m_Level, GetStreamColor( statement.m_Stream ), statement.m_Indent );
}

void Console::PrintStatements(const V_Statement& statements, u32 streamFilter)
{
  Windows::TakeSection section (g_Section);

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

void Console::PrintColor(Console::Color color, const char* fmt, ...)
{
  Windows::TakeSection section (g_Section);

  va_list args;
  va_start(args, fmt); 
  static char string[MAX_PRINT_SIZE];
  int size = _vsnprintf(string, sizeof(string), fmt, args);
  string[ sizeof(string) - 1] = 0; 
  NOC_ASSERT(size >= 0);
  PrintString(string, Streams::Normal, Levels::Default, color); 
  va_end(args); 
}

void Console::Print(const char *fmt,...) 
{
  Windows::TakeSection section (g_Section);

  va_list args;
  va_start(args, fmt); 
  static char string[MAX_PRINT_SIZE];
  int size = _vsnprintf(string, sizeof(string), fmt, args);
  string[ sizeof(string) - 1] = 0; 
  NOC_ASSERT(size >= 0);

  PrintString(string, Streams::Normal, Levels::Default, Console::GetStreamColor( Streams::Normal ));
  va_end(args);      
}

void Console::Print(Level level, const char *fmt,...) 
{
  Windows::TakeSection section (g_Section);

  va_list args;
  va_start(args, fmt); 
  static char string[MAX_PRINT_SIZE];
  int size = _vsnprintf(string, sizeof(string), fmt, args);
  string[ sizeof(string) - 1] = 0; 
  NOC_ASSERT(size >= 0);

  PrintString(string, Streams::Normal, level, Console::GetStreamColor( Streams::Normal ));
  va_end(args);       
}

void Console::Debug(const char *fmt,...) 
{
  Windows::TakeSection section (g_Section);

  va_list args;
  va_start(args, fmt); 
  static char string[MAX_PRINT_SIZE];
  int size = _vsnprintf(string, sizeof(string), fmt, args);
  string[ sizeof(string) - 1] = 0; 
  NOC_ASSERT(size >= 0);

  PrintString(string, Streams::Debug, Levels::Default, Console::GetStreamColor( Streams::Debug ), 0);
  va_end(args);
}

void Console::Debug(Level level, const char *fmt,...) 
{
  Windows::TakeSection section (g_Section);

  va_list args;
  va_start(args, fmt); 
  static char string[MAX_PRINT_SIZE];
  int size = _vsnprintf(string, sizeof(string), fmt, args);
  string[ sizeof(string) - 1] = 0; 
  NOC_ASSERT(size >= 0);

  PrintString(string, Streams::Debug, level, Console::GetStreamColor( Streams::Debug ), 0);
  va_end(args);
}

void Console::Profile(const char *fmt,...) 
{
  Windows::TakeSection section (g_Section);

  va_list args;
  va_start(args, fmt); 
  static char string[MAX_PRINT_SIZE];
  int size = _vsnprintf(string, sizeof(string), fmt, args);
  string[ sizeof(string) - 1] = 0; 
  NOC_ASSERT(size >= 0);

  PrintString(string, Streams::Profile, Levels::Default, Console::GetStreamColor( Streams::Profile ), 0);
  va_end(args);
}

void Console::Profile(Level level, const char *fmt,...) 
{
  Windows::TakeSection section (g_Section);

  va_list args;
  va_start(args, fmt); 
  static char string[MAX_PRINT_SIZE];
  int size = _vsnprintf(string, sizeof(string), fmt, args);
  string[ sizeof(string) - 1] = 0; 
  NOC_ASSERT(size >= 0);

  PrintString(string, Streams::Profile, level, Console::GetStreamColor( Streams::Profile ), 0);
  va_end(args);
}

void Console::Warning(const char *fmt,...) 
{
  Windows::TakeSection section (g_Section);

  static char format[MAX_PRINT_SIZE];
  sprintf(format, "Warning (%d): ", ++g_WarningCount);
  strcat(format, fmt);

  va_list args;
  va_start(args, fmt); 
  static char string[MAX_PRINT_SIZE];
  int size = _vsnprintf(string, sizeof(string), format, args);
  string[ sizeof(string) - 1] = 0; 
  NOC_ASSERT(size >= 0);

  PrintString(string, Streams::Warning, Levels::Default, Console::GetStreamColor( Streams::Warning ), 0);
  va_end(args);      
}

void Console::Warning(Level level, const char *fmt,...) 
{
  Windows::TakeSection section (g_Section);

  static char format[MAX_PRINT_SIZE];
  if (level == Levels::Default)
  {
    sprintf(format, "Warning (%d): ", ++g_WarningCount);
  }
  else
  {
    format[0] = '\0';
  }
  strcat(format, fmt);

  va_list args;
  va_start(args, fmt); 
  static char string[MAX_PRINT_SIZE];
  int size = _vsnprintf(string, sizeof(string), format, args);
  string[ sizeof(string) - 1] = 0; 
  NOC_ASSERT(size >= 0);

  PrintString(string, Streams::Warning, level, Console::GetStreamColor( Streams::Warning ), 0);
  va_end(args);      
}

void Console::Error(const char *fmt,...) 
{
  Windows::TakeSection section (g_Section);

  static char format[MAX_PRINT_SIZE];
  sprintf(format, "Error (%d): ", ++g_ErrorCount);
  strcat(format, fmt);

  va_list args;
  va_start(args, fmt); 
  static char string[MAX_PRINT_SIZE];
  int size = _vsnprintf(string, sizeof(string), format, args);
  string[ sizeof(string) - 1] = 0; 
  NOC_ASSERT(size >= 0);

  PrintString(string, Streams::Error, Levels::Default, Console::GetStreamColor( Streams::Error ), 0);
  va_end(args);
}

void Console::Error(Level level, const char *fmt,...) 
{
  Windows::TakeSection section (g_Section);

  static char format[MAX_PRINT_SIZE];
  if (level == Levels::Default)
  {
    sprintf(format, "Error (%d): ", ++g_ErrorCount);
  }
  else
  {
    format[0] = '\0';
  }
  strcat(format, fmt);

  va_list args;
  va_start(args, fmt); 
  static char string[MAX_PRINT_SIZE];
  int size = _vsnprintf(string, sizeof(string), format, args);
  string[ sizeof(string) - 1] = 0; 
  NOC_ASSERT(size >= 0);

  PrintString(string, Streams::Error, level, Console::GetStreamColor( Streams::Error ), 0);
  va_end(args);
}

Console::Heading::Heading(const char *fmt, ...)
{
  Windows::TakeSection section (g_Section);

  // do a basic print
  va_list args;
  va_start(args, fmt); 
  static char string[MAX_PRINT_SIZE];
  int size = _vsnprintf(string, sizeof(string), fmt, args);
  string[ sizeof(string) - 1] = 0; 
  NOC_ASSERT(size >= 0);

  PrintString(string, Streams::Normal, Levels::Default, Console::GetStreamColor( Streams::Normal ));
  va_end(args);      

  // now indent
  Indent();
}

Console::Heading::~Heading()
{
  Windows::TakeSection section (g_Section);

  // unindent
  UnIndent();
}

std::vector<std::string> g_OutlineState;

Console::Bullet::Bullet(const char *fmt, ...)
: m_Stream( Streams::Normal )
, m_Level( Console::Levels::Default )
, m_Valid( fmt != NULL )
{
  if (m_Valid)
  {
    Windows::TakeSection section (g_Section);

    va_list args;
    va_start(args, fmt); 
    CreateBullet( fmt, args );
    va_end(args);
  }
}

Console::Bullet::Bullet(Stream stream, Console::Level level, const char *fmt, ...)
: m_Stream( stream )
, m_Level( level )
, m_Valid( fmt != NULL )
{
  if (m_Valid)
  {
    Windows::TakeSection section (g_Section);

    va_list args;
    va_start(args, fmt); 
    CreateBullet( fmt, args );
    va_end(args);
  }
}

Console::Bullet::~Bullet()
{
  if (m_Valid)
  {
    Windows::TakeSection section (g_Section);

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

void Console::Bullet::CreateBullet(const char *fmt, va_list args)
{
  static char delims[] = { 'o', '*', '>', '-' };

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
  static char format[MAX_PRINT_SIZE];
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
  strcat(format, fmt);

  // format the bullet string
  static char string[MAX_PRINT_SIZE];
  int size = _vsnprintf(string, sizeof(string), format, args);
  string[ sizeof(string) - 1] = 0; 
  NOC_ASSERT(size >= 0);

  // do the print and capture the output
  static char output[MAX_PRINT_SIZE];
  if (g_Indent == 1)
  {
    PrintString( "\n", m_Stream, m_Level, Console::GetStreamColor( m_Stream ), -1, output, sizeof( output ) );
  }
  PrintString( string, m_Stream, m_Level, Console::GetStreamColor( m_Stream ), -1, output, sizeof( output ) );

  // push state
  g_OutlineState.push_back( output );

  if ( print )
  {
    // now indent
    Indent();
  }
}

std::string Console::GetOutlineState()
{
  Windows::TakeSection section (g_Section);

  std::string state;

  std::vector<std::string>::const_iterator itr = g_OutlineState.begin();
  std::vector<std::string>::const_iterator end = g_OutlineState.end();
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
