#pragma once

namespace Luna
{
  namespace AnimationEventsEditorIDs
  {
    enum AnimationEventsEditorID
    {
      // Controls
      AnimClipList = wxID_HIGHEST + 1,
      TimelineSlider,
      TimelineMarkersPanel,

      // Menu items
      SearchForFile,
      SelectAll,

      // Commands
      COMMAND_BUILD_DONE,
      COMMAND_UPDATE_EVENT_LIST
    };
  }
  typedef AnimationEventsEditorIDs::AnimationEventsEditorID AnimationEventsEditorID;
}