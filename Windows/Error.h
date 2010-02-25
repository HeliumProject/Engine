#pragma once

#include <string>

#ifndef REQUIRES_WINDOWS_H
#define REQUIRES_WINDOWS_H
#endif
#include "API.h"

#include "Common/Exception.h"

namespace Windows
{
  //
  // Decode the last error string
  //

  WINDOWS_API std::string GetErrorString( DWORD errorOverride = 0 );

  WINDOWS_API void EnableCPPErrorHandling( bool enable = true );


  //
  // Windows::Exception class
  //

  class Exception : public Nocturnal::Exception
  {
  protected:
    int m_ErrorCode;

  protected:
    Exception() throw() // hide default c_tor
      : m_ErrorCode( 0 )
    {

    }

  public:
    Exception( const char *msgFormat, ... )
      : m_ErrorCode( GetLastError() )
    {
      va_list msgArgs;
      va_start( msgArgs, msgFormat );
      SetMessage( msgFormat, msgArgs );
      va_end( msgArgs );
    }

    Exception( DWORD dwError, const char *msgFormat, ... )
      : m_ErrorCode( dwError )
    {
      va_list msgArgs;
      va_start( msgArgs, msgFormat );
      SetMessage( msgFormat, msgArgs );
      va_end( msgArgs );
    }

    virtual int GetErrorCode() const throw()
    {
      return m_ErrorCode;
    }

  protected:
    virtual void SetMessage( const char *msgFormat, va_list msgArgs )
    {
      __super::SetMessage( msgFormat, msgArgs );

      // append system error information
      m_Message += " (" + GetErrorString(m_ErrorCode) + ")";
    }
  };
}