#pragma once

#include "Common/Compiler.h"
#include "Common/Exception.h"

namespace NSL
{
  //
  // Implementation of Nocturnal::Exception
  //

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

  ///////////////////////////////////////////////////////////////////////////////////////////

  class FailedCopyException : public Exception
  {
  public:
    FailedCopyException( const char* source, const char* target, const char* error )
      : Exception( "Failed to copy %s to %s: %s", source, target, error )
    {

    }
  };

  class FailedToGetAttributesException : public Exception
  {
  public:
    FailedToGetAttributesException( const char* file )
      : Exception( "Could not get file attributes on file: %s", file )
    {
    
    }
  };
};