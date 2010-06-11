#pragma once

#include "API.h"
#include "Control.h"
#include "Foundation/Automation/Event.h"

namespace Inspect
{
  //
  // Items control (base class for comboboxes and listboxes)
  //

  const static char ITEMS_ATTR_ITEM[]      = "item";
  const static char ITEMS_ATTR_ITEM_DELIM  = '|';
  const static char ITEMS_ATTR_REQUIRED[]  = "required";
  const static char ITEMS_ATTR_PREFIX[]    = "prefix";

  class INSPECT_API Items NOC_ABSTRACT : public Reflect::AbstractInheritor<Items, Control>
  {
  protected:
    std::string m_Value;
    std::string m_Prefix;
    V_Item m_Items;
    V_Item m_Statics;
    bool m_Required;
    bool m_Highlight;

  public:
    Items();

  protected:
    virtual bool Process(const std::string& key, const std::string& value) NOC_OVERRIDE;

    virtual void SetDefaultAppearance(bool def) NOC_OVERRIDE;

    void SetToDefault(const ContextMenuEventArgs& event);

  public:
    virtual void Realize(Container* parent) NOC_OVERRIDE;

    // clear all the items
    virtual void Clear();

    // add an entry to the control
    virtual const V_Item& GetItems() const;
    virtual void SetItems(const V_Item& items);

    // get the value of an entry data
    virtual std::string GetValue();
    virtual void SetValue(const std::string& data);

    // query item membership
    virtual bool Contains(const std::string& data);

    // set the highlight state
    virtual void SetHighlight(bool highlighted);
  };
}