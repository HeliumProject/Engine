#pragma once

#include "API.h"
#include "Reflect/Version.h"
#include "Reflect/Serializers.h"

namespace Luna
{
  //
  // Version information for scene classes
  //

  class LUNA_EDITOR_API SessionVersion : public Reflect::Version
  {
  public:
    std::string m_SessionVersion;
    static const char* SESSION_VERSION;

    REFLECT_DECLARE_CLASS(SessionVersion, Reflect::Version)

    static void EnumerateClass( Reflect::Compositor<SessionVersion>& comp );

    SessionVersion();
  };

  typedef Nocturnal::SmartPtr<SessionVersion> SessionVersionPtr;
  typedef std::vector<SessionVersionPtr> V_SessionVersion;
}