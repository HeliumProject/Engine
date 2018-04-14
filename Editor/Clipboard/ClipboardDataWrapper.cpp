#include "EditorPch.h"
#include "ClipboardDataWrapper.h"

HELIUM_DEFINE_CLASS( Helium::Editor::ClipboardDataWrapper );

using namespace Helium;
using namespace Helium::Editor;

void ClipboardDataWrapper::PopulateMetaType( Reflect::MetaStruct& comp )
{
    comp.AddField( &ClipboardDataWrapper::m_Data, "m_Data" );
}

ClipboardDataWrapper::ClipboardDataWrapper()
{
}

ClipboardDataWrapper::~ClipboardDataWrapper()
{
}