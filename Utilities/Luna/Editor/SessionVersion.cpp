#include "Precompile.h"
#include "SessionVersion.h"

const char* Luna::SessionVersion::SESSION_VERSION = "2";

using namespace Luna;

REFLECT_DEFINE_CLASS(SessionVersion)

void SessionVersion::EnumerateClass( Reflect::Compositor<SessionVersion>& comp )
{
  Reflect::Field* fieldSessionVersion = comp.AddField( &SessionVersion::m_SessionVersion, "m_SessionVersion", Reflect::FieldFlags::Force );
}


Luna::SessionVersion::SessionVersion()
{
  m_SessionVersion = SESSION_VERSION;
}