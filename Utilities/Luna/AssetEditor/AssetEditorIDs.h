#pragma once

namespace Luna
{
  namespace AssetEditorIDs
  {
    enum AssetEditorID
    {
      // Controls
      AssetOutlinerTreeControl = wxID_HIGHEST + 1,
      // Insert new control IDs here

      // Menu items
      SaveAllAssetClasses,
      AddAttribute,
      DeleteAttribute,
      Back,
      Forward,
      Refresh,
      BrowseSelection,
      PrintHTML,
      ChangeAttributeView,
      AttributeHelp,
      SearchForFile,
      SortOpenFiles,
      Checkout,
      AddShaderGroup,
      DeleteShaderGroup,
      Build,
      View,
      Export,
      SyncShaders,
      AddAnimationSet,
      AddAnimationGroup,
      EditAnimationGroup,
      AddAnimationClip,
      AddAnimationClipToNewChain,
      AddAnimationClipToExistingChain,
      UpdateSymbols,
      SaveSession,
      SaveSessionAs,
      OpenSession,
      MoveUp,
      MoveDown,
      Preview,
      ExpandAll,
      CollapseAll,
      // Insert new menu IDs here (unless they overlap with wxWidget IDs, see below)

      // Menu items that overlap with predefined wxWidget IDs
      NewAssetClass = wxID_NEW,
      OpenAssetClass = wxID_OPEN,
      SaveAssetClass = wxID_SAVE,
      CloseAssetClass = wxID_CLOSE,
      Cut = wxID_CUT,
      Copy = wxID_COPY,
      Paste = wxID_PASTE,
      Undo = wxID_UNDO,
      Redo = wxID_REDO
    };
  }
  typedef AssetEditorIDs::AssetEditorID AssetEditorID;
}
