#pragma once

#include "LightingList.h"
#include "Entity.h"

namespace Luna
{
  // Forwards
  struct InstanceTypeChangeArgs;
  struct EntityAssetSetChangeArgs;

  /////////////////////////////////////////////////////////////////////////////
  // List UI for lightable instances.
  // 
  class LightingListObject : public LightingList< Luna::Entity >
  {
  private:
    enum Column
    {
      ColumnName = 0,
      ColumnZone,
      ColumnLit,
      ColumnLightmap,
      ColumnLightmap1,
      ColumnLightmap2,
      ColumnTexelsPerMeter,
    };

  public:
    LightingListObject( wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxLC_ICON, const wxValidator& validator = wxDefaultValidator, const wxString& name = "LightingListView" );
    virtual ~LightingListObject();

    virtual void RemoveSceneItems( Luna::Scene* scene ) NOC_OVERRIDE;
    void UpdateLit( Luna::Entity* item, bool lit );


  protected:
    virtual void SceneAdded( const SceneChangeArgs& args ) NOC_OVERRIDE;
    virtual void SceneRemoved( const SceneChangeArgs& args ) NOC_OVERRIDE;
    virtual void DoAddListItem( Luna::Entity* item, const i32 itemID ) NOC_OVERRIDE;
    virtual bool ShouldAddItem( Luna::Entity* item ) NOC_OVERRIDE;

    void EntitySetAdded( const NodeChangeArgs& args );
    void EntitySetRemoved( const NodeChangeArgs& args );
    void EntityAssetSetReloaded( const EntityAssetSetChangeArgs& args );
    void ObjectChanged( const ObjectChangeArgs& args );
    void UpdateLightMapSetting( Luna::Entity* item, long row );
    void UpdateTPM( Luna::Entity* item, long row );
    
  };
}
