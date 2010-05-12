#pragma once

// Includes
#include "API.h"
#include "LeafItem.h"
#include "TUID/TUID.h"

namespace Luna
{
  // Forwards
  class LightingJob;
  class Light;

  /////////////////////////////////////////////////////////////////////////////
  // Class representing unloaded zones displayed in the lighting UI tree.  
  // 
  class ZoneUnloadedItem : public LeafItem
  {
  private:
    std::string m_Label;
    tuid m_FileID;

  public:
    ZoneUnloadedItem( wxTreeCtrlBase* tree, Luna::LightingJob* job, const tuid& fileID );
    virtual ~ZoneUnloadedItem();

    const std::string& GetLabel() const;
    tuid GetFileID() const;

    // Overrides
    virtual Undo::CommandPtr GetRemoveCommand( const OS_SelectableDumbPtr& selection ) NOC_OVERRIDE;
  };
}