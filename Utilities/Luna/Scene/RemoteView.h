#pragma once

#include "API.h"

#include "../../libraries/rpc/interfaces/rpc_lunaview.h"
#include "../../libraries/rpc/interfaces/rpc_lunaview_host.h"
#include "../../libraries/rpc/interfaces/rpc_lunaview_tool.h"

#include "math/Matrix4.h"

namespace Luna
{
  class Transform;
  class SceneEditor;
  class Light;
  class LightingVolume;
  class Entity;
  struct NodeChangeArgs;
  struct LunaViewTool;

  namespace RemoteView
  {
    // the interface to the remote host
    extern RPC::ILunaViewHost* g_LunaViewHost;

    // the interface to our remote view
    extern RPC::ILunaViewTool* g_LunaViewTool;

    void Initialize();
    void Cleanup();

    void Enable( bool enable );
    void EnableCamera( bool enable );
    void TransformInstance( Luna::Transform* transform );
    void TransformCamera( const Math::Matrix4& transform );

    void AddLight( Luna::Light* light );
    void RemoveLight( Luna::Light* light );
    void SetLightParams( Luna::Light* light );
    void SetLightAnimation( Luna::Light* light );

    void TweakLightmapSettings( Luna::Entity* entity, u32 lm_set_index );
    void TweakCubemapSettings( Luna::Entity* entity );

    void SceneNodeDeleted( const NodeChangeArgs& args );
    void SceneNodeCreated( const NodeChangeArgs& args );

    void SetLightingVolumeParams( Luna::LightingVolume* volume );
    void AddLightingVolume( Luna::LightingVolume* volume );
    void RemoveLightingVolume( Luna::LightingVolume* volume );
  }
}
