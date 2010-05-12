#pragma once

namespace ShaderProcess
{
  // Available texture channels on a shader.
  // NOTE: The order matters.  This order is expected to be the same
  // as the textures are declared on a PS3 shader (see IGPS3::Shader
  // for example).  If you have a new type of map, you can add it here
  // and specify which one of the existing channels the map should
  // be found in when built.
  namespace TextureSlots
  {
    enum TextureSlot
    {
      ColorMap = 0,
      
      NormalMap = 1,                          
      
      ExpensiveMap = 2,
      AnisotropicExpensiveMap = ExpensiveMap,
      FurControl = ExpensiveMap,
      Foliage = ExpensiveMap,

      DetailMap = 3,
      GrainMap = DetailMap,
      FoliageShadow = DetailMap,
      BRDFMap = DetailMap,

      CustomMapA = 4,
      CustomMapB = 5,
      NumTextureSlots  // Not a vaild channel - provided as a count
    };
  }
  typedef TextureSlots::TextureSlot TextureSlot;
}
