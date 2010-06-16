#pragma once

#include "Application/API.h"
#include "Application/Inspect/Widgets/Control.h"

#include "Reflect/Class.h"

namespace Inspect
{
  const static char LABEL_ATTR_TEXT[] = "text";

  class APPLICATION_API Label : public Reflect::ConcreteInheritor<Label, Control>
  {
  private:
    // True if truncated labels should automatically have their full
    // string set as the tooltip.  False to leave the tooltip alone.
    bool m_AutoToolTip; 

  public:
    Label();

  protected:
    virtual bool Process(const std::string& key, const std::string& value);

  public:
    virtual void Realize(Container* parent);

    virtual void Read();

    void SetText( const std::string& text );
    std::string GetText() const;

    void SetAutoToolTip( bool enable );
    virtual void SetToolTip( const std::string& toolTip ) NOC_OVERRIDE;
    virtual bool TrimString(std::string& str, int width) NOC_OVERRIDE;

  private:
    void UpdateUI( const std::string& text );
  };

  typedef Nocturnal::SmartPtr<Label> LabelPtr;
}