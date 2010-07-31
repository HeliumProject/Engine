#include "ClipboardDataWrapper.h"

using namespace Helium;
using namespace Helium::Inspect;

REFLECT_DEFINE_CLASS( ClipboardDataWrapper );

void ClipboardDataWrapper::EnumerateClass( Reflect::Compositor<ClipboardDataWrapper>& comp )
{
  Reflect::ElementField* elemData = comp.AddField( &ClipboardDataWrapper::m_Data, "m_Data" );
}


///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void ClipboardDataWrapper::InitializeType()
{
  Reflect::RegisterClass< ClipboardDataWrapper >( TXT( "ClipboardDataWrapper" ) );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void ClipboardDataWrapper::CleanupType()
{
  Reflect::UnregisterClass< ClipboardDataWrapper >();
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ClipboardDataWrapper::ClipboardDataWrapper()
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ClipboardDataWrapper::~ClipboardDataWrapper()
{
}