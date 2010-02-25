#pragma once

#include "API.h"
#include "Button.h"

namespace Inspect
{
  ///////////////////////////////////////////////////////////////////////////
  // 
  // 
  class INSPECT_API Action : public Reflect::ConcreteInheritor<Action, Button>
  {
  protected:
    // Event for when the button is clicked
    ActionSignature::Event m_ActionEvent;

  public:
    Action();

    virtual bool Write();

    // Button click listeners
  public:
    void AddListener( const ActionSignature::Delegate& listener )
    {
      m_ActionEvent.Add( listener );
    }
    void RemoveListener( const ActionSignature::Delegate& listener )
    {
      m_ActionEvent.Remove( listener );
    }
  };

  typedef Nocturnal::SmartPtr<Action> ActionPtr;
}