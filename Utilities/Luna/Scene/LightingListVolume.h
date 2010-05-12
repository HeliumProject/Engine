#pragma once

#include "LightingList.h"
#include "LightingVolume.h"

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // List UI for lighting volumes.
  // 
  class LightingListVolume : public LightingList< Luna::LightingVolume >
  {
  private:
    enum Column
    {
      ColumnName = 0,
      ColumnZone,
    };

  public:
    LightingListVolume( wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxLC_ICON, const wxValidator& validator = wxDefaultValidator, const wxString& name = "LightingListView" );
    virtual ~LightingListVolume();

    virtual void RemoveSceneItems( Luna::Scene* scene ) NOC_OVERRIDE;

  protected:
    virtual void DoAddListItem( Luna::LightingVolume* item, const i32 itemID ) NOC_OVERRIDE;
  };
}
