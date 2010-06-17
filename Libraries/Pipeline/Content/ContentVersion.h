#pragma once

#include "Pipeline/API.h"
#include "Reflect/Version.h"
#include "Reflect/Serializers.h"

namespace Content
{
  //
  // Version information for scene classes
  //

  class PIPELINE_API ContentVersion : public Reflect::Version
  {
  public:
    std::string m_ContentVersion;
    static const char* CONTENT_VERSION;

    REFLECT_DECLARE_CLASS(ContentVersion, Reflect::Version)

    static void EnumerateClass( Reflect::Compositor<ContentVersion>& comp );

    ContentVersion();

    ContentVersion(const char* source, const char* sourceVersion);

    virtual bool IsCurrent();
  };

  typedef Nocturnal::SmartPtr<ContentVersion> ContentVersionPtr;
  typedef std::vector<ContentVersionPtr> V_ContentVersion;
}