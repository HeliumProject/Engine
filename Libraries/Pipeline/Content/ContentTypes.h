#pragma once

#include "Reflect/Class.h"

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
      info->AddElement(Null, "Null");
      info->AddElement(Default, "Default");
      info->AddElement(Geometry, "Geometry");
      info->AddElement(Skeleton, "Skeleton");
      info->AddElement(Bangle, "Bangle");
      info->AddElement(HighResCollision, "HighResCollision");
      info->AddElement(LowResCollision, "LowResCollision");
      info->AddElement(Pathfinding, "Pathfinding");
      info->AddElement(LowResPathfinding, "LowResPathfinding");
      info->AddElement(LightMapped, "LightMapped");
      info->AddElement(VertexLit, "VertexLit");
      info->AddElement(Overlay, "Overlay");
      info->AddElement(PreShell, "PreShell");
      info->AddElement(BloomPreShell, "BloomPreShell");
      info->AddElement(PostShell, "PostShell");
      info->AddElement(BloomPostShell, "BloomPostShell");
      info->AddElement(Foliage, "Foliage");
      info->AddElement(FragmentGroup, "FragmentGroup");
      info->AddElement(MonitorCam, "MonitorCam");
      info->AddElement(Water, "Water");
      info->AddElement(RisingWater, "RisingWater");
      info->AddElement(WrinkleMap, "WrinkleMap");
      info->AddElement(Destruction, "Destruction");
      info->AddElement(Debris, "Debris");
      info->AddElement(Glue, "Glue");
      info->AddElement(Pins, "Pins");
      info->AddElement(NavMeshHiRes, "NavMeshHiRes");
      info->AddElement(NavMeshLowRes, "NavMeshLowRes");
    }
  } // namespace ContentTypes

  typedef ContentTypes::ContentType ContentType;
}
