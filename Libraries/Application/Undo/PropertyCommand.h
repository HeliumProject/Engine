#pragma once

#include "Command.h"

#include "Foundation/Automation/Property.h"

namespace Undo
{
  //
  // Command to get/set property data
  //

  template <class V>
  class PropertyCommand : public Command
  {
  private:
    // the property object we will get/set through
    Nocturnal::SmartPtr< Nocturnal::Property<V> > m_Property;

    // the latent data value
    V m_Value;

    bool m_Significant; 

  public:
    PropertyCommand(const Nocturnal::SmartPtr< Nocturnal::Property<V> >& property)
      : m_Property (property)
      , m_Significant( true )
    {
      m_Value = m_Property->Get();
    }

    PropertyCommand(const Nocturnal::SmartPtr< Nocturnal::Property<V> >& property, const V& val)
      : m_Property (property)
      , m_Value (val)
      , m_Significant( true )
    {
      Swap();
    }

    void SetSignificant(bool significant)
    {
      m_Significant = significant; 
    }

    virtual bool IsSignificant() const
    {
      return m_Significant; 
    }
    
    virtual void Undo() NOC_OVERRIDE
    {
      Swap();
    }

    virtual void Redo() NOC_OVERRIDE
    {
      Swap();
    }

    void Swap()
    {
      // read the existing value
      V old = m_Property->Get();

      // set the stored value
      m_Property->Set(m_Value);

      // save the previous one
      m_Value = old;
    }
  };
}