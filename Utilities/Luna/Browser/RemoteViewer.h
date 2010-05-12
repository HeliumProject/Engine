#pragma once

#include "Live/RemoteEditor.h"

namespace Luna
{
  class RemoteViewer : public Luna::RemoteEditor
  {
  public:
    RemoteViewer( wxWindow* win );
    virtual ~RemoteViewer();

    void BuildAndViewAsset( Asset::AssetClass* assetClass, bool showOptions );

  private:
    wxWindow* m_Window;
  };
}
