#pragma once

#include "Application/API.h"
#include "Application/Inspect/Controls/Control.h"

namespace Inspect
{
  ///////////////////////////////////////////////////////////////////////////
  // A gradient color control that lets the user add keys of various color
  // values along its length.
  // 
  class APPLICATION_API ParametricKeyControl : public Reflect::ConcreteInheritor<ParametricKeyControl, Inspect::Control>
  {
  private:
    bool m_BlockRead;

  public:
    ParametricKeyControl();

    virtual void Realize( Inspect::Container* parent ) HELIUM_OVERRIDE;

    virtual void Read() HELIUM_OVERRIDE;
    virtual bool Write() HELIUM_OVERRIDE;

  protected:
    virtual void Create() HELIUM_OVERRIDE;
  };

  typedef Helium::SmartPtr<ParametricKeyControl> ParametricKeyControlPtr;
}