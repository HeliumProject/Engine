#include "Windows.h"
#include "Error.h"

#include <eh.h>
#include <new.h>
#include <stdlib.h>

using namespace Windows;

std::string Windows::GetErrorString( DWORD errorOverride )
{
  // get the system error
  DWORD error = ( errorOverride != 0 ) ? errorOverride : GetLastError();

  LPVOID lpMsgBuf;
  ::FormatMessage( 
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
    NULL,
    error,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR) &lpMsgBuf,
    0,
    NULL 
    );

  std::string result;

  if (lpMsgBuf)
  {
    result = (char*)lpMsgBuf;

    ::LocalFree( lpMsgBuf );
  }
  else
  {
    result = "Unknown error (the error code could not be translated)";
  }

  // trim enter characters from message
  while (!result.empty() && (*result.rbegin() == '\n' || *result.rbegin() == '\r'))
  {
    result.resize( result.size() - 1 );
  }

  return result;
}

int NewHandler( size_t size )
{
  throw Nocturnal::Exception("Failed to allocate %u bytes", size);
}

void PureCallHandler()
{
  throw Nocturnal::Exception("A pure virtual function was called");
}

void InvalidParameterHandler( const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t /*pReserved*/ )
{
#ifdef _DEBUG

  static const wchar_t unknown[] = L"Unknown"; // this is *probably* overkill, but just for insurance

  char ex[ 256 ];
  wcstombs( ex, expression ? expression : unknown, sizeof(ex) );
  char fn[ 256 ];
  wcstombs( fn, function ? function : unknown, sizeof(fn) );
  char fl[ 256 ];
  wcstombs( fl, file ? file : unknown, sizeof(fl) );

  throw Nocturnal::Exception("An invalid parameter was passed: Expression: %s Function: %s File: %s Line: %d\n", ex, fn, fl, line );

#else

  throw Nocturnal::Exception("An invalid parameter was passed");

#endif
}

void Windows::EnableCPPErrorHandling(bool enable)
{
  if (enable)
  {
    _set_new_handler( &NewHandler );
  }
  else if ( _query_new_handler() == &NewHandler )
  {
    _set_new_handler( NULL );
  }

  if (enable)
  {
    _set_purecall_handler( &PureCallHandler );
  }
  else if ( _get_purecall_handler() == &PureCallHandler )
  {
    _set_purecall_handler( NULL );
  }

  if (enable)
  {
    _set_invalid_parameter_handler( &InvalidParameterHandler );
  }
  else if ( _get_invalid_parameter_handler() == &InvalidParameterHandler )
  {
    _set_invalid_parameter_handler( NULL );
  }
}