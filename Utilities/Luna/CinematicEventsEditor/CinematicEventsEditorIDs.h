#pragma once

namespace Luna
{
  namespace CinematicEventsEditorIDs
  {
    enum CinematicEventsEditorID
    {
      // Controls
      CinematicList = wxID_HIGHEST + 1,
      TimelineSlider,
      TimelineMarkersPanel,

      // Menu items
      SaveAllAssetClasses,
      Refresh,
      SearchForFile,
      Checkout,
      SelectAll,
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
      Redo = wxID_REDO,

      // commands
      COMMAND_BUILD_DONE,
      COMMAND_UPDATE_EVENT_LIST
    };
  }
  typedef CinematicEventsEditorIDs::CinematicEventsEditorID CinematicEventsEditorID;
}