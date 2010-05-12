#pragma once

#include "API.h"
#include "Control.h"

namespace Inspect
{
  const static char BUTTON_ATTR_TEXT[] = "text";

  ///////////////////////////////////////////////////////////////////////////
  // 
  // 
  class INSPECT_API Button : public Reflect::ConcreteInheritor<Button, Control>
  {
  protected:
    // Label on the button
    std::string m_Text;

    // Icon for the button
    std::string m_Icon;

  public:
    Button();

  protected:
    virtual bool Process( const std::string& key, const std::string& value );

  public:
    virtual void Realize( Container* parent );
    virtual bool Write();

    virtual void SetText( const std::string& text );
    virtual void SetIcon( const std::string& icon );
  };

  typedef Nocturnal::SmartPtr<Button> ButtonPtr;
}