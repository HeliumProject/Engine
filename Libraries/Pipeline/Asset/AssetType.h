#pragma once

#include "Foundation/Reflect/Enumeration.h"

namespace Asset
{
  namespace AssetTypes
  {
    enum AssetType
    {
      Null = -1,

      AnimationSet,
      Cinematic,   
      CubeMap,     
      Entity,      
      Font,        
      Level,       
      Movie,       
      Shader,      
      Sky,         
      TexturePack, 

      Count,
    };

    static void AssetTypeEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(Null, "Null");

      info->AddElement(AnimationSet, "AnimationSet");
      info->AddElement(Cinematic, "Cinematic");
      info->AddElement(CubeMap, "CubeMap");
      info->AddElement(Entity, "Entity");  
      info->AddElement(Font, "Font");
      info->AddElement(Level, "Level");
      info->AddElement(Movie, "Movie");
      info->AddElement(Shader, "Shader");
      info->AddElement(Sky, "Sky");
      info->AddElement(TexturePack, "TexturePack");
    }
  }
  typedef AssetTypes::AssetType AssetType;
}