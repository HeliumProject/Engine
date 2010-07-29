#pragma once

#include "Platform/Compiler.h"
#include "Platform/Exception.h"

namespace Application
{
  class Exception : public Helium::Exception
  {
  public:
    Exception( const tchar *msgFormat, ... )
    {
      va_list msgArgs;
      va_start( msgArgs, msgFormat );
      SetMessage( msgFormat, msgArgs );
      va_end( msgArgs );
    }

  protected:
    Exception() throw() {} // hide default c_tor
  };

  // Should only be thrown when a required tools version check fails.
  // Differentiates this kind of exception from other Application::Exceptions.
  class CheckVersionException : public Application::Exception
  {
  public:
    CheckVersionException( const tchar* msg )
      : Application::Exception( msg )
    {
    }
  };
}
