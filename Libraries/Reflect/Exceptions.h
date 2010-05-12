#pragma once

#include "Common/Compiler.h"
#include "Common/Exception.h"

namespace Reflect
{
  //
  // Basic exception for all reflect exceptions classes
  //

  class Exception NOC_ABSTRACT : public Nocturnal::Exception
  {

  };

  //
  // For TryCast<>, etc...
  //

  class CastException : public Reflect::Exception
  {
  public:
    CastException( const char *msgFormat, ... )
    {
      va_list msgArgs;
      va_start( msgArgs, msgFormat );
      SetMessage( msgFormat, msgArgs );
      va_end( msgArgs );
    }

  protected:
    CastException() throw() {} // hide default c_tor
  };

  //
  // Base class for heinous exception cases
  //

  class LogisticException : public Reflect::Exception
  {
  public:
    LogisticException( const char *msgFormat, ... )
    {
      va_list msgArgs;
      va_start( msgArgs, msgFormat );
      SetMessage( msgFormat, msgArgs );
      va_end( msgArgs );
    }

  protected:
    LogisticException() throw() {} // hide default c_tor
  };

  //
  // RTTI information is missing or invalid
  //

  class TypeInformationException : public Reflect::LogisticException
  {
  public:
    TypeInformationException( const char *msgFormat, ... )
    {
      va_list msgArgs;
      va_start( msgArgs, msgFormat );
      SetMessage( msgFormat, msgArgs );
      va_end( msgArgs );
    }

  protected:
    TypeInformationException() throw() {} // hide default c_tor
  };

  //
  // Data or its format is invalid or unreadable
  //

  class DataFormatException : public Reflect::LogisticException
  {
  public:
    DataFormatException( const char *msgFormat, ... )
    {
      va_list msgArgs;
      va_start( msgArgs, msgFormat );
      SetMessage( msgFormat, msgArgs );
      va_end( msgArgs );
    }

  protected:
    DataFormatException() throw() {} // hide default c_tor
  };

  //
  // There is a problem dealing with a opening/closing reading/writing to a file or stream
  //

  class StreamException : public Reflect::Exception
  {
  public:
    StreamException( const char *msgFormat, ... )
    {
      va_list msgArgs;
      va_start( msgArgs, msgFormat );
      SetMessage( msgFormat, msgArgs );
      va_end( msgArgs );
    }

  protected:
    StreamException() throw() {} // hide default c_tor
  };

  //
  // There is a problem with the checksum of a file, either its corrupted or Reflect could not generate a checksum
  //

  class ChecksumException : public Reflect::StreamException
  {
  public:
    ChecksumException( const char *msgFormat, ... )
    {
      va_list msgArgs;
      va_start( msgArgs, msgFormat );
      SetMessage( msgFormat, msgArgs );
      va_end( msgArgs );
    }

  protected:
    ChecksumException() throw() {} // hide default c_tor
  };
}