#pragma once

#include "API.h"

#include "rpc/interfaces/rpc_lunacinematic.h"
#include "rpc/interfaces/rpc_lunacinematicevents_host.h"
#include "rpc/interfaces/rpc_lunacinematicevents_tool.h"

#include "CinematicEventsManager.h"

namespace Luna
{
  class CinematicEventsManager;

  namespace RemoteCinematicEvents
  {
    bool Initialize( CinematicEventsManager& manager );
    void Cleanup();

    void SetValidCinematic( bool valid );
    void InitRemote();
    void UpdateEvents();
    void SetPlayRate( float rate );
    void PlayEvent( const std::string& eventType, const float value, bool untrigger, bool infinite, bool dominantOnly, bool gameplay );
    void StopAll();

    void ClipChanged( const CinematicChangeArgs& args );
    void FrameChanged( const FrameChangedArgs& args );
    void EventExistence( const EventExistenceArgs& args );
    void EventsChanged( const EventsChangedArgs& args );
    void AssetOpened( const CinematicFileChangeArgs& args );
    void AssetClosed( const CinematicFileChangeArgs& args );
    void CinematicLoaded( const CinematicLoadedArgs& args );
  }
}