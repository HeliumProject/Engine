#pragma once

#include "Application/API.h"
#include "Application/Inspect/Widgets/Control.h"

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
    std::string m_BitfieldString;

  public:
    ReflectBitfieldCheckBox();

    virtual void Realize( Container* parent ) NOC_OVERRIDE;
    virtual void Read() NOC_OVERRIDE;
    virtual bool Write() NOC_OVERRIDE;
    virtual CheckBoxState GetChecked();
    virtual void SetChecked( CheckBoxState checked );
    void SetBitfieldString( const std::string& value );
    virtual bool IsDefault() const NOC_OVERRIDE;

  protected:
    virtual void SetDefaultAppearance( bool def ) NOC_OVERRIDE;
    bool WriteBitfield();
    std::string BuildBitfieldString( std::vector< std::string > tokens, const std::string& delimiter );
    void UpdateUI( CheckBoxStates::CheckBoxState state );
    CheckBoxState GetUIState() const;
  };

  typedef Nocturnal::SmartPtr<ReflectBitfieldCheckBox> BitfieldCheckBoxPtr;
}