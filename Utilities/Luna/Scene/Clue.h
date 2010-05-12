#pragma once

#include "Volume.h"

#include "Content/Clue.h"

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // Luna's wrapper for an volume instance.
  // 
  class Clue : public Luna::Volume
  {
    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::Clue, Luna::Volume );
    static void InitializeType();
    static void CleanupType();


    //
    // Member functions
    //

    Clue(Luna::Scene* s);
    Clue(Luna::Scene* s, Content::Clue* volume);
    virtual ~Clue();

    virtual i32 GetImageIndex() const NOC_OVERRIDE;
    virtual std::string GetApplicationTypeName() const NOC_OVERRIDE;
  };

  typedef Nocturnal::SmartPtr<Luna::Clue> LCluePtr;
}
