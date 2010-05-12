#pragma once

#include "TUID/TUID.h"
#include "Common/Compiler.h"
#include "Common/Exception.h"

#include "RCS/RCS.h"

#include <sstream>

namespace File
{
  DEFINE_EXCEPTION_CLASS
  DEFINE_INIT_EXCEPTIONS_CLASSES

  class BadPatchRecordException : public Exception
  {
  public:
    BadPatchRecordException() : Exception( "Could not parse invalid patch record!" ) {}
    BadPatchRecordException( const char* patchRecord ) : Exception( "Could not parse invalid patch record: %s", patchRecord ) {}
  };

  class BadPatchOpException : public Exception
  {
  public:
    BadPatchOpException() : Exception( "Could not apply invalid patch record operation!" ) {}
    BadPatchOpException( const char* patchRecord ) : Exception( "Could not apply invalid patch record: %s", patchRecord ) {}
  };

  class DuplicateEntryException : public Exception
  {
  public:
    DuplicateEntryException( const tuid id, const char* path ) : Exception( "Duplicate entry found for file path: %s", path ) { mExistingEntryId = id; }
    DuplicateEntryException( const tuid id, const char* path, const char* exMsgFormat ) : Exception( exMsgFormat, path ) { mExistingEntryId = id; }

    virtual tuid GetExistingID() const throw()       { return mExistingEntryId; }
  
  protected:
    tuid mExistingEntryId;

    DuplicateEntryException();  // hide the default c_tor - it's pretty useless    
  };

  class DuplicatePatchEntryException : public DuplicateEntryException
  {
  public:
    DuplicatePatchEntryException( const tuid id, const char* path ) : DuplicateEntryException( id, path, "Patching failed while attempting to apply patch which contained a duplicate file path: %s" ) {}
  
  protected:
    DuplicatePatchEntryException();  // hide the default c_tor - it's pretty useless
  };

  class NotInManagedAssetsRootException : public Exception
  {
  public:
    NotInManagedAssetsRootException( const char* path, const char* assetRoot )
      : Exception( "Cannot add the file %s outside of the asset branch: %s", path, assetRoot )
    {
    }

    NotInManagedAssetsRootException( const char* path, const char* targetPath, const char* assetRoot )
      : Exception( "Cannot move or copy the file %s to %s; the target path is outside of the asset branch: %s", path, targetPath, assetRoot )
    {
    }
  };

  class NoTransOpenException : public Exception
  {
  public:
    NoTransOpenException() : Exception( "There is no open transaction to commit!"  ) {}
  };

  class MaxOpenTransException : public Exception
  {
  public:
    MaxOpenTransException() : Exception( "Maximum number of transactions exceeded."  ) {}
  };

  class MissingFileException : public Exception
  {
  public:
    MissingFileException( const tuid& id, const std::string& referenceType = "", const std::string& referencingFile = "", const std::string& hint = "" )
    {
      std::string message = std::string( "Unknown " ) + ( referenceType.empty() ? "file" : referenceType ) + " id: "TUID_HEX_FORMAT".";
      
      if ( !referencingFile.empty() )
      {
        message += std::string( "  The file '" ) + referencingFile + "' references an unknown file id, listed above.";

        RCS::File rcsFile( referencingFile );
        std::string username;
        rcsFile.GetLastModifiedByUser( username );
        message += std::string( "  The last user to edit '") + referencingFile + "' was: " + username + ".";
      }

      if ( !hint.empty() )
      {
        message += "  ";
        message += hint;
      }

      SetMessage( message.c_str(), id );
    }
  };
}