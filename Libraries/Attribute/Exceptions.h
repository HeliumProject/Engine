#pragma once

#include "Foundation/Exception.h"
#include "Foundation/TUID.h"

namespace Attribute
{
  DEFINE_EXCEPTION_CLASS

  class InvalidAttributeCollectionException : public Nocturnal::Exception
  {
  public:
    InvalidAttributeCollectionException() : Exception( "Trying to operate on an invalid attribute collection!" ) {}
  };

  class MissingAttributeException : public Nocturnal::Exception
  {
  public:
    MissingAttributeException( const char* typeName ) : Exception( "Trying to access a non-existent attribute of type '%s'", typeName ) {}
  };

  class DisabledAttributeException : public Nocturnal::Exception
  {
  public:
    DisabledAttributeException( const char* typeName ) : Exception( "Trying to access a disabled attribute of type '%s'", typeName ) {}
  };
}
