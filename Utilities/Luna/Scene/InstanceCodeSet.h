#pragma once

#include "InstanceSet.h"

namespace Luna
{
  class Instance;
  class InstanceType;

  class InstanceCodeSet : public Luna::InstanceSet
  {
    //
    // Members
    //

  protected:
    std::string m_Name;


    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::InstanceCodeSet, Luna::InstanceSet );
    static void InitializeType();
    static void CleanupType();


    //
    // Implementation
    //

    InstanceCodeSet(Luna::InstanceType* type, const std::string& classStr);

    virtual const std::string& GetName() const NOC_OVERRIDE;

    virtual void AddInstance(Luna::Instance* i) NOC_OVERRIDE;

    virtual void RemoveInstance(Luna::Instance* i) NOC_OVERRIDE;
  };
}
