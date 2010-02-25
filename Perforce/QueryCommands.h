#pragma once

#include "Command.h"

namespace Perforce
{
  class WhereCommand : public Command
  {
  public:
    WhereCommand( Provider* provider, RCS::File* File )
      : Command ( provider, "where" )
      , m_File( File )
    {

    }

    virtual void Run();
    virtual void OutputStat( StrDict *dict );
    
    RCS::File* m_File;
  };

  class HaveCommand : public Command
  {
  public:
    HaveCommand( Provider* provider )
      : Command ( provider, "have" )
    {

    }

    virtual void OutputStat( StrDict *dict );

  };

  class DirsCommand : public Command
  {
  public:
    DirsCommand( Provider* provider )
      : Command ( provider, "dirs" )
    {

    }

    virtual void OutputStat( StrDict *dict );

  };

  class FilesCommand : public Command
  {
  public:
    FilesCommand( Provider* provider )
      : Command ( provider, "files" )
    {

    }

    virtual void OutputStat( StrDict *dict );

  };

  class FStatCommand : public Command
  {
  public:
    FStatCommand( Provider* provider, const char* command = "", RCS::File* File = NULL )
      : Command( provider, command )
      , m_File ( File )
    {
      AddArg( "-Ol" );
    }

    virtual void OutputStat( StrDict *dict );
    virtual void HandleError( Error* error );

  protected:
    RCS::File* m_File;
  };

  class SingleFStatCommand : public FStatCommand
  {
  public:
    SingleFStatCommand( Provider* provider, RCS::File* File )
      : FStatCommand ( provider, "fstat", File )
    {
    }

    virtual void Run();
    virtual void OutputStat( StrDict *dict ) NOC_OVERRIDE;
  };

  class MultiFStatCommand : public FStatCommand
  {
  public:
    MultiFStatCommand( Provider* provider, const std::string& folder, RCS::V_File* files, bool recursive, u32 fileData = RCS::FileData::All, u32 actionData = RCS::ActionData::All )
      : FStatCommand ( provider, "fstat", NULL )
      , m_Folder ( folder )
      , m_Files ( files )
      , m_Recursive ( recursive )
      , m_FileData ( fileData )
      , m_ActionData ( actionData )
    {

    }

    virtual void Run();
    virtual void OutputStat( StrDict *dict ) NOC_OVERRIDE;

  protected:
    std::string m_Folder;
    RCS::V_File* m_Files;
    bool m_Recursive;
    u32 m_FileData;
    u32 m_ActionData;
  };

  class FileLogCommand : public Command
  {
  public:
    FileLogCommand( Provider* provider, RCS::File* File, bool getIntegrationHistory = false )
      : Command ( provider, "filelog" )
      , m_File( File )
    {
      if ( getIntegrationHistory )
      {
        AddArg( "-i" );
      }
    }

    virtual void Run();
    virtual void OutputStat( StrDict *dict );
    virtual void HandleError( Error* error );

    RCS::File* m_File;
  };
}