#pragma once

#include "Platform/Exception.h"
#include "Foundation/TUID.h"

namespace Asset
{
  class UnableToLocateAssetClassException : public Nocturnal::Exception
  {
  public:
    UnableToLocateAssetClassException( tuid assetClassID ) : Exception( TXT( "Unable to locate asset class " ) TUID_HEX_FORMAT, assetClassID ) {}
  };

  class UnableToLoadAssetClassException : public Nocturnal::Exception
  {
  public:
    UnableToLoadAssetClassException( tuid assetClassID, const tstring& assetClassFile ) : Exception( TXT( "Unable to load asset class " ) TUID_HEX_FORMAT TXT( " (%s)" ), assetClassID, assetClassFile.c_str() ) {}
  };
}
