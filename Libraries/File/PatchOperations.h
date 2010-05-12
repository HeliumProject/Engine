#pragma once

#include "API.h"
#include "Exceptions.h"

#include "Common/Types.h"
#include "Common/Memory/SmartPtr.h"

#include "FileSystem/File.h"

#include "Reflect/Registry.h"
#include "Reflect/Serializers.h"

#include "TUID/TUID.h"

namespace File
{ 
  /////////////////////////////////////////////////////////////////////////////
  // used to write out the patch operation in the event 
  namespace PatchOperations
  {
    enum PatchOperation
    {
      Unknown = -1,    // Used by File::Resolver CacheDB to represent an unknown patch event

      // NOTE: New operations must be added to the END of this list or
      //       current event files will not be parsed correctly, causing a 
      //       BadPatchOpException to occur in the File::Resolver
      Insert =  0,
      Update,
      Delete,
      Restore,
      Branch,
      Obliterate,      // Used internal to File::Resolver to mark file obliterated in the history table

      // Last Element
      Count
    };
  }
  FILE_API typedef int PatchOperation;

  extern FILE_API const char* PatchOperationStrings[File::PatchOperations::Count];

  FILE_API PatchOperation GetPatchOperation( const std::string& operation );
  FILE_API std::string    GetPatchOperationString( const PatchOperation operation );
}