#pragma once

#include "API.h"
#include "Inspect/Control.h"

namespace Inspect
{
  ///////////////////////////////////////////////////////////////////////////
  // A gradient color control that lets the user add keys of various color
  // values along its length.
  // 
  class INSPECTCONTENT_API ParametricKeyControl : public Reflect::ConcreteInheritor<ParametricKeyControl, Inspect::Control>
  {
  private:
    bool m_BlockRead;

  public:
    ParametricKeyControl();

    virtual void Realize( Inspect::Container* parent ) NOC_OVERRIDE;

    virtual void Read() NOC_OVERRIDE;
    virtual bool Write() NOC_OVERRIDE;

  protected:
    virtual void Create() NOC_OVERRIDE;
  };

  typedef Nocturnal::SmartPtr<ParametricKeyControl> ParametricKeyControlPtr;
}