#include "Precompile.h"
#include "ClipboardDataWrapper.h"

using namespace Helium;
using namespace Helium::Editor;

REFLECT_DEFINE_OBJECT( ClipboardDataWrapper );

void ClipboardDataWrapper::AcceptCompositeVisitor( Reflect::Composite& comp )
{
    comp.AddField( &ClipboardDataWrapper::m_Data, TXT( "m_Data" ) );
}

ClipboardDataWrapper::ClipboardDataWrapper()
{
}

ClipboardDataWrapper::~ClipboardDataWrapper()
{
}