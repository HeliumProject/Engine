#include "Precompile.h"
#include "RemoteViewer.h"

#include "Live/RuntimeConnection.h"
#include "rpc/interfaces/rpc_lunaview.h"
#include "rpc/interfaces/rpc_lunaview_host.h"
#include "Task/Build.h"

using namespace Luna;

RemoteViewer::RemoteViewer( wxWindow* win )
: RemoteEditor( NULL )
, m_Window( win )
{
}

RemoteViewer::~RemoteViewer()
{
}

void RemoteViewer::BuildAndViewAsset( Asset::AssetClass* assetClass, bool showOptions )
{
  if ( assetClass->GetEngineType() == Asset::EngineTypes::Moby )
  {
    SetMode( RPC::LunaViewModes::MobyView );
  }
  else
  {
    SetMode( RPC::LunaViewModes::Normal );
  }

  Luna::ViewAsset( assetClass->m_AssetClassID, m_Window, NULL, showOptions );

  // now control will pass to our build/task callbacks:
  // RemoteEditor::BuildFinished
  // RemoteEditor::AssetBuilt
}
