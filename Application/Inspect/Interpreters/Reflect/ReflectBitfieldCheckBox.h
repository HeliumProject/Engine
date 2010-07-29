#pragma once

#include "Application/API.h"
#include "Application/Inspect/Controls/Control.h"

namespace Inspect
{
  namespace CheckBoxStates
  {
    enum CheckBoxState
    {
      Checked,
      Unchecked,
      Tristate
    };
  }
  typedef CheckBoxStates::CheckBoxState CheckBoxState;

  class APPLICATION_API ReflectBitfieldCheckBox : public Reflect::ConcreteInheritor<ReflectBitfieldCheckBox, Control>
  {
  private:
    CheckBoxState m_State;
    tstring m_BitfieldString;

  public:
    ReflectBitfieldCheckBox();

    virtual void Realize( Container* parent ) HELIUM_OVERRIDE;
    virtual void Read() HELIUM_OVERRIDE;
    virtual bool Write() HELIUM_OVERRIDE;
    virtual CheckBoxState GetChecked();
    virtual void SetChecked( CheckBoxState checked );
    void SetBitfieldString( const tstring& value );
    virtual bool IsDefault() const HELIUM_OVERRIDE;

  protected:
    virtual void SetDefaultAppearance( bool def ) HELIUM_OVERRIDE;
    bool WriteBitfield();
    tstring BuildBitfieldString( std::vector< tstring > tokens, const tstring& delimiter );
    void UpdateUI( CheckBoxStates::CheckBoxState state );
    CheckBoxState GetUIState() const;
  };

  typedef Helium::SmartPtr<ReflectBitfieldCheckBox> BitfieldCheckBoxPtr;
}