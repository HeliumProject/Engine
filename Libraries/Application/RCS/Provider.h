#pragma once

#include "Application/API.h"
#include "RCSFile.h"
#include "Changeset.h"

namespace RCS
{
  //
  // Abstract base class for a revision control provider
  //

  class APPLICATION_API Provider
  {
  public:
    Provider();
    virtual ~Provider();

    virtual bool IsEnabled() = 0;
    virtual void SetEnabled( bool enabled ) = 0;

    virtual const char* GetName() = 0;

    virtual void Sync( RCS::File& file, const u64 timestamp = 0 ) = 0;
    
    virtual void GetInfo( RCS::File& file, const GetInfoFlag flags = GetInfoFlags::Default ) = 0;
    virtual void GetInfo( const std::string& folder, RCS::V_File& files, bool recursive = false, u32 fileData = FileData::All, u32 actionData = ActionData::All ) = 0;

    virtual void Add( RCS::File& file ) = 0;
    virtual void Edit( RCS::File& file ) = 0;
    virtual void Delete( RCS::File& file ) = 0;

    virtual void GetOpenedFiles( RCS::V_File& files ) = 0;

    virtual void Reopen( RCS::File& file ) = 0;

    virtual void Rename( RCS::File& source, RCS::File& dest ) = 0;
    virtual void Integrate( RCS::File& source, RCS::File& dest ) = 0;

    virtual void Revert( RCS::Changeset& changeset, bool revertUnchangedOnly = false ) = 0;
    virtual void Revert( RCS::File& file, bool revertUnchangedOnly = false ) = 0;

    virtual void Commit( RCS::Changeset& changeset ) = 0;

    virtual void CreateChangeset( RCS::Changeset& changeset ) = 0;
    virtual void GetChangesets( RCS::V_Changeset& changesets ) = 0;

    /*
    virtual void GetChangeset( RCS::Changeset& changeset ) = 0;
    virtual void DeleteChangeset( RCS::Changeset& changeset ) = 0;
    */
  };


}