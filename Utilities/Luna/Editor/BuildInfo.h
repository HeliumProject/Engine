#pragma once

#include <vector>

#include "Platform/Types.h"
#include "Foundation/Memory/SmartPtr.h"

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // Description of a level on the build server.
  // 
  class LevelInfo : public Nocturnal::RefCountBase< LevelInfo >
  {
  public:
    tstring m_Name;
    bool m_HasServerData;

    tstring GetStrippedName() const;
  };
  typedef Nocturnal::SmartPtr< LevelInfo > LevelInfoPtr;
  typedef std::vector< LevelInfoPtr > V_LevelInfo;

  /////////////////////////////////////////////////////////////////////////////
  // Description of a build on the build server
  // 
  class BuildInfo : public Nocturnal::RefCountBase< BuildInfo >
  {
  public:
    tstring m_Name;
    __time64_t m_Timestamp;
    bool m_CodeSucceeded;
    bool m_SymbolsSucceeded; // "xml" in build server lingo
    tstring m_Template;
    V_LevelInfo m_Levels;

  public:
    BuildInfo( const tstring& name = TXT( "" ) ); 
    virtual ~BuildInfo();

    bool Succeeded() const;
  };
  typedef Nocturnal::SmartPtr< BuildInfo > BuildInfoPtr;
  typedef std::vector< BuildInfoPtr > V_BuildInfoPtr;
}
