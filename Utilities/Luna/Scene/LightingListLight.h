#pragma once

#include "LightingList.h"
#include "Light.h"

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // UI for the list of lights in the Lighting UI
  //
  class LightingListLight : public LightingList< Luna::Light >
  {
  private:
    enum Column
    {
      ColumnName = 0,
      ColumnZone,
    };

  public:
    LightingListLight( wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxLC_ICON, const wxValidator& validator = wxDefaultValidator, const wxString& name = "LightingListView" );
    virtual ~LightingListLight();

    virtual void RemoveSceneItems( Luna::Scene* scene ) NOC_OVERRIDE;

  protected:
    virtual void DoAddListItem( Luna::Light* item, const i32 itemID ) NOC_OVERRIDE;
  };
}
