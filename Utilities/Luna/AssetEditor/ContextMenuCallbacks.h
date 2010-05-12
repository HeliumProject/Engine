#pragma once

#include "Editor/ContextMenu.h"

namespace Luna
{
  void OnSaveSelectedAssets( const ContextMenuArgsPtr& args );
  void OnCheckOutSelectedAssets( const ContextMenuArgsPtr& args );
  void OnRevisionHistorySelectedAssets( const ContextMenuArgsPtr& args );
  void OnCloseSelectedAssets( const ContextMenuArgsPtr& args );
  void OnDuplicateAsset( const ContextMenuArgsPtr& args );
  void OnRenameAsset( const ContextMenuArgsPtr& args );
  void OnDeleteAsset( const ContextMenuArgsPtr& args );
}
