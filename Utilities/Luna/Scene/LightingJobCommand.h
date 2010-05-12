#pragma once

#include "Undo/Command.h"
#include "UniqueID/TUID.h"
#include "TUID/TUID.h"

namespace Luna
{
  // Forwards
  class LightingJob;
  typedef Nocturnal::SmartPtr< Luna::LightingJob > LightingJobPtr;

  class Scene;
  class SceneNode;
  typedef Nocturnal::SmartPtr< Luna::SceneNode > SceneNodePtr;

  // Add or remove an item from a lighting job
  namespace LightingJobCommandActions
  {
    enum LightingJobCommandAction
    {
      Add,
      Remove
    };
  }
  typedef LightingJobCommandActions::LightingJobCommandAction LightingJobCommandAction;

  // What group the item is/is going to be part of
  namespace LightingJobCommandGroups
  {
    enum LightingJobCommandGroup
    {
      Light,
      Probe,
      Render,
      Shadow,
      LightingEnvironment,
      Zone
    };
  }
  typedef LightingJobCommandGroups::LightingJobCommandGroup LightingJobCommandGroup;

  /////////////////////////////////////////////////////////////////////////////
  // Command for adding and removing objects to/from a lighting job.
  // 
  class LightingJobCommand : public Undo::Command
  {
  private:
    LightingJobCommandAction m_Action;
    LightingJobCommandGroup m_Group;
    Luna::LightingJob* m_LightingJob;
    UniqueID::TUID m_NodeID;
    tuid m_ZoneID;

  public:
    LightingJobCommand( LightingJobCommandAction action, LightingJobCommandGroup group, Luna::LightingJob* job, const SceneNodePtr& node, bool execute );
    virtual ~LightingJobCommand();
    virtual void Undo() NOC_OVERRIDE;
    virtual void Redo() NOC_OVERRIDE;

  private:
    void Add();
    void Remove();
    Luna::Scene* Verify();
  };
}
