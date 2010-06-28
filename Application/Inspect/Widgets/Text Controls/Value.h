#pragma once

#include "Application/API.h"
#include "Application/Inspect/Widgets/Control.h"

#include "Foundation/Reflect/Class.h"

namespace Inspect
{
  const static tchar TEXTBOX_ATTR_REQUIRED[] = TXT( "required" );
  const static tchar TEXTBOX_ATTR_JUSTIFY[] = TXT( "justify" );
  const static tchar TEXTBOX_ATTR_JUSTIFY_LEFT[] = TXT( "left" );
  const static tchar TEXTBOX_ATTR_JUSTIFY_RIGHT[] = TXT( "right" );

  class APPLICATION_API Value : public Reflect::ConcreteInheritor<Value, Control>
  {
  public:
    enum Justify
    {
      kLeft,
      kRight,
    };

    Value();

  protected:
    virtual bool Process(const tstring& key, const tstring& value) NOC_OVERRIDE;

    virtual void SetDefaultAppearance(bool def) NOC_OVERRIDE;

    void SetToDefault(const ContextMenuEventArgs& event);

  public:
    virtual void Realize(Container* parent) NOC_OVERRIDE;

    virtual void Read() NOC_OVERRIDE;

    virtual bool Write() NOC_OVERRIDE;

    void GetText(tstring& text);
    void SetText(const tstring& text);

    Justify GetJustification() const;
    void SetJustification( Justify justification );

    void SetHighlight(bool highlighted);
    virtual void SetReadOnly(bool readOnly) NOC_OVERRIDE;

  private:
    void UpdateUI( const tstring& text );

  protected:
    bool m_Required;
    Justify m_Justify;
    tstring m_Text;
    bool m_Highlight;
  };

  typedef Nocturnal::SmartPtr<Value> ValuePtr;
}