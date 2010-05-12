#pragma once

// Includes
#include "API.h"
#include "LightingTreeItem.h"
#include "ZoneUnloadedItem.h"
#include "TUID/TUID.h"

namespace Luna
{
  // Forwards
  class LightGroupItem;
  class LightingJob;
  class Object;
  class ProbeGroupItem;
  class RenderGroupItem;
  class SceneNode;
  class ShadowGroupItem;
  class LightingEnvironmentGroupItem;
  class ZoneGroupItem;

  /////////////////////////////////////////////////////////////////////////////
  // Item data representing a lighting job in the tree control.
  // 
  class JobItem : public LightingTreeItem
  {
  private:
    typedef std::map< tuid, ZoneUnloadedItem* > M_UnloadedZone;

  private:
    LightGroupItem* m_LightGroup;
    RenderGroupItem* m_RenderGroup;
    ShadowGroupItem* m_ShadowGroup;
    ProbeGroupItem* m_ProbeGroup;
    LightingEnvironmentGroupItem* m_EnvironmentGroup;
    ZoneGroupItem* m_ZoneGroup;
    M_UnloadedZone m_UnloadedZones;


  public:
    JobItem( wxTreeCtrlBase* tree, Luna::LightingJob* job );
    virtual ~JobItem();

    void LoadZone( tuid zoneID );
    void UnloadZone( tuid zoneID );

  private:
    void AddUnloadedZoneItem( const tuid& zoneID );
    void RemoveUnloadedZoneItem( const tuid& zoneID );

  public:
    // Overrides
    virtual void Load() NOC_OVERRIDE;
    virtual bool IsSelectable( const OS_TreeItemIds& currentSelection ) const NOC_OVERRIDE;
    virtual void GetSelectableItems( OS_SelectableDumbPtr& selection ) const NOC_OVERRIDE;
    virtual bool CanRename() const NOC_OVERRIDE;
    virtual void Rename( const std::string& newName ) NOC_OVERRIDE;
    virtual bool CanAddChild( Object* object ) const NOC_OVERRIDE;
    virtual Undo::CommandPtr AddChild( Object* object ) NOC_OVERRIDE;
    virtual Undo::CommandPtr GetRemoveCommand( const OS_SelectableDumbPtr& selection ) NOC_OVERRIDE;
  };
}