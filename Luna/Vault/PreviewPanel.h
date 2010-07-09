#pragma once

#include "VaultGenerated.h"

namespace Asset
{
  class AssetClass;
  typedef Nocturnal::SmartPtr< AssetClass > AssetClassPtr;
}

namespace Reflect
{
  struct ElementChangeArgs;
}

namespace Luna
{
  class VaultFrame;

  class PreviewPanel : public PreviewPanelGenerated
  {
  public:
		PreviewPanel( VaultFrame* browserFrame );
		virtual ~PreviewPanel();

    void Preview( Asset::AssetClass* asset );

    void DisplayReferenceAxis( bool display = true );

  private:
    VaultFrame* m_VaultFrame;
    Asset::AssetClassPtr m_PreviewAsset;
    tstring m_ContentFile;
  };
}