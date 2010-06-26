#pragma once

#include "Foundation/Reflect/Class.h"

namespace Content
{
  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // ContentTypes - What stuff is/has been exported as
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////

  namespace ContentTypes
  {
    //******************** !!IMPORTANT!! ***********************
    //
    //        !!ELEMENT ORDER MUST BE PRESERVED!!
    //
    // The ContentTypes values are stored in the maya files,
    // Reordering the elements, or inserting into the middle
    // of the element list will break the data in existing 
    // Maya files.
    //
    // "Last Element" should always be: NumContentTypes
    //
    // The ContentTypes enum in these files must match up:
    //  * shared/tools/libraries/Content/ContentTypes.h
    //  * shared/tools/libraries/MayaUtils/Export.cpp
    //  * shared/tools/distro/maya/scripts/igExportNodes.mel
    //
    //******************** !!IMPORTANT!! ***********************
    enum ContentType
    {
      Null = -1,

      Default = 0,
      Geometry,
      Skeleton,
      Bangle,
      HighResCollision,
      LowResCollision,
      Pathfinding,
      LowResPathfinding,
      LightMapped,
      VertexLit,
      Overlay,
      PreShell,
      BloomPreShell,
      PostShell,
      BloomPostShell,
      Foliage,
      FragmentGroup,
      MonitorCam,
      Water,
      RisingWater,
      WrinkleMap,
      Exclude,
      Destruction,
      Debris,
      Glue,
      Pins,
      NavMeshHiRes,
      NavMeshLowRes,

      // Last Element
      NumContentTypes,
    };

    //******************** !!IMPORTANT!! ***********************
    //
    // If you are hitting any one of these asserts, it means
    // that you have altered the order of the ContentTypes enum
    // elements. Please read the !!IMPORTANT!! warning above 
    // and undo your changes:
    NOC_COMPILE_ASSERT(ContentTypes::Bangle == 3);
    NOC_COMPILE_ASSERT(ContentTypes::Pathfinding == 6);
    NOC_COMPILE_ASSERT(ContentTypes::Overlay == 10);
    NOC_COMPILE_ASSERT(ContentTypes::BloomPostShell == 14);
    NOC_COMPILE_ASSERT(ContentTypes::Water == 18);
    NOC_COMPILE_ASSERT(ContentTypes::Destruction == 22);
    //
    // If you are hitting this assert, it means that you have
    // changed the number of ContentTypes enum elements. If you
    // believe your changes were made in compliance with the
    // !!IMPORTANT!! warning above, please change the magic-number
    // below so that the asserts no longer triggers:
    NOC_COMPILE_ASSERT(28 == ContentTypes::NumContentTypes);
    //
    //******************** !!IMPORTANT!! ***********************

    static void ContentTypeEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(Null, TXT( "Null" ) );
      info->AddElement(Default, TXT( "Default" ) );
      info->AddElement(Geometry, TXT( "Geometry" ) );
      info->AddElement(Skeleton, TXT( "Skeleton" ) );
      info->AddElement(Bangle, TXT( "Bangle" ) );
      info->AddElement(HighResCollision, TXT( "HighResCollision" ) );
      info->AddElement(LowResCollision, TXT( "LowResCollision" ) );
      info->AddElement(Pathfinding, TXT( "Pathfinding" ) );
      info->AddElement(LowResPathfinding, TXT( "LowResPathfinding" ) );
      info->AddElement(LightMapped, TXT( "LightMapped" ) );
      info->AddElement(VertexLit, TXT( "VertexLit" ) );
      info->AddElement(Overlay, TXT( "Overlay" ) );
      info->AddElement(PreShell, TXT( "PreShell" ) );
      info->AddElement(BloomPreShell, TXT( "BloomPreShell" ) );
      info->AddElement(PostShell, TXT( "PostShell" ) );
      info->AddElement(BloomPostShell, TXT( "BloomPostShell" ) );
      info->AddElement(Foliage, TXT( "Foliage" ) );
      info->AddElement(FragmentGroup, TXT( "FragmentGroup" ) );
      info->AddElement(MonitorCam, TXT( "MonitorCam" ) );
      info->AddElement(Water, TXT( "Water" ) );
      info->AddElement(RisingWater, TXT( "RisingWater" ) );
      info->AddElement(WrinkleMap, TXT( "WrinkleMap" ) );
      info->AddElement(Destruction, TXT( "Destruction" ) );
      info->AddElement(Debris, TXT( "Debris" ) );
      info->AddElement(Glue, TXT( "Glue" ) );
      info->AddElement(Pins, TXT( "Pins" ) );
      info->AddElement(NavMeshHiRes, TXT( "NavMeshHiRes" ) );
      info->AddElement(NavMeshLowRes, TXT( "NavMeshLowRes" ) );
    }
  } // namespace ContentTypes

  typedef ContentTypes::ContentType ContentType;
}
