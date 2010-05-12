#pragma once

#include "LightingList.h"
#include "CubeMapProbe.h"

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // List UI for cube map probes.
  // 
  class LightingListProbe : public LightingList< Luna::CubeMapProbe >
  {
  private:
    enum Column
    {
      ColumnName = 0,
      ColumnZone,
    };

  public:
    LightingListProbe( wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxLC_ICON, const wxValidator& validator = wxDefaultValidator, const wxString& name = "LightingListView" );
    virtual ~LightingListProbe();

    virtual void RemoveSceneItems( Luna::Scene* scene ) NOC_OVERRIDE;

  protected:
    virtual void DoAddListItem( Luna::CubeMapProbe* item, const i32 itemID ) NOC_OVERRIDE;
  };
}
