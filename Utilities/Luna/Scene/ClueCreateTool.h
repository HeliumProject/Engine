#pragma once

#include "API.h"
#include "Clue.h"
#include "CreateTool.h"

#include "TUID/TUID.h"

namespace Luna
{
  class ClueCreateTool : public Luna::CreateTool
  {
    //
    // RTTI
    //

    LUNA_DECLARE_TYPE(Luna::ClueCreateTool, Luna::CreateTool);
    static void InitializeType();
    static void CleanupType();

  public:
    ClueCreateTool(Luna::Scene* scene, Enumerator* enumerator);

    virtual ~ClueCreateTool();

    virtual Luna::TransformPtr CreateNode() NOC_OVERRIDE;
  };

  typedef Nocturnal::SmartPtr< Luna::Clue > CluePtr;
}
