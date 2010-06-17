#pragma once

namespace Component
{
  namespace ComponentCategoryTypes
  {
    enum ComponentCategoryType
    {
      Lighting,
      Animation,
      Cinematic,
      File,
      LOD,
      Gameplay,
      Meta,
      Font,
      Misc,
      Configuration,
      Joint,
      PostEffects
    };
  }
  typedef ComponentCategoryTypes::ComponentCategoryType ComponentCategoryType;
}
