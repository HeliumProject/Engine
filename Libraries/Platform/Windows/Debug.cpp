#include "Debug.h"

#include "Platform/Types.h"
#include "Platform/Mutex.h"
#include "Platform/Windows/Windows.h"
#include "Foundation/Log.h"
#include "Foundation/Exception.h"

#include <time.h>
#include <shlobj.h>
#include <dbghelp.h>
#include <tlhelp32.h>
#pragma comment ( lib, "dbghelp.lib" )

//#define DEBUG_SYMBOLS

// disable the optimizer if debugging in release
#if defined(DEBUG_SYMBOLS) && defined(NDEBUG)
#pragma optimize("", off)
#endif

// loaded flag
static bool g_Initialized = false;

// Utility to print to a string
static void PrintString(std::string& buffer, const char* str, ...)
{
  static char buf[4096];

  va_list argptr;
  va_start(argptr, str);
  vsnprintf(buf, sizeof(buf), str, argptr);
  buf[ sizeof(buf) - 1] = 0; 
  va_end(argptr);

  buffer += buf;
}

// Callback that loads symbol data from loaded dll into DbgHelp system, dumping error info
static BOOL CALLBACK EnumerateLoadedModulesProc(PCSTR name, DWORD64 base, ULONG size, PVOID data)
{
  if (SymLoadModule64(GetCurrentProcess(), 0, name, 0, base, size))
  {
    IMAGEHLP_MODULE64 moduleInfo;
    ZeroMemory(&moduleInfo, sizeof(IMAGEHLP_MODULE64));
    moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
    if ( SymGetModuleInfo64( GetCurrentProcess(), base, &moduleInfo ) )
    {
      if ( moduleInfo.LoadedPdbName[0] != '\0' )
      {
        Log::Debug( "Success loading symbols for module: %s, base: 0x%08I64X, size: %u: %s\n", name, base, size, moduleInfo.LoadedPdbName );
      }
      else
      {
        Log::Debug( "Success loading symbols for module: %s, base: 0x%08I64X, size: %u\n", name, base, size );
      }
    }
    else
    {
      Log::Debug( "Failure loading symbols for module: %s: %s\n", name, Platform::GetErrorString().c_str() );
    }
  }

  return TRUE;
}

// Load debugging information for the currently loaded dlls, call this each time you need to use Sym*() API.  Keep calling it
static void EnumerateLoadedModules()
{
  // load debugging information
  //  this is probably slow, but SYMOPT_DEFERRED_LOADS doesn't always work with 
  //  dynamically loaded dlls so we do this instead of SYMOPT_DEFERRED_LOADS and 
  //  invading the process during SymInitialize
  EnumerateLoadedModules64(GetCurrentProcess(), &EnumerateLoadedModulesProc, NULL);
}

bool Debug::Initialize(const std::string& pdbPaths)
{
  if ( !g_Initialized )
  {
    std::string dir;

    if ( pdbPaths.empty() )
    {
      char module[MAX_PATH];
      char drive[MAX_PATH];
      char path[MAX_PATH];
      char file[MAX_PATH];
      char ext[MAX_PATH];
      GetModuleFileName(0,module,MAX_PATH);
      _splitpath(module,drive,path,file,ext);

      dir = drive;
      dir += path;
    }
    else
    {
      dir = pdbPaths;
    }

    DWORD options = SYMOPT_FAIL_CRITICAL_ERRORS |
                    SYMOPT_LOAD_LINES |
                    SYMOPT_UNDNAME;

    SymSetOptions(options);

    Log::Debug( "Symbol Path: %s\n", dir.c_str() );

    // initialize symbols (dbghelp.dll)
    if ( SymInitialize(GetCurrentProcess(), dir.c_str(), FALSE) == 0 )
    {
      Log::Debug( "Failure initializing symbol API: %s\n", Platform::GetErrorString().c_str() );
      return false;
    }

    // success
    g_Initialized = true;
  }

  return true;
}

bool Debug::IsInitialized()
{
  return g_Initialized;
}

std::string Debug::GetSymbolInfo(uintptr adr, bool enumLoadedModules)
{
  NOC_ASSERT( Debug::IsInitialized() );

  if ( enumLoadedModules )
  {
    // load debugging information
    EnumerateLoadedModules();
  }

  // module image name "reflect.dll"
  static char module[MAX_PATH];
  ZeroMemory(&module, sizeof(module));
  static char extension[MAX_PATH];
  ZeroMemory(&extension, sizeof(extension));

  // symbol name "Reflect::Class::AddSerializer + 0x16d"
  static char symbol[MAX_SYM_NAME+16];
  ZeroMemory(&symbol, sizeof(symbol));

  // source file name "typeinfo.cpp"
  static char filename[MAX_PATH];
  ZeroMemory(&filename, sizeof(filename));

  // line number in source "246"
  DWORD line = 0xFFFFFFFF;

  // resulting line is worst case of all components
  static char result[sizeof(module) + sizeof(symbol) + sizeof(filename) + 64];
  ZeroMemory(&result, sizeof(result));


  //
  // Start by finding the module the address is in
  //

  IMAGEHLP_MODULE64 moduleInfo;
  ZeroMemory(&moduleInfo, sizeof(IMAGEHLP_MODULE64));
  moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
  if (SymGetModuleInfo64(GetCurrentProcess(), adr, &moduleInfo))
  {
    // success, copy the module info
    _splitpath(moduleInfo.ImageName, NULL, NULL, module, extension);
    strcat(module, extension);

    //
    // Now find symbol information
    //

    // displacement of the symbol
    DWORD64 disp;

    // okay, the name runs off the end of the structure, so setup a buffer and cast it
    ULONG64 buffer[(sizeof(SYMBOL_INFO) + MAX_SYM_NAME*sizeof(TCHAR) + sizeof(ULONG64) - 1) / sizeof(ULONG64)];
    PSYMBOL_INFO symbolInfo = (PSYMBOL_INFO)buffer;
    symbolInfo->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbolInfo->MaxNameLen = MAX_SYM_NAME;

    if ( SymFromAddr(GetCurrentProcess(), adr, &disp, symbolInfo) != 0 )
    {
      // success, copy the symbol info
      sprintf(symbol, "%s + 0x%X", symbolInfo->Name, disp);

      //
      // Now find source line information
      //

      DWORD d;
      IMAGEHLP_LINE64 l;
      ZeroMemory(&l,sizeof(l));
      l.SizeOfStruct = sizeof(l);
      if ( SymGetLineFromAddr64(GetCurrentProcess(), adr, &d, &l) !=0 )
      {
        // success, copy the source file name
        strcpy(filename, l.FileName);
        static char ext[MAX_PATH];
        static char file[MAX_PATH];
        _splitpath(filename, NULL, NULL, file, ext);

        sprintf(result, "%s, %s : %s%s(%d)", module, symbol, file, ext, l.LineNumber);
        return result;
      }

      sprintf(result, "%s, %s", module, symbol);
      return result;
    }

    sprintf(result, "%s", module);
    return result;
  }
  else
  {
    DWORD err = GetLastError();
    return "Unknown";
  }
}

std::exception* Debug::GetCxxException(uintptr addr)
{
  std::exception* cppException = (std::exception*)addr;

  __try
  {
    // if its non-null
    if (cppException)
    {
      // this will explode if the address isn't really a c++ exception (std::exception)
      cppException->what();
    }

    // i guess we lived!
    return cppException;
  }
  __except(EXCEPTION_EXECUTE_HANDLER)
  {
    // uh oh, somebody is throwing register types or another root struct or class
    return NULL;
  }
}

bool Debug::GetStackTrace(std::vector<uintptr>& trace, unsigned omitFrames)
{
  //  Some techniques borrowed from Visual Leak Detector 1.9
  //   (http://www.codeproject.com/tools/visualleakdetector.asp)

  CONTEXT context;

  volatile char *p = 0;
  __try
  {
    *p = 0;
  }
  __except(CopyMemory(&context, (GetExceptionInformation())->ContextRecord, sizeof(context)), EXCEPTION_EXECUTE_HANDLER)
  {

  }

  return GetStackTrace(&context, trace, omitFrames+1);
}

bool Debug::GetStackTrace(LPCONTEXT context, std::vector<uintptr>& stack, unsigned omitFrames)
{
  NOC_ASSERT( Debug::IsInitialized() );

  // load debugging information
  EnumerateLoadedModules();

  if (omitFrames == 0)
  {
    // our our current location as the top of the stack
    stack.push_back( context->IPREG );
  }
  else
  {
    omitFrames--;
  }

  // this is for handling stack overflows
  std::map<i64, u32> visited;

  // setup the stack frame to use for traversal
  STACKFRAME64 frame;
  memset(&frame,0,sizeof(frame));
  frame.AddrPC.Offset = context->IPREG;
  frame.AddrPC.Segment = 0;
  frame.AddrPC.Mode = AddrModeFlat;
  frame.AddrStack.Offset = context->SPREG;
  frame.AddrStack.Segment = 0;
  frame.AddrStack.Mode = AddrModeFlat;
  frame.AddrFrame.Offset = context->BPREG;
  frame.AddrFrame.Segment = 0;
  frame.AddrFrame.Mode = AddrModeFlat;

  // make a copy here because StackWalk64 can modify the one you give it
  CONTEXT context_copy;
  memcpy(&context_copy, context, sizeof(context_copy));

  while(1)
  {
    if (!StackWalk64(IMAGE_FILE_ARCH,
                     GetCurrentProcess(),
                     GetCurrentThread(),
                     &frame,
                     &context_copy,
                     NULL,
                     SymFunctionTableAccess64,
                     SymGetModuleBase64,
                     NULL))
    {
      break;
    }

    if (frame.AddrReturn.Offset == 0x0 || frame.AddrReturn.Offset == 0xffffffffcccccccc)
    {
      break;
    }

    if (visited[ frame.AddrReturn.Offset ]++ >= 8192)
    {
      break;
    }

#ifdef DEBUG_SYMBOLS
    printf( "0x%08I64X - %s\n", frame.AddrReturn.Offset, GetSymbolInfo(frame.AddrReturn.Offset, false).c_str() );
#endif

    if (omitFrames == 0)
    {
      stack.push_back( (uintptr)frame.AddrReturn.Offset );
    }
    else
    {
      omitFrames--;
    }
  }

  return !stack.empty();
}

void Debug::TranslateStackTrace(const std::vector<uintptr>& trace, std::string& buffer)
{
  std::vector<uintptr>::const_iterator itr = trace.begin();
  std::vector<uintptr>::const_iterator end = trace.end();
  for ( ; itr != end; ++itr )
  {
    PrintString(buffer, "0x%08I64X - %s\n", *itr, GetSymbolInfo(*itr, false).c_str() );
  }
}

const char* Debug::GetExceptionClass(u32 exceptionCode)
{
  const char* ex_name = NULL;

  switch (exceptionCode)
  {
  case EXCEPTION_ACCESS_VIOLATION:
    ex_name = "EXCEPTION_ACCESS_VIOLATION";
    break;

  case EXCEPTION_BREAKPOINT:
    ex_name = "EXCEPTION_BREAKPOINT";
    break;

  case EXCEPTION_SINGLE_STEP:
    ex_name = "EXCEPTION_SINGLE_STEP";
    break;

  case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
    ex_name = "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
    break;

  case EXCEPTION_FLT_DENORMAL_OPERAND:
    ex_name = "EXCEPTION_FLT_DENORMAL_OPERAND";
    break;

  case EXCEPTION_FLT_DIVIDE_BY_ZERO:
    ex_name = "EXCEPTION_FLT_DIVIDE_BY_ZERO";
    break;

  case EXCEPTION_FLT_INEXACT_RESULT:
    ex_name = "EXCEPTION_FLT_INEXACT_RESULT";
    break;

  case EXCEPTION_FLT_INVALID_OPERATION:
    ex_name = "EXCEPTION_FLT_INVALID_OPERATION";
    break;

  case EXCEPTION_FLT_OVERFLOW:
    ex_name = "EXCEPTION_FLT_OVERFLOW";
    break;

  case EXCEPTION_FLT_STACK_CHECK:
    ex_name = "EXCEPTION_FLT_STACK_CHECK";
    break;

  case EXCEPTION_FLT_UNDERFLOW:
    ex_name = "EXCEPTION_FLT_UNDERFLOW";
    break;

  case EXCEPTION_INT_DIVIDE_BY_ZERO:
    ex_name = "EXCEPTION_INT_DIVIDE_BY_ZERO";
    break;

  case EXCEPTION_INT_OVERFLOW:
    ex_name = "EXCEPTION_INT_OVERFLOW";
    break;

  case EXCEPTION_PRIV_INSTRUCTION:
    ex_name = "EXCEPTION_PRIV_INSTRUCTION";
    break;

  case EXCEPTION_IN_PAGE_ERROR:
    ex_name = "EXCEPTION_IN_PAGE_ERROR";
    break;

  case EXCEPTION_ILLEGAL_INSTRUCTION:
    ex_name = "EXCEPTION_ILLEGAL_INSTRUCTION";
    break;

  case EXCEPTION_NONCONTINUABLE_EXCEPTION:
    ex_name = "EXCEPTION_NONCONTINUABLE_EXCEPTION";
    break;

  case EXCEPTION_STACK_OVERFLOW:
    ex_name = "EXCEPTION_STACK_OVERFLOW";
    break;

  case EXCEPTION_INVALID_DISPOSITION:
    ex_name = "EXCEPTION_INVALID_DISPOSITION";
    break;

  case EXCEPTION_GUARD_PAGE:
    ex_name = "EXCEPTION_GUARD_PAGE";
    break;

  case EXCEPTION_INVALID_HANDLE:
    ex_name = "EXCEPTION_INVALID_HANDLE";
    break;

  case 0xC00002B5:
    ex_name = "Multiple floating point traps";
    break;
  }

  if (ex_name == NULL)
  {
    ex_name = "Unknown Exception";
  }

  return ex_name;
}

void Debug::GetExceptionDetails( LPEXCEPTION_POINTERS info, ExceptionArgs& args )
{
  static Platform::Mutex s_ExceptionMutex;
  Platform::TakeMutex mutex ( s_ExceptionMutex );

  // we need to take console's section here because we are going to suspend threads,
  //  and this library could try and take this section via a function call
  struct TakeConsoleMutex
  {
    TakeConsoleMutex()
    {
      Log::LockMutex();
    }
    ~TakeConsoleMutex()
    {
      Log::UnlockMutex();
    }
  } consoleMutex;

  typedef std::vector< std::pair<DWORD, HANDLE> > V_ThreadHandles;
  V_ThreadHandles threads;

  // the first thing we do is check out other threads, since they are still running!
  HANDLE snapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
  if ( snapshot != INVALID_HANDLE_VALUE )
  {
    THREADENTRY32 thread;
    memset(&thread, 0, sizeof( thread ));
    thread.dwSize = sizeof( thread );
    if ( Thread32First(snapshot, &thread) )
    {
      do
      {
        if ( thread.th32OwnerProcessID == GetCurrentProcessId() && thread.th32ThreadID != GetCurrentThreadId() )
        {
          HANDLE handle = ::OpenThread( THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION, FALSE, thread.th32ThreadID );
          if ( handle )
          {
            ::SuspendThread( handle );
            threads.push_back( std::make_pair( thread.th32ThreadID, handle ) );
          }
        }
      }
      while ( Thread32Next( snapshot, &thread ) );
    }

    ::CloseHandle( snapshot );
  }

  V_ThreadHandles::const_iterator itr = threads.begin();
  V_ThreadHandles::const_iterator end = threads.end();
  for ( ; itr != end; ++itr )
  {
    DWORD id = itr->first;
    HANDLE handle = itr->second;

    args.m_Threads.push_back( "" );
    PrintString( args.m_Threads.back(), "Thread %d:\n", id );
    std::string::size_type size = args.m_Threads.back().size();

    CONTEXT context;
    memset(&context, 0, sizeof( context ));
    context.ContextFlags = CONTEXT_FULL;
    if ( ::GetThreadContext( handle, &context ) )
    {
      PrintString( args.m_Threads.back(), 
        "\nControl Registers:\n"
        "EIP = 0x%08X  ESP = 0x%08X\n"
        "EBP = 0x%08X  EFL = 0x%08X\n",
        info->ContextRecord->IPREG,
        info->ContextRecord->SPREG,
        info->ContextRecord->BPREG,
        info->ContextRecord->EFlags );

      PrintString( args.m_Threads.back(), 
        "\nInteger Registers:\n"
        "EAX = 0x%08X  EBX = 0x%08X\n"
        "ECX = 0x%08X  EDX = 0x%08X\n"
        "ESI = 0x%08X  EDI = 0x%08X\n",
        info->ContextRecord->AXREG,
        info->ContextRecord->BXREG,
        info->ContextRecord->CXREG,
        info->ContextRecord->DXREG,
        info->ContextRecord->SIREG,
        info->ContextRecord->DIREG );

      PrintString( args.m_Threads.back(), "\nCallstack:\n" );

      std::vector<uintptr> trace;
      if ( GetStackTrace( &context, trace ) )
      {
        TranslateStackTrace( trace, args.m_Threads.back() );
      }
    }

    if ( args.m_Threads.back().size() == size )
    {
      args.m_Threads.back() += "No thread info\n";
    }

    ::ResumeThread( handle );
    ::CloseHandle( handle );
  }

  threads.clear();

  args.m_SEHCode = info->ExceptionRecord->ExceptionCode;
  args.m_SEHClass = GetExceptionClass( info->ExceptionRecord->ExceptionCode );
  
  if (info->ExceptionRecord->ExceptionCode == 0xE06D7363) // Microsoft C++ Exception code
  {
    args.m_Type = ExceptionTypes::CPP;

    std::exception* cppException = GetCxxException(info->ExceptionRecord->ExceptionInformation[1]);
    if (cppException)
    {
      const char* cppClass = NULL;
      try
      {
        cppClass = typeid(*cppException).name();
      }
      catch (const std::__non_rtti_object&)
      {
        cppClass = "Unknown";
      }

      args.m_CPPClass = cppClass;
      args.m_Message = cppException->what();
    }
    else
    {
      args.m_Message = "Thrown object is not a std::exception";
    }

    info->ContextRecord->IPREG = (DWORD)info->ExceptionRecord->ExceptionInformation[2];
  }
  else
  {
    args.m_Type = ExceptionTypes::SEH;

    if (args.m_SEHCode == EXCEPTION_ACCESS_VIOLATION)
    {
      PrintString( args.m_Message, "Attempt to %s address 0x%08X", (info->ExceptionRecord->ExceptionInformation[0]==1)?"write to":"read from", info->ExceptionRecord->ExceptionInformation[1]);
    }

    if (info->ContextRecord->ContextFlags & CONTEXT_CONTROL)
    {
      PrintString( args.m_SEHControlRegisters, 
        "Control Registers:\n"
        "EIP = 0x%08X  ESP = 0x%08X\n"
        "EBP = 0x%08X  EFL = 0x%08X\n",
        info->ContextRecord->IPREG,
        info->ContextRecord->SPREG,
        info->ContextRecord->BPREG,
        info->ContextRecord->EFlags );
    }

    if ( info->ContextRecord->ContextFlags & CONTEXT_INTEGER )
    {
      PrintString( args.m_SEHIntegerRegisters, 
        "Integer Registers:\n"
        "EAX = 0x%08X  EBX = 0x%08X\n"
        "ECX = 0x%08X  EDX = 0x%08X\n"
        "ESI = 0x%08X  EDI = 0x%08X\n",
        info->ContextRecord->AXREG,
        info->ContextRecord->BXREG,
        info->ContextRecord->CXREG,
        info->ContextRecord->DXREG,
        info->ContextRecord->SIREG,
        info->ContextRecord->DIREG );
    }
  }

  std::vector<uintptr> trace;
  if ( GetStackTrace( info->ContextRecord, trace ) )
  {
    TranslateStackTrace( trace, args.m_Callstack );
  }
}

std::string Debug::GetExceptionInfo(LPEXCEPTION_POINTERS info)
{
  ExceptionArgs args ( ExceptionTypes::SEH, false );
  GetExceptionDetails( info, args );

  std::string buffer;
  buffer += "An exception has occurred\n";

  switch ( args.m_Type )
  {
  case ExceptionTypes::CPP:
    {
      PrintString( buffer, "Type:    C++ Exception\n" );
      PrintString( buffer, "Class:   %s\n", args.m_CPPClass.c_str() );

      if ( !args.m_Message.empty() )
      {
        PrintString( buffer, "Message:\n%s\n", args.m_Message.c_str() );
      }

      break;
    }

  case ExceptionTypes::SEH:
    {
      PrintString( buffer, "Type:    SEH Exception\n" );
      PrintString( buffer, "Code:    0x%08X\n", args.m_SEHCode );
      PrintString( buffer, "Class:   %s\n", args.m_SEHClass.c_str() );

      if ( !args.m_Message.empty() )
      {
        PrintString( buffer, "Message:\n%s\n", args.m_Message.c_str() );
      }

      if ( !args.m_SEHControlRegisters.empty() )
      {
        PrintString( buffer, "\n%s", args.m_SEHControlRegisters.c_str() );
      }

      if ( !args.m_SEHIntegerRegisters.empty() )
      {
        PrintString( buffer, "\n%s", args.m_SEHIntegerRegisters.c_str() );
      }
      
      break;
    }
  }

  buffer += "\nCallstack:\n";

  if ( !args.m_Callstack.empty() )
  {
    PrintString( buffer, "%s", args.m_Callstack.c_str() );
  }
  else
  {
    buffer += "No call stack info\n";
  }

  V_string::const_iterator itr = args.m_Threads.begin();
  V_string::const_iterator end = args.m_Threads.end();
  for ( ; itr != end; ++itr )
  {
    PrintString( buffer, "\n%s", itr->c_str() );
  }

  return buffer;
}

std::string Debug::WriteDump(LPEXCEPTION_POINTERS info, bool full)
{
  char* tempDir = getenv( "NOC_PROJECT_TMP" );
  if ( tempDir == NULL )
  {
    Log::Error( "Failed to write crash dump because the temporary directory (%s) to save the file to could not be determined.\n", "NOC_PROJECT_TMP" );
    return "";
  }

  // Make sure that the directory exists
  char directory[MAX_PATH] = { '\0' };
  sprintf_s( directory, sizeof( directory ) - 1, "%s\\crashdumps", tempDir );
  SHCreateDirectoryEx( NULL, directory, NULL );

  // Tack time (in seconds since UTC) onto end of file name
  time_t now;
  time( &now );

  char module[MAX_PATH];
  char file[MAX_PATH];
  GetModuleFileName( 0, module, MAX_PATH );
  _splitpath( module, NULL, NULL, file, NULL );

  char dmp_file[MAX_PATH] = { '\0' };
  sprintf_s( dmp_file, sizeof( dmp_file ) - 1, "%s\\%s_%ld.dmp", directory, file, now );

  HANDLE dmp;
  dmp = CreateFile( dmp_file, FILE_ALL_ACCESS, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );
  if ( dmp!=INVALID_HANDLE_VALUE )
  {
    MINIDUMP_EXCEPTION_INFORMATION ex;
    ex.ClientPointers = true;
    ex.ExceptionPointers = info;
    ex.ThreadId = GetCurrentThreadId();

    _MINIDUMP_TYPE type;

    if ( full )
    {
      type = MiniDumpWithFullMemory;
    }
    else
    {
      type = MiniDumpNormal;
    }

    // generate the minidump
    MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), dmp, type, &ex, 0, 0 );

    // close the file
    CloseHandle( dmp );

    return dmp_file;
  }

  return "";
}
