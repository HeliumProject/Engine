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
#pragma TODO("This is terrible and needs to be fixed")
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
