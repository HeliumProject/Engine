#pragma once

#include "BuildInfo.h"

namespace Luna
{
  typedef std::map< std::string, BuildInfoPtr > M_Builds;

  class BuildServer
  {
  private:
    std::string m_BuildLocation;
    M_Builds m_Builds;

  public:
    BuildServer();
    virtual ~BuildServer();

    const BuildInfo* FindBuild( const std::string& name ) const;
    const M_Builds& GetBuilds() const;

    void RefreshBuilds();
  };
}
