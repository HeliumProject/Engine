#pragma once

#include "BrowserGenerated.h"

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
  class BrowserFrame;

  class PreviewPanel : public PreviewPanelGenerated
  {
  public:
		PreviewPanel( BrowserFrame* browserFrame );
		virtual ~PreviewPanel();

    void Preview( Asset::AssetClass* asset );

    void DisplayReferenceAxis( bool display = true );

  private:
    BrowserFrame* m_BrowserFrame;
    Asset::AssetClassPtr m_PreviewAsset;
    std::string m_ContentFile;
  };
}