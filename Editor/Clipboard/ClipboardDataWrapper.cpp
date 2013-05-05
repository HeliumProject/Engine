#include "EditorPch.h"
#include "ClipboardDataWrapper.h"

REFLECT_DEFINE_OBJECT( Helium::Editor::ClipboardDataWrapper );

using namespace Helium;
using namespace Helium::Editor;

void ClipboardDataWrapper::PopulateStructure( Reflect::Structure& comp )
{
    comp.AddField( &ClipboardDataWrapper::m_Data, TXT( "m_Data" ) );
}

ClipboardDataWrapper::ClipboardDataWrapper()
{
}

ClipboardDataWrapper::~ClipboardDataWrapper()
{
}