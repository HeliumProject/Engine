#pragma once

// Includes
#include "API.h"
#include "LightableGroupItem.h"

namespace Luna
{
  // Forwards
  class LightingJob;
  class Object;
  struct LightingJobMemberChangeArgs;

  /////////////////////////////////////////////////////////////////////////////
  // Tree item representing a group of render targets for a given lighting job.
  // 
  class RenderGroupItem : public LightableGroupItem
  {

  public:
    RenderGroupItem( wxTreeCtrlBase* tree, Luna::LightingJob* job );
    virtual ~RenderGroupItem();

    static const std::string& GetRenderGroupLabel();
    virtual const std::string& GetLabel() const NOC_OVERRIDE;

    // Overrides
    virtual void Load() NOC_OVERRIDE;

    // Application event callbacks
  private:
    void RenderTargetAdded( const LightingJobMemberChangeArgs& args );
    void RenderTargetRemoved( const LightingJobMemberChangeArgs& args );
  };
}