#pragma once

#include "Common/Types.h"
#include "Common/Memory/SmartPtr.h"

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // Description of a level on the build server.
  // 
  class LevelInfo : public Nocturnal::RefCountBase< LevelInfo >
  {
  public:
    std::string m_Name;
    bool m_HasServerData;

    std::string GetStrippedName() const;
  };
  typedef Nocturnal::SmartPtr< LevelInfo > LevelInfoPtr;
  typedef std::vector< LevelInfoPtr > V_LevelInfo;

  /////////////////////////////////////////////////////////////////////////////
  // Description of a build on the build server
  // 
  class BuildInfo : public Nocturnal::RefCountBase< BuildInfo >
  {
  public:
    std::string m_Name;
    __time64_t m_Timestamp;
    bool m_CodeSucceeded;
    bool m_SymbolsSucceeded; // "xml" in build server lingo
    std::string m_Template;
    V_LevelInfo m_Levels;

  public:
    BuildInfo( const std::string& name = "" ); 
    virtual ~BuildInfo();

    bool Succeeded() const;
  };
  typedef Nocturnal::SmartPtr< BuildInfo > BuildInfoPtr;
  typedef std::vector< BuildInfoPtr > V_BuildInfoPtr;
}
