#include "PatchOperations.h"

using namespace File;

// Used by File::Resolver CacheDB to represent an unknown patch event
static const char* s_UNKNOWN      = "UNKNOWN"; 

const char* File::PatchOperationStrings[PatchOperations::Count] =
{
  "INSERT",
  "UPDATE",
  "DELETE",
  "RESTORE",
  "BRANCH",
  "OBLITERATE",
};

/////////////////////////////////////////////////////////////////////////////
// Functions for translating PatchOperation into strings and back
PatchOperation File::GetPatchOperation( const std::string& operation )
{
  for ( int index = 0; index < PatchOperations::Count; ++index )
  {
    if ( operation == PatchOperationStrings[index] )
    {
      return (PatchOperation) index;
    }
  }

  return PatchOperations::Unknown;
}

std::string File::GetPatchOperationString( const PatchOperation operation )
{
  if ( operation >= 0 && operation < PatchOperations::Count )
  {
    return std::string( PatchOperationStrings[operation] );
  }
  
  return std::string( s_UNKNOWN );
}