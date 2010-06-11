#pragma once

#include "Platform/Compiler.h"
#include "Foundation/Exception.h"

namespace AppUtils
{
  class Exception : public Nocturnal::Exception
  {
  public:
    Exception( const char *msgFormat, ... )
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
  // Differentiates this kind of exception from other AppUtils::Exceptions.
  class CheckVersionException : public AppUtils::Exception
  {
  public:
    CheckVersionException( const char* msg )
      : AppUtils::Exception( msg )
    {
    }
  };
}
