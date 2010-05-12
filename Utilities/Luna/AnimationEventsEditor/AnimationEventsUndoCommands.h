#pragma once

#include "Undo/Command.h"
#include "Asset/AnimationClip.h"

namespace Luna
{
  class AnimationEventsManager;

  class AnimationEventsChangedCommand : public Undo::Command
  {
  public:
    AnimationEventsChangedCommand( AnimationEventsManager& manager );

    virtual void Undo();
    virtual void Redo();

    void UpdateValues();
    void Swap();

  public:
    Asset::AnimationClipDataPtr   m_Clip;
    Asset::OS_AnimationEvent      m_Events;
    Asset::OS_AnimationEvent      m_Selection;
    int                           m_Frame;
    AnimationEventsManager&       m_Manager;
  };
}