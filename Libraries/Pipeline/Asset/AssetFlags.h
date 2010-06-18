#pragma once

#include "Foundation/Reflect/Field.h"

namespace Asset
{
  namespace AssetFlags
  {
    enum AssetFlag
    {
      RealTimeUpdateable  = Reflect::FieldFlags::Last << 1, // Member is availabe while connected to devkit
      Hierarchy           = Reflect::FieldFlags::Last << 2, // Member should be shown hierarchically in the UI 

      // Flags used by the AssetManager ConfigOptions
      AssetManager        = Reflect::FieldFlags::Last << 3,   // this AssetClass or Attribute is managed by the asset manager Delete/Duplicate/Rename operations

      ManageField         = AssetManager << 0,                // this AssetClass or Attribute or Field is managed by the asset manager Delete/Duplicate/Rename operations

      CreateSubFolder     = AssetManager << 1,                // Place the asset in a subfolder of the same name
      CannotOverrideCSF   = AssetManager << 2,                //  - user can NOC_OVERRIDE and choose not to place the asset in a subfolder

      PerformOperation    = AssetManager << 3,                // Duplicate/Rename/Delete this file by default
      CannotOverridePO    = AssetManager << 4,                //  - user can over ride the default value for perform op

      EnsureParentDir     = AssetManager << 5,                // Ensure that assets are not placed directly in a root path (x:/rcf/assets/devel/entities/<ASSET>)
      CannotOverrideEPD   = AssetManager << 6,                


      Last = CannotOverrideEPD,
    };
  }
  typedef AssetFlags::AssetFlag AssetFlag;


  //
  // Key strings for common asset attributes, classes, and field properties
  //
    
  namespace AssetProperties
  {
    static const char* ShortDescription  = "ShortDescription";
    static const char* LongDescription   = "ShortDescription";
    static const char* SmallIcon         = "SmallIcon";
    static const char* RootFolderSpec    = "RootFolderSpec";
    static const char* FilterSpec        = "FilterSpec";
    static const char* ModifierSpec      = "ModifierSpec";
    static const char* AssetTemplates    = "AssetTemplates";
  }
}