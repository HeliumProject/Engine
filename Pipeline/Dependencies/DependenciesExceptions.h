#pragma once

#include "Platform/Compiler.h"
#include "Foundation/Exception.h"

namespace Dependencies
{
  DEFINE_EXCEPTION_CLASS
  DEFINE_INIT_EXCEPTIONS_CLASSES
  
  class MaxGraphDepthException : public Exception
  {
  public:
    MaxGraphDepthException( ) : Exception( "Max graph depth was exceeded!" ) {}
  };
};