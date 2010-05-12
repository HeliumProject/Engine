#pragma once

#include "Reflect/Enumeration.h"

namespace Asset
{
  namespace EngineTypes
  {
    enum EngineType
    {
      Null = -1,

      // NOTE: Please keep this list sorted alphabetically, this is possible because
      //       EngineType is not currently being serialized in any Reflect files

      AnimationSet,     // set of animation clips
      Cinematic,        // cinematics
      CineScene,        // cinematics
      CubeMap,          // cubemap
      Foliage,          // speed-tree based asset
      Font,             // font
      Level,            // finished chunk of game
      LooseGraphShader, // loose graph shader
      Material,         // collision system material
      Moby,             // entity type
      Movie,            // movie
      Shader,           // geometric shader asset
      Shrub,            // entity type
      Sky,              // skydome asset
      TexturePack,      // texture pack asset
      Tie,              // entity type
      Ufrag,            // unique level geometry fragment
      WrinkleMap,       // wrinkle map is a special kind of texture pack

      Count,
    };

    static void EngineTypeEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(Null, "Null");

      info->AddElement(AnimationSet, "AnimationSet");
      info->AddElement(Cinematic, "Cinematic");
      info->AddElement(CineScene, "CineScene");
      info->AddElement(CubeMap, "CubeMap");
      info->AddElement(Foliage, "Foliage");  
      info->AddElement(Font, "Font");
      info->AddElement(Level, "Level");
      info->AddElement(LooseGraphShader, "LooseGraphShader");
      info->AddElement(Material, "Material");
      info->AddElement(Moby, "Moby");
      info->AddElement(Movie, "Movie");
      info->AddElement(Shader, "Shader");
      info->AddElement(Shrub, "Shrub");
      info->AddElement(Sky, "Sky");
      info->AddElement(TexturePack, "TexturePack");
      info->AddElement(Tie, "Tie");
      info->AddElement(Ufrag, "Ufrag");
      info->AddElement(WrinkleMap, "WrinkleMap");
    }
  }
  typedef EngineTypes::EngineType EngineType;
}