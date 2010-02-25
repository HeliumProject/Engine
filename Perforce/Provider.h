#pragma once

#include "API.h"
#include "RCS/RCS.h"
#include "RCS/Provider.h"
#include "Common/Compiler.h"
#include "Common/Automation/Event.h"

class ClientApi;

namespace Platform
{
  class Mutex;
}

namespace Perforce
{
  class PERFORCE_API WaitInterface
  {
  public:
    virtual ~WaitInterface();
    virtual bool StopWaiting() = 0;
  };

  class PERFORCE_API Provider : public RCS::Provider
  {
  public:
    Provider();
    ~Provider();

    //
    // RCS::Provider
    //

    virtual bool IsEnabled() NOC_OVERRIDE;
    virtual void SetEnabled( bool online ) NOC_OVERRIDE;

    virtual const char* GetName() NOC_OVERRIDE;

    virtual void Sync( RCS::File& file, const u64 timestamp = 0 ) NOC_OVERRIDE;

    virtual void GetInfo( RCS::File& file, const RCS::GetInfoFlag flags = RCS::GetInfoFlags::Default ) NOC_OVERRIDE;
    virtual void GetInfo( const std::string& folder, RCS::V_File& files, bool recursive = false, u32 fileData = RCS::FileData::All, u32 actionData = RCS::ActionData::All ) NOC_OVERRIDE;

    virtual void Add( RCS::File& file ) NOC_OVERRIDE;
    virtual void Edit( RCS::File& file ) NOC_OVERRIDE;
    virtual void Delete( RCS::File& file ) NOC_OVERRIDE;

    virtual void GetOpenedFiles( RCS::V_File& file ) NOC_OVERRIDE;

    virtual void Reopen( RCS::File& file ) NOC_OVERRIDE;

    virtual void Rename( RCS::File& source, RCS::File& dest ) NOC_OVERRIDE;
    virtual void Integrate( RCS::File& source, RCS::File& dest ) NOC_OVERRIDE;

    virtual void Revert( RCS::Changeset& changeset, bool revertUnchangedOnly = false ) NOC_OVERRIDE;
    virtual void Revert( RCS::File& file, bool revertUnchangedOnly = false ) NOC_OVERRIDE;

    virtual void Commit( RCS::Changeset& changeset ) NOC_OVERRIDE;

    virtual void CreateChangeset( RCS::Changeset& changeset ) NOC_OVERRIDE;
    virtual void GetChangesets( RCS::V_Changeset& changesets ) NOC_OVERRIDE;

  public:
    bool                  m_Enabled;
    bool                  m_Connected;
    ClientApi*            m_Client;
    Platform::Mutex*      m_Mutex;
    std::string           m_UserName;
    std::string           m_ClientName;
  };
}