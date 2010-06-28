#include "Debug.h"

#include "Platform/Types.h"
#include "Platform/Mutex.h"
#include "Platform/Error.h"
#include "Platform/Assert.h"
#include "Platform/String.h"
#include "Platform/Windows/Windows.h"

#include <map>
#include <time.h>
#include <shlobj.h>
#include <dbghelp.h>
#include <tlhelp32.h>
#pragma comment ( lib, "dbghelp.lib" )

#ifdef _UNICODE
# undef IMAGEHLP_MODULE64
# define IMAGEHLP_MODULE64 IMAGEHLP_MODULEW64
# undef IMAGEHLP_LINE64
# define IMAGEHLP_LINE64 IMAGEHLP_LINEW64
# define SymInitialize SymInitializeW
# define SymGetModuleInfo64 SymGetModuleInfoW64
# define SymGetLineFromAddr64 SymGetLineFromAddrW64
#endif

//#define DEBUG_SYMBOLS

// disable the optimizer if debugging in release
#if defined(DEBUG_SYMBOLS) && defined(NDEBUG)
#pragma optimize("", off)
#endif

// loaded flag
static bool g_Initialized = false;

// Utility to print to a string
static void PrintString(tstring& buffer, const tchar* tstring, ...)
{
  static tchar buf[4096];

  va_list argptr;
  va_start(argptr, tstring);
  _vsntprintf(buf, sizeof(buf), tstring, argptr);
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
        _tprintf( TXT("Success loading symbols for module: %s, base: 0x%08I64X, size: %u: %s\n"), name, base, size, moduleInfo.LoadedPdbName );
      }
      else
      {
        _tprintf( TXT("Success loading symbols for module: %s, base: 0x%08I64X, size: %u\n"), name, base, size );
      }
    }
    else
    {
      _tprintf( TXT("Failure loading symbols for module: %s: %s\n"), name, Platform::GetErrorString().c_str() );
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

bool Debug::Initialize(const tstring& pdbPaths)
{
  if ( !g_Initialized )
  {
    tstring dir;

    if ( pdbPaths.empty() )
    {
      tchar module[MAX_PATH];
      tchar drive[MAX_PATH];
      tchar path[MAX_PATH];
      tchar file[MAX_PATH];
      tchar ext[MAX_PATH];
      GetModuleFileName(0,module,MAX_PATH);
      _tsplitpath(module,drive,path,file,ext);

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

    _tprintf( TXT("Symbol Path: %s\n"), dir.c_str() );

    // initialize symbols (dbghelp.dll)
    if ( SymInitialize(GetCurrentProcess(), dir.c_str(), FALSE) == 0 )
    {
      _tprintf( TXT("Failure initializing symbol API: %s\n"), Platform::GetErrorString().c_str() );
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

tstring Debug::GetSymbolInfo(uintptr adr, bool enumLoadedModules)
{
  NOC_ASSERT( Debug::IsInitialized() );

  if ( enumLoadedModules )
  {
    // load debugging information
    EnumerateLoadedModules();
  }

  // module image name "reflect.dll"
  static tchar module[MAX_PATH];
  ZeroMemory(&module, sizeof(module));
  static tchar extension[MAX_PATH];
  ZeroMemory(&extension, sizeof(extension));

  // symbol name "Reflect::Class::AddSerializer + 0x16d"
  static tchar symbol[MAX_SYM_NAME+16];
  ZeroMemory(&symbol, sizeof(symbol));

  // source file name "typeinfo.cpp"
  static tchar filename[MAX_PATH];
  ZeroMemory(&filename, sizeof(filename));

  // line number in source "246"
  DWORD line = 0xFFFFFFFF;

  // resulting line is worst case of all components
  static tchar result[sizeof(module) + sizeof(symbol) + sizeof(filename) + 64];
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
    _tsplitpath(moduleInfo.ImageName, NULL, NULL, module, extension);
    _tcscat(module, extension);

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
      _stprintf(symbol, TXT("%s + 0x%X"), symbolInfo->Name, disp);

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
        _tcscpy(filename, l.FileName);
        static tchar ext[MAX_PATH];
        static tchar file[MAX_PATH];
        _tsplitpath(filename, NULL, NULL, file, ext);

        _stprintf(result, TXT("%s, %s : %s%s(%d)"), module, symbol, file, ext, l.LineNumber);
        return result;
      }

      _stprintf(result, TXT("%s, %s"), module, symbol);
      return result;
    }

    _stprintf(result, TXT("%s"), module);
    return result;
  }
  else
  {
    DWORD err = GetLastError();
    return TXT("Unknown");
  }
}

Nocturnal::Exception* Debug::GetNocturnalException(uintptr addr)
{
  Nocturnal::Exception* cppException = (Nocturnal::Exception*)addr;

  __try
  {
    // if its non-null
    if (cppException)
    {
      // this will explode if the address isn't really a c++ exception (std::exception)
      cppException->What();
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

std::exception* Debug::GetStandardException(uintptr addr)
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

  volatile tchar *p = 0;
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

void Debug::TranslateStackTrace(const std::vector<uintptr>& trace, tstring& buffer)
{
  std::vector<uintptr>::const_iterator itr = trace.begin();
  std::vector<uintptr>::const_iterator end = trace.end();
  for ( ; itr != end; ++itr )
  {
    PrintString(buffer, TXT("0x%08I64X - %s\n"), *itr, GetSymbolInfo(*itr, false).c_str() );
  }
}

const tchar* Debug::GetExceptionClass(u32 exceptionCode)
{
  const tchar* ex_name = NULL;

  switch (exceptionCode)
  {
  case EXCEPTION_ACCESS_VIOLATION:
    ex_name = TXT("EXCEPTION_ACCESS_VIOLATION");
    break;

  case EXCEPTION_BREAKPOINT:
    ex_name = TXT("EXCEPTION_BREAKPOINT");
    break;

  case EXCEPTION_SINGLE_STEP:
    ex_name = TXT("EXCEPTION_SINGLE_STEP");
    break;

  case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
    ex_name = TXT("EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
    break;

  case EXCEPTION_FLT_DENORMAL_OPERAND:
    ex_name = TXT("EXCEPTION_FLT_DENORMAL_OPERAND");
    break;

  case EXCEPTION_FLT_DIVIDE_BY_ZERO:
    ex_name = TXT("EXCEPTION_FLT_DIVIDE_BY_ZERO");
    break;

  case EXCEPTION_FLT_INEXACT_RESULT:
    ex_name = TXT("EXCEPTION_FLT_INEXACT_RESULT");
    break;

  case EXCEPTION_FLT_INVALID_OPERATION:
    ex_name = TXT("EXCEPTION_FLT_INVALID_OPERATION");
    break;

  case EXCEPTION_FLT_OVERFLOW:
    ex_name = TXT("EXCEPTION_FLT_OVERFLOW");
    break;

  case EXCEPTION_FLT_STACK_CHECK:
    ex_name = TXT("EXCEPTION_FLT_STACK_CHECK");
    break;

  case EXCEPTION_FLT_UNDERFLOW:
    ex_name = TXT("EXCEPTION_FLT_UNDERFLOW");
    break;

  case EXCEPTION_INT_DIVIDE_BY_ZERO:
    ex_name = TXT("EXCEPTION_INT_DIVIDE_BY_ZERO");
    break;

  case EXCEPTION_INT_OVERFLOW:
    ex_name = TXT("EXCEPTION_INT_OVERFLOW");
    break;

  case EXCEPTION_PRIV_INSTRUCTION:
    ex_name = TXT("EXCEPTION_PRIV_INSTRUCTION");
    break;

  case EXCEPTION_IN_PAGE_ERROR:
    ex_name = TXT("EXCEPTION_IN_PAGE_ERROR");
    break;

  case EXCEPTION_ILLEGAL_INSTRUCTION:
    ex_name = TXT("EXCEPTION_ILLEGAL_INSTRUCTION");
    break;

  case EXCEPTION_NONCONTINUABLE_EXCEPTION:
    ex_name = TXT("EXCEPTION_NONCONTINUABLE_EXCEPTION");
    break;

  case EXCEPTION_STACK_OVERFLOW:
    ex_name = TXT("EXCEPTION_STACK_OVERFLOW");
    break;

  case EXCEPTION_INVALID_DISPOSITION:
    ex_name = TXT("EXCEPTION_INVALID_DISPOSITION");
    break;

  case EXCEPTION_GUARD_PAGE:
    ex_name = TXT("EXCEPTION_GUARD_PAGE");
    break;

  case EXCEPTION_INVALID_HANDLE:
    ex_name = TXT("EXCEPTION_INVALID_HANDLE");
    break;

  case 0xC00002B5:
    ex_name = TXT("Multiple floating point traps");
    break;
  }

  if (ex_name == NULL)
  {
    ex_name = TXT("Unknown Exception");
  }

  return ex_name;
}

void Debug::GetExceptionDetails( LPEXCEPTION_POINTERS info, ExceptionArgs& args )
{
  static Platform::Mutex s_ExceptionMutex;
  Platform::TakeMutex mutex ( s_ExceptionMutex );

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

    args.m_Threads.push_back( TXT("") );
    PrintString( args.m_Threads.back(), TXT("Thread %d:\n"), id );
    tstring::size_type size = args.m_Threads.back().size();

    CONTEXT context;
    memset(&context, 0, sizeof( context ));
    context.ContextFlags = CONTEXT_FULL;
    if ( ::GetThreadContext( handle, &context ) )
    {
      PrintString( args.m_Threads.back(), 
        TXT("\nControl Registers:\n")
        TXT("EIP = 0x%08X  ESP = 0x%08X\n")
        TXT("EBP = 0x%08X  EFL = 0x%08X\n"),
        info->ContextRecord->IPREG,
        info->ContextRecord->SPREG,
        info->ContextRecord->BPREG,
        info->ContextRecord->EFlags );

      PrintString( args.m_Threads.back(), 
        TXT("\nInteger Registers:\n")
        TXT("EAX = 0x%08X  EBX = 0x%08X\n")
        TXT("ECX = 0x%08X  EDX = 0x%08X\n")
        TXT("ESI = 0x%08X  EDI = 0x%08X\n"),
        info->ContextRecord->AXREG,
        info->ContextRecord->BXREG,
        info->ContextRecord->CXREG,
        info->ContextRecord->DXREG,
        info->ContextRecord->SIREG,
        info->ContextRecord->DIREG );

      PrintString( args.m_Threads.back(), TXT("\nCallstack:\n") );

      std::vector<uintptr> trace;
      if ( GetStackTrace( &context, trace ) )
      {
        TranslateStackTrace( trace, args.m_Threads.back() );
      }
    }

    if ( args.m_Threads.back().size() == size )
    {
      args.m_Threads.back() += TXT("No thread info\n");
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

    Nocturnal::Exception* nocturnalException = GetNocturnalException(info->ExceptionRecord->ExceptionInformation[1]);
    if (nocturnalException)
    {
      const char* cppClass = NULL;
      try
      {
        cppClass = typeid(*nocturnalException).name();
      }
      catch (...)
      {
        cppClass = "Unknown";
      }

      Platform::ConvertString( cppClass, args.m_CPPClass );
      Platform::ConvertString( nocturnalException->What(), args.m_Message );
    }
    else
    {
        std::exception* standardException = GetStandardException(info->ExceptionRecord->ExceptionInformation[1]);
        if (standardException)
        {
          const char* cppClass = NULL;
          try
          {
            cppClass = typeid(*standardException).name();
          }
          catch (...)
          {
              cppClass = "Unknown";
          }

          Platform::ConvertString( cppClass, args.m_CPPClass );
          Platform::ConvertString( standardException->what(), args.m_Message );
        }
    }
    
    if ( args.m_CPPClass.empty() && args.m_Message.empty() )
    {
      args.m_Message = TXT("Thrown object is not a known type of C++ exception");
    }

    info->ContextRecord->IPREG = (DWORD)info->ExceptionRecord->ExceptionInformation[2];
  }
  else
  {
    args.m_Type = ExceptionTypes::SEH;

    if (args.m_SEHCode == EXCEPTION_ACCESS_VIOLATION)
    {
      PrintString( args.m_Message, TXT("Attempt to %s address 0x%08X"), (info->ExceptionRecord->ExceptionInformation[0]==1)?TXT("write to"):TXT("read from"), info->ExceptionRecord->ExceptionInformation[1]);
    }

    if (info->ContextRecord->ContextFlags & CONTEXT_CONTROL)
    {
      PrintString( args.m_SEHControlRegisters, 
        TXT("Control Registers:\n")
        TXT("EIP = 0x%08X  ESP = 0x%08X\n")
        TXT("EBP = 0x%08X  EFL = 0x%08X\n"),
        info->ContextRecord->IPREG,
        info->ContextRecord->SPREG,
        info->ContextRecord->BPREG,
        info->ContextRecord->EFlags );
    }

    if ( info->ContextRecord->ContextFlags & CONTEXT_INTEGER )
    {
      PrintString( args.m_SEHIntegerRegisters, 
        TXT("Integer Registers:\n")
        TXT("EAX = 0x%08X  EBX = 0x%08X\n")
        TXT("ECX = 0x%08X  EDX = 0x%08X\n")
        TXT("ESI = 0x%08X  EDI = 0x%08X\n"),
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

tstring Debug::GetExceptionInfo(LPEXCEPTION_POINTERS info)
{
  ExceptionArgs args ( ExceptionTypes::SEH, false );
  GetExceptionDetails( info, args );

  tstring buffer;
  buffer += TXT("An exception has occurred\n");

  switch ( args.m_Type )
  {
  case ExceptionTypes::CPP:
    {
      PrintString( buffer, TXT("Type:    C++ Exception\n") );
      PrintString( buffer, TXT("Class:   %s\n"), args.m_CPPClass.c_str() );

      if ( !args.m_Message.empty() )
      {
        PrintString( buffer, TXT("Message:\n%s\n"), args.m_Message.c_str() );
      }

      break;
    }

  case ExceptionTypes::SEH:
    {
      PrintString( buffer, TXT("Type:    SEH Exception\n") );
      PrintString( buffer, TXT("Code:    0x%08X\n"), args.m_SEHCode );
      PrintString( buffer, TXT("Class:   %s\n"), args.m_SEHClass.c_str() );

      if ( !args.m_Message.empty() )
      {
        PrintString( buffer, TXT("Message:\n%s\n"), args.m_Message.c_str() );
      }

      if ( !args.m_SEHControlRegisters.empty() )
      {
        PrintString( buffer, TXT("\n%s"), args.m_SEHControlRegisters.c_str() );
      }

      if ( !args.m_SEHIntegerRegisters.empty() )
      {
        PrintString( buffer, TXT("\n%s"), args.m_SEHIntegerRegisters.c_str() );
      }
      
      break;
    }
  }

  buffer += TXT("\nCallstack:\n");

  if ( !args.m_Callstack.empty() )
  {
    PrintString( buffer, TXT("%s"), args.m_Callstack.c_str() );
  }
  else
  {
    buffer += TXT("No call stack info\n");
  }

  std::vector< tstring >::const_iterator itr = args.m_Threads.begin();
  std::vector< tstring >::const_iterator end = args.m_Threads.end();
  for ( ; itr != end; ++itr )
  {
    PrintString( buffer, TXT("\n%s"), itr->c_str() );
  }

  return buffer;
}

tstring Debug::WriteDump(LPEXCEPTION_POINTERS info, bool full)
{
  tchar* tempDir = _tgetenv( TXT("NOC_PROJECT_TMP") );
  if ( tempDir == NULL )
  {
    _tprintf( TXT("Failed to write crash dump because the temporary directory (%s) to save the file to could not be determined.\n"), TXT("NOC_PROJECT_TMP") );
    return TXT("");
  }

  // Make sure that the directory exists
  tchar directory[MAX_PATH] = { 0 };
  _sntprintf( directory, sizeof( directory ) - 1, TXT("%s\\crashdumps"), tempDir );
  SHCreateDirectoryEx( NULL, directory, NULL );

  // Tack time (in seconds since UTC) onto end of file name
  time_t now;
  time( &now );

  tchar module[MAX_PATH];
  tchar file[MAX_PATH];
  GetModuleFileName( 0, module, MAX_PATH );
  _tsplitpath( module, NULL, NULL, file, NULL );

  tchar dmp_file[MAX_PATH] = { '\0' };
  _sntprintf( dmp_file, sizeof( dmp_file ) - 1, TXT("%s\\%s_%ld.dmp"), directory, file, now );

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

  return TXT("");
}
