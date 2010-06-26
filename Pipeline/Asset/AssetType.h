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
      info->AddElement(Null, TXT( "Null" ) );

      info->AddElement(AnimationSet, TXT( "AnimationSet" ) );
      info->AddElement(Cinematic, TXT( "Cinematic" ) );
      info->AddElement(CubeMap, TXT( "CubeMap" ) );
      info->AddElement(Entity, TXT( "Entity" ) );  
      info->AddElement(Font, TXT( "Font" ) );
      info->AddElement(Level, TXT( "Level" ) );
      info->AddElement(Movie, TXT( "Movie" ) );
      info->AddElement(Shader, TXT( "Shader" ) );
      info->AddElement(Sky, TXT( "Sky" ) );
      info->AddElement(TexturePack, TXT( "TexturePack" ) );
    }
  }
  typedef AssetTypes::AssetType AssetType;
}