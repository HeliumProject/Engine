#pragma once

#include "LightingList.h"
#include "Zone.h"

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // UI for the list of lights in the Lighting UI
  //
  class LightingListZone : public LightingList< Zone >
  {
  private:
    enum Column
    {
      ColumnName = 0,
      ColumnZone,
    };

  public:
    LightingListZone( wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxLC_ICON, const wxValidator& validator = wxDefaultValidator, const wxString& name = "LightingListView" );
    virtual ~LightingListZone();

    virtual void RemoveSceneItems( Luna::Scene* scene ) NOC_OVERRIDE;

  protected:
    virtual void DoAddListItem( Zone* item, const i32 itemID ) NOC_OVERRIDE;
  };
}
