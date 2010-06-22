#include "ContentVersion.h"

const char* Content::ContentVersion::CONTENT_VERSION = "5";

using namespace Reflect;
using namespace Content;

REFLECT_DEFINE_CLASS(ContentVersion)

void ContentVersion::EnumerateClass( Reflect::Compositor<ContentVersion>& comp )
{
  Reflect::Field* fieldContentVersion = comp.AddField( &ContentVersion::m_ContentVersion, "m_ContentVersion", Reflect::FieldFlags::Force );
}


Content::ContentVersion::ContentVersion()
{
  m_ContentVersion = CONTENT_VERSION;
}

Content::ContentVersion::ContentVersion(const char* source, const char* sourceVersion)
: Version (source, sourceVersion)
{
  m_ContentVersion = CONTENT_VERSION;
}

bool Content::ContentVersion::IsCurrent()
{
  return __super::IsCurrent() && m_ContentVersion == CONTENT_VERSION;
}