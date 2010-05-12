#pragma once

namespace Attribute
{
  namespace AttributeCategoryTypes
  {
    enum AttributeCategoryType
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
  typedef AttributeCategoryTypes::AttributeCategoryType AttributeCategoryType;
}
