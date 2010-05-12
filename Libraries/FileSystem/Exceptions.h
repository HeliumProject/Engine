#pragma once

#include "Common/Compiler.h"
#include "Common/Exception.h"

namespace FileSystem
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

  class FileDoesNotExistException : public FileSystem::Exception
  {
  public:
    FileDoesNotExistException( const char *path ) : Exception( "File does not exist: %s", path ) {}
  };

  class InvalidFilePathException : public FileSystem::Exception
  {
  public:
    InvalidFilePathException( const char *path ) : Exception( "Invalid file path: %s", path ) {}
  };

  class FileOperationException : public FileSystem::Exception
  {
  public:
    FileOperationException( const char *msgFormat, ... ) : Exception()
    {
      va_list msgArgs;
      va_start( msgArgs, msgFormat );
      SetMessage( msgFormat, msgArgs );
      va_end( msgArgs );
    }
  };
};