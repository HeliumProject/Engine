#pragma once

#include "Application/API.h"
#include "Application/Inspect/Widgets/Control.h"

#include "Foundation/Reflect/Class.h"

namespace Inspect
{
  const static tchar LABEL_ATTR_TEXT[] = TXT( "text" );

  class APPLICATION_API Label : public Reflect::ConcreteInheritor<Label, Control>
  {
  private:
    // True if truncated labels should automatically have their full
    // string set as the tooltip.  False to leave the tooltip alone.
    bool m_AutoToolTip; 

  public:
    Label();

  protected:
    virtual bool Process(const tstring& key, const tstring& value);

  public:
    virtual void Realize(Container* parent);

    virtual void Read();

    void SetText( const tstring& text );
    tstring GetText() const;

    void SetAutoToolTip( bool enable );
    virtual void SetToolTip( const tstring& toolTip ) NOC_OVERRIDE;
    virtual bool TrimString(tstring& str, int width) NOC_OVERRIDE;

  private:
    void UpdateUI( const tstring& text );
  };

  typedef Nocturnal::SmartPtr<Label> LabelPtr;
}