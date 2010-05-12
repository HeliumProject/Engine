#pragma once

#include "Common/Compiler.h"

namespace Luna
{
  class AnimationEventsManager;

  template < class TPanel >
  class AnimationPanelBase NOC_ABSTRACT : public wxEvtHandler
  {
  public:
    AnimationPanelBase( AnimationEventsManager& manager, TPanel* panel )
      : m_Panel( panel )
      , m_Manager( manager )
    {
      m_Panel->PushEventHandler( this );
    }

    virtual ~AnimationPanelBase() 
    {
      m_Panel->PopEventHandler();

      delete m_Panel;
    }

    TPanel* GetPanel() { return m_Panel; }
    AnimationEventsManager& GetManager() { return m_Manager; }

  protected:
    TPanel* m_Panel;
    AnimationEventsManager& m_Manager;
  };
}