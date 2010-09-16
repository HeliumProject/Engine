#include "Precompile.h"
#include "ReflectClipboardData.h"

using namespace Helium;
using namespace Helium::Editor;

REFLECT_DEFINE_ABSTRACT( ReflectClipboardData );

void ReflectClipboardData::EnumerateClass( Reflect::Compositor<ReflectClipboardData>& comp )
{
}

ReflectClipboardData::ReflectClipboardData()
{
}

ReflectClipboardData::~ReflectClipboardData()
{
}