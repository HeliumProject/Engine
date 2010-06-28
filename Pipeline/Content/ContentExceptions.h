#pragma once

#include "Foundation/Exception.h"

namespace Content
{
  DEFINE_EXCEPTION_CLASS

  class MissingJointException : public Content::Exception
  {
  public:
    MissingJointException( const tstring& jointId, const tstring& dataLocation ) : Exception( TXT( "Could not locate joint [%s] in %s." ), jointId.c_str(), dataLocation.c_str() ) {}
  };
}