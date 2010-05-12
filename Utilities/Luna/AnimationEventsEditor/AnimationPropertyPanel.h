#pragma once

#include "AnimationPanelBase.h"
#include "AnimationEventsManager.h"
#include "Inspect/Canvas.h"
#include "InspectReflect/ReflectInterpreter.h"

#include "Asset/AnimationClip.h"

namespace Luna
{
  class AnimationEventsPropertyPanel : public AnimationPanelBase< wxPanel >
  {
  public:
    AnimationEventsPropertyPanel( AnimationEventsManager& manager, wxPanel* panel );
    ~AnimationEventsPropertyPanel();

    void EnableAll();
    void DisableAll();

    void UpdatePanel();

    void ClipChanged( const AnimClipChangeArgs& args );
    void FrameChanged( const FrameChangedArgs& args );
    void EventExistence( const EventExistenceArgs& args );
    void SelectionChanged( const EventSelectionChangedArgs& args );
    void EventsChanged( const EventsChangedArgs& args );
    void UpdateClassChanged( const UpdateClassChangedArgs& args );
    void EventTypesChanged( const EventTypesChangedArgs& args );
    void AssetOpened( const MobyFileChangeArgs& args );
    void AssetClosed( const MobyFileChangeArgs& args );

    void PropertyChanged( const Inspect::ChangeArgs& args );

    void OnChar( wxKeyEvent& e );
    void OnUpdateEventList( wxCommandEvent& event );

  private:

    Inspect::CanvasWindow* m_CanvasWindow;
    Inspect::Canvas m_Canvas;
    Inspect::ReflectInterpreterPtr m_Interpreter;

    bool m_Frozen;

    DECLARE_EVENT_TABLE();
  };
}