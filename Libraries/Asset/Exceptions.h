#pragma once

#include "Foundation/Exception.h"
#include "Foundation/TUID.h"

namespace Asset
{
  DEFINE_EXCEPTION_CLASS

  class UnableToLocateAssetClassException : public Asset::Exception
  {
  public:
    UnableToLocateAssetClassException( tuid assetClassID ) : Exception( "Unable to locate asset class "TUID_HEX_FORMAT, assetClassID ) {}
  };

  class UnableToLoadAssetClassException : public Asset::Exception
  {
  public:
    UnableToLoadAssetClassException( tuid assetClassID, const std::string& assetClassFile ) : Exception( "Unable to load asset class "TUID_HEX_FORMAT" (%s)", assetClassID, assetClassFile.c_str() ) {}
  };
}
