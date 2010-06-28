#pragma once

#include "Platform/Compiler.h"
#include "Platform/Exception.h"

namespace Perforce
{
  class Exception : public Nocturnal::Exception
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
}