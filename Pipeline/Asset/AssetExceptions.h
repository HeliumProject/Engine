#pragma once

#include "Foundation/Exception.h"
#include "Foundation/TUID.h"

namespace Asset
{
  DEFINE_EXCEPTION_CLASS

  class UnableToLocateAssetClassException : public Asset::Exception
  {
  public:
    UnableToLocateAssetClassException( tuid assetClassID ) : Exception( TXT( "Unable to locate asset class " ) TUID_HEX_FORMAT, assetClassID ) {}
  };

  class UnableToLoadAssetClassException : public Asset::Exception
  {
  public:
    UnableToLoadAssetClassException( tuid assetClassID, const tstring& assetClassFile ) : Exception( TXT( "Unable to load asset class " ) TUID_HEX_FORMAT TXT( " (%s)" ), assetClassID, assetClassFile.c_str() ) {}
  };
}
