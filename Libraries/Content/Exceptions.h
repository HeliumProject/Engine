#pragma once

#include "Common/Exception.h"

namespace Content
{
  DEFINE_EXCEPTION_CLASS

  class MissingJointException : public Content::Exception
  {
  public:
    MissingJointException( const std::string& jointId, const std::string& dataLocation ) : Exception( "Could not locate joint [%s] in %s.", jointId.c_str(), dataLocation.c_str() ) {}
  };
}