#pragma once

#include "CinematicPanelBase.h"
#include "CinematicEventsManager.h"
#include "Inspect/Canvas.h"
#include "Inspect/Panel.h"
#include "InspectReflect/ReflectInterpreter.h"
#include "InspectSymbol/SymbolInterpreter.h"

#include "Asset/AnimationClip.h"

namespace Luna
{
  class CinematicPropertyPanel : public CinematicPanelBase< wxPanel >
  {
  public:
    CinematicPropertyPanel( CinematicEventsManager& manager, wxPanel* panel );
    ~CinematicPropertyPanel();

    void EnableAll();
    void DisableAll();

    void CreateRuntimeData();
    void UpdatePanel();

    void ClipChanged( const CinematicChangeArgs& args );
    void FrameChanged( const FrameChangedArgs& args );
    void EventExistence( const EventExistenceArgs& args );
    void SelectionChanged( const EventSelectionChangedArgs& args );
    void EventsChanged( const EventsChangedArgs& args );
    void CinematicLoaded( const CinematicLoadedArgs& args );
    void EventTypesChanged( const EventTypesChangedArgs& args );
    void AssetOpened( const CinematicFileChangeArgs& args );
    void AssetClosed( const CinematicFileChangeArgs& args );

    void RefreshInstances( bool layout );
    bool RectifyAndBuildClasses( S_string& classList );
    bool RefreshClasses();
    void RecurseAddDerivedClasses( const std::string& baseClass, S_string& classList );

    void PropertyChanged( const Inspect::ChangeArgs& args );

    void OnChar( wxKeyEvent& e );
    void OnUpdateEventList( wxCommandEvent& event );

    std::string GetRuntimeClassName() const;
    void SetRuntimeClassName( const std::string& className );
    void OnRuntimeClassChanged( const Inspect::ChangeArgs& args );
    void OnRuntimeClassButton( Inspect::Button* button );

  private:

    Inspect::CanvasWindow* m_CanvasWindow;
    Inspect::Canvas m_Canvas;
    Inspect::ReflectInterpreterPtr m_Interpreter;
    Inspect::Action* m_RuntimeClassesButton;
    Inspect::Choice* m_RuntimeClassesChoice;
    Inspect::Panel* m_RuntimeDataPanel;
    Asset::OS_CinematicEvent m_Selection;
    Inspect::SymbolInterpreterPtr m_SymbolInterpreter;


    bool m_Frozen;

    DECLARE_EVENT_TABLE();
  };
}