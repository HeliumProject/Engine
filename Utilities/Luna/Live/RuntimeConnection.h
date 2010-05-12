#pragma once

#include "API.h"

#include "Common/Automation/Event.h"

// Forwards
namespace RPC
{
  struct Host;
  struct ILunaShaderHostRemote;
  struct ILunaViewHostRemote;
  struct ILunaAnimationEventsHostRemote;
  struct ILunaCinematicEventsHostRemote;
}

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // Event and arguments for PS3 connection status changes.
  // 
  struct RuntimeConnectionStatusArgs
  {
    bool m_Connected;

    RuntimeConnectionStatusArgs( bool connected )
      : m_Connected( connected )
    {}
  };
  typedef Nocturnal::Signature< void, const RuntimeConnectionStatusArgs& > RuntimeConnectionStatusSignature;

  /////////////////////////////////////////////////////////////////////////////
  // Manages connection to the PS3.
  // 
  namespace RuntimeConnection
  {
    void Initialize();
    void Cleanup();
    void Process();

    LUNA_LIVE_API bool IsConnected();
    LUNA_LIVE_API void ResetConnection();

    LUNA_LIVE_API RPC::Host* GetHost();
    LUNA_LIVE_API RPC::ILunaShaderHostRemote* GetRemoteShaderView();
    LUNA_LIVE_API RPC::ILunaViewHostRemote* GetRemoteLevelView();
    LUNA_LIVE_API RPC::ILunaAnimationEventsHostRemote* GetRemoteAnimationEventsView();
    LUNA_LIVE_API RPC::ILunaCinematicEventsHostRemote* GetRemoteCinematicEventsView();

    LUNA_LIVE_API bool IsViewerConnected( const std::string& viewerName );

    LUNA_LIVE_API void AddRuntimeConnectionStatusListener( const RuntimeConnectionStatusSignature::Delegate& listener );
    LUNA_LIVE_API void RemoveRuntimeConnectionStatusListener( const RuntimeConnectionStatusSignature::Delegate& listener );
  }
}
