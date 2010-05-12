#pragma once

#include "Common/Compiler.h"

namespace Luna
{
  class CinematicEventsManager;

  template < class TPanel >
  class CinematicPanelBase NOC_ABSTRACT : public wxEvtHandler
  {
  public:
    CinematicPanelBase( CinematicEventsManager& manager, TPanel* panel )
      : m_Panel( panel )
      , m_Manager( manager )
    {
      m_Panel->PushEventHandler( this );
    }

    virtual ~CinematicPanelBase() 
    {
      m_Panel->PopEventHandler();

      delete m_Panel;
    }

    TPanel* GetPanel() { return m_Panel; }
    CinematicEventsManager& GetManager() { return m_Manager; }

  protected:
    TPanel* m_Panel;
    CinematicEventsManager& m_Manager;
  };
}