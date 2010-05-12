#include "stdafx.h"
#include "KeyClipboardData.h"
#include "Reflect/ElementArraySerializer.h"

using namespace Inspect;

REFLECT_DEFINE_CLASS( KeyClipboardData )

void KeyClipboardData::EnumerateClass( Reflect::Compositor< KeyClipboardData >& comp )
{
  Reflect::Field* fieldKeys = comp.AddField( &KeyClipboardData::m_Keys, "m_Keys" );
}

KeyClipboardData::KeyClipboardData()
{
}
