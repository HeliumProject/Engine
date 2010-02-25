#pragma once

#include "API.h"
#include "Control.h"

namespace Inspect
{
  const static char TEXTBOX_ATTR_REQUIRED[] = "required";
  const static char TEXTBOX_ATTR_JUSTIFY[] = "justify";
  const static char TEXTBOX_ATTR_JUSTIFY_LEFT[] = "left";
  const static char TEXTBOX_ATTR_JUSTIFY_RIGHT[] = "right";

  class INSPECT_API Value : public Reflect::ConcreteInheritor<Value, Control>
  {
  public:
    enum Justify
    {
      kLeft,
      kRight,
    };

    Value();

  protected:
    virtual bool Process(const std::string& key, const std::string& value) NOC_OVERRIDE;

    virtual void SetDefaultAppearance(bool def) NOC_OVERRIDE;

    void SetToDefault(const ContextMenuEventArgs& event);

  public:
    virtual void Realize(Container* parent) NOC_OVERRIDE;

    virtual void Read() NOC_OVERRIDE;

    virtual bool Write() NOC_OVERRIDE;

    void GetText(std::string& text);
    void SetText(const std::string& text);

    Justify GetJustification() const;
    void SetJustification( Justify justification );

    void SetHighlight(bool highlighted);
    virtual void SetReadOnly(bool readOnly) NOC_OVERRIDE;

  private:
    void UpdateUI( const std::string& text );

  protected:
    bool m_Required;
    Justify m_Justify;
    std::string m_Text;
    bool m_Highlight;
  };

  typedef Nocturnal::SmartPtr<Value> ValuePtr;
}