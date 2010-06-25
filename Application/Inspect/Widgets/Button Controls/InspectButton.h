#pragma once

#include "Application/API.h"
#include "Application/Inspect/Widgets/Control.h"

#include "Foundation/Reflect/Class.h"

namespace Inspect
{
  const static tchar BUTTON_ATTR_TEXT[] = TXT( "text" );

  ///////////////////////////////////////////////////////////////////////////
  // 
  // 
  class APPLICATION_API Button : public Reflect::ConcreteInheritor<Button, Control>
  {
  protected:
    // Label on the button
    tstring m_Text;

    // Icon for the button
    tstring m_Icon;

  public:
    Button();

  protected:
    virtual bool Process( const tstring& key, const tstring& value );

  public:
    virtual void Realize( Container* parent );
    virtual bool Write();

    virtual void SetText( const tstring& text );
    virtual void SetIcon( const tstring& icon );
  };

  typedef Nocturnal::SmartPtr<Button> ButtonPtr;
}