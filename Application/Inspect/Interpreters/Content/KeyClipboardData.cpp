#include "KeyClipboardData.h"
#include "Foundation/Reflect/ElementArraySerializer.h"

using namespace Helium;
using namespace Helium::Inspect;

REFLECT_DEFINE_CLASS( KeyClipboardData )

void KeyClipboardData::EnumerateClass( Reflect::Compositor< KeyClipboardData >& comp )
{
  Reflect::Field* fieldKeys = comp.AddField( &KeyClipboardData::m_Keys, "m_Keys" );
}

KeyClipboardData::KeyClipboardData()
{
}
