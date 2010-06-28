#pragma once

#include "Foundation/Exception.h"
#include "Foundation/TUID.h"

namespace Component
{
  DEFINE_EXCEPTION_CLASS

  class InvalidComponentCollectionException : public Nocturnal::Exception
  {
  public:
    InvalidComponentCollectionException() : Exception( TXT( "Trying to operate on an invalid attribute collection!" ) ) {}
  };

  class MissingComponentException : public Nocturnal::Exception
  {
  public:
    MissingComponentException( const tchar* typeName ) : Exception( TXT( "Trying to access a non-existent attribute of type '%s'" ), typeName ) {}
  };

  class DisabledComponentException : public Nocturnal::Exception
  {
  public:
    DisabledComponentException( const tchar* typeName ) : Exception( TXT( "Trying to access a disabled attribute of type '%s'" ), typeName ) {}
  };
}
