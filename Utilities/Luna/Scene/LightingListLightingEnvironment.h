#pragma once

#include "LightingList.h"
#include "LightingEnvironment.h"

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // UI for the list of lights in the Lighting UI
  //
  class LightingListLightingEnvironment : public LightingList< Luna::LightingEnvironment >
  {
  private:
    enum Column
    {
      ColumnName = 0,
      ColumnZone,
    };

  public:
    LightingListLightingEnvironment( wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxLC_ICON, const wxValidator& validator = wxDefaultValidator, const wxString& name = "LightingListView" );
    virtual ~LightingListLightingEnvironment();

    virtual void RemoveSceneItems( Luna::Scene* scene ) NOC_OVERRIDE;

  protected:
    virtual void DoAddListItem( Luna::LightingEnvironment* item, const i32 itemID ) NOC_OVERRIDE;
  };
}
