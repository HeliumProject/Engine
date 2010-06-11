#pragma once

#include "API.h"
#include "Items.h"
#include "Foundation/Automation/Event.h"

namespace Inspect
{
  //
  // Event delegates
  //

  class INSPECT_API Choice;

  // the delegate for populating items
  struct ChoiceArgs
  {
    Choice* m_Choice;

    ChoiceArgs(Choice* choice)
      : m_Choice (choice)
    {

    }
  };
  typedef Nocturnal::Signature<void, const ChoiceArgs&> ChoiceSignature;

  // the delegate for connecting an enumerated type's values
  struct ChoiceEnumerateArgs : public ChoiceArgs
  {
    std::string m_Enumeration;

    ChoiceEnumerateArgs(Choice* choice, const std::string& enumeration)
      : ChoiceArgs (choice)
      , m_Enumeration (enumeration)
    {

    }
  };
  typedef Nocturnal::Signature<void, const ChoiceEnumerateArgs&> ChoiceEnumerateSignature;


  //
  // Choice control (base class for comboboxes and listboxes)
  //

  const static char CHOICE_ATTR_ENUM[]      = "enum";
  const static char CHOICE_ATTR_SORTED[]    = "sorted";
  const static char CHOICE_ATTR_DROPDOWN[]  = "dropdown";

  class INSPECT_API Choice : public Reflect::ConcreteInheritor<Choice, Items>
  {
  protected:
    std::string m_Enum;
    bool m_Sorted;
    bool m_DropDown;
    bool m_EnableAdds; 

  public:
    Choice();

  protected:
    virtual bool Process(const std::string& key, const std::string& value) NOC_OVERRIDE;

    virtual void SetOverride(bool isOverride);

    virtual void SetDefaultAppearance(bool def) NOC_OVERRIDE;

    void SetToDefault(const ContextMenuEventArgs& event);

  public:
    virtual void SetSorted(bool sorted);
    virtual void SetDropDown(bool dropDown);
    virtual void SetEnableAdds(bool enabled); 

    virtual std::string GetValue() NOC_OVERRIDE;
    virtual void SetValue(const std::string& data) NOC_OVERRIDE;

    virtual void Realize(Container* parent) NOC_OVERRIDE;
      
    virtual void Populate() NOC_OVERRIDE;

    virtual void Read() NOC_OVERRIDE;

    virtual bool Write() NOC_OVERRIDE;

  protected:
    ChoiceSignature::Event m_Populate;
  public:
    void AddPopulateListener(const ChoiceSignature::Delegate& listener)
    {
      m_Populate.Add(listener);
    }
    void RemovePopulateListener(const ChoiceSignature::Delegate& listener)
    {
      m_Populate.Remove(listener);
    }

  protected:
    ChoiceEnumerateSignature::Event m_Enumerate;
  public:
    void AddEnumerateListener(const ChoiceEnumerateSignature::Delegate& listener)
    {
      m_Enumerate.Add(listener);
    }
    void RemoveEnumerateListener(const ChoiceEnumerateSignature::Delegate& listener)
    {
      m_Enumerate.Remove(listener);
    }
  };

  typedef Nocturnal::SmartPtr<Choice> ChoicePtr;
}
