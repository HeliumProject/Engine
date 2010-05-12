#pragma once

#include "API.h"

#include "rpc/interfaces/rpc_lunaanimation.h"
#include "rpc/interfaces/rpc_lunaanimationevents_host.h"
#include "rpc/interfaces/rpc_lunaanimationevents_tool.h"

#include "AnimationEventsManager.h"

namespace Luna
{
  class AnimationEventsManager;

  namespace RemoteAnimationEvents
  {
    bool Initialize( AnimationEventsManager& manager );
    void Cleanup();

    void SetValidMoby( bool valid );
    void InitRemote();
    void UpdateEvents();
    void SetPlayRate( float rate );
    void PlayEvent( const std::string& eventType, const float value, const u32 value2, bool untrigger, bool infinite, bool dominantOnly, bool gameplay );
    void StopAll();

    void ClipChanged( const AnimClipChangeArgs& args );
    void FrameChanged( const FrameChangedArgs& args );
    void EventExistence( const EventExistenceArgs& args );
    void EventsChanged( const EventsChangedArgs& args );
    void AssetOpened( const MobyFileChangeArgs& args );
    void AssetClosed( const MobyFileChangeArgs& args );
    void UpdateClassChanged( const UpdateClassChangedArgs& args );
  }
}