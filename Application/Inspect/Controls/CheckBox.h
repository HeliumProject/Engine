#pragma once

#include "Application/API.h"
#include "Application/Inspect/Controls/Control.h"

#include "Foundation/Reflect/Class.h"

namespace Inspect
{
  class APPLICATION_API CheckBox : public Reflect::ConcreteInheritor<CheckBox, Control>
  {
  private:
    bool m_State;

  public:
    CheckBox();

    virtual void Realize(Container* parent);
    virtual void Read();
    virtual bool Write();
    virtual bool GetChecked();
    virtual void SetChecked(bool checked);

  private:
    virtual void SetDefaultAppearance( bool def ) HELIUM_OVERRIDE;
    void UpdateUI( bool state );
  };

  typedef Helium::SmartPtr<CheckBox> CheckBoxPtr;
}