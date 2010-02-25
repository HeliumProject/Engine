#include "stdafx.h"
#include "ClipboardDataWrapper.h"

using namespace Inspect;

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
  Reflect::RegisterClass< ClipboardDataWrapper >( "ClipboardDataWrapper" );
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