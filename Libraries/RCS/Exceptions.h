#pragma once

#include "Platform/Compiler.h"
#include "Foundation/Exception.h"

namespace RCS
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

  class FileInUseException : public RCS::Exception
  {
  public:
    FileInUseException( const char *path, const char *username ) : Exception( "File '%s' is currently in use by '%s'.", path, username) {}
  };

  class FileOutOfDateException : public RCS::Exception
  {
  public:
    FileOutOfDateException( const char *path, const int curRev, const int headRev ) : Exception( "File '%s' is not up to date (local revision: %d / remote revision: %d).  Please sync the file to resolve this error.", path, curRev, headRev ) {}
  };
}