#pragma once

#include "P4Command.h"

namespace Perforce
{
  class SyncCommand : public Command
  {
  public:
    SyncCommand( Provider* provider, RCS::File* file = NULL, const u64 timestamp = 0 )
      : Command ( provider, TXT( "sync" ) )
      , m_File ( file )
      , m_SyncTime( timestamp )
    {
    }

    virtual void OutputStat( StrDict* dict ) HELIUM_OVERRIDE;
    virtual void HandleError( Error* error ) HELIUM_OVERRIDE;
    virtual void Run() HELIUM_OVERRIDE;

  protected:
    RCS::File* m_File;
    u64 m_SyncTime;
  };

  class OpenCommand : public Command
  {
  public:
    OpenCommand( Provider* provider, const tchar* command = TXT( "" ), RCS::File* file = NULL )
      : Command ( provider, command )
      , m_File ( file )
    {

    }

    virtual void OutputStat( StrDict* dict );
    virtual void Run() HELIUM_OVERRIDE;

  protected:
    RCS::File* m_File;
  };

  class IntegrateCommand : public Command
  {
  public:
    IntegrateCommand( Provider* provider, RCS::File* source, RCS::File* dest )
      : Command( provider, TXT( "integrate" ) )
      , m_Source( source )
      , m_Dest( dest )
    {
    }

    virtual void OutputStat( StrDict* dict ) HELIUM_OVERRIDE;
    virtual void Run() HELIUM_OVERRIDE;

  protected:
    RCS::File* m_Source;
    RCS::File* m_Dest;
  };
}
