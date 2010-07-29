#pragma once

#include "VaultGenerated.h"

namespace Asset
{
  class AssetFile;
}

namespace Editor
{
  class DetailsFrame : public DetailsFrameGenerated
  {
  public:
    DetailsFrame( wxWindow* parent );

    void Populate( const Helium::Path& path );
  };
}
