#pragma once

#include "Command.h"

namespace Perforce
{
  class SyncCommand : public Command
  {
  public:
    SyncCommand( Provider* provider, RCS::File* file = NULL, const u64 timestamp = 0 )
      : Command ( provider, "sync" )
      , m_File ( file )
      , m_SyncTime( timestamp )
    {
    }

    virtual void OutputStat( StrDict* dict ) NOC_OVERRIDE;
    virtual void HandleError( Error* error ) NOC_OVERRIDE;
    virtual void Run() NOC_OVERRIDE;

  protected:
    RCS::File* m_File;
    u64 m_SyncTime;
  };

  class OpenCommand : public Command
  {
  public:
    OpenCommand( Provider* provider, const char* command = "", RCS::File* file = NULL )
      : Command ( provider, command )
      , m_File ( file )
    {

    }

    virtual void OutputStat( StrDict* dict );
    virtual void Run() NOC_OVERRIDE;

  protected:
    RCS::File* m_File;
  };

  class IntegrateCommand : public Command
  {
  public:
    IntegrateCommand( Provider* provider, RCS::File* source, RCS::File* dest )
      : Command( provider, "integrate" )
      , m_Source( source )
      , m_Dest( dest )
    {
    }

    virtual void OutputStat( StrDict* dict ) NOC_OVERRIDE;
    virtual void Run() NOC_OVERRIDE;

  protected:
    RCS::File* m_Source;
    RCS::File* m_Dest;
  };
}
