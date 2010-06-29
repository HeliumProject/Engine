#include "Precompile.h"
#include "Tool.h"

using namespace Luna;

LUNA_DEFINE_TYPE(Luna::Tool);

void Tool::InitializeType()
{
  Reflect::RegisterClass< Luna::Tool >( TXT( "Luna::Tool" ) );
}

void Tool::CleanupType()
{
  Reflect::UnregisterClass< Luna::Tool >();
}