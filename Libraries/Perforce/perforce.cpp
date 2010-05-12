#include "Perforce.h"
#include "Provider.h"

#include "Windows/Windows.h"

#define MAX_ERR_LENGTH 200

using namespace Perforce;

u32 Perforce::g_InitThread = GetCurrentThreadId();

WaitSignature::Delegate Perforce::g_ShowWaitDialog;
MessageSignature::Delegate Perforce::g_ShowWarningDialog;

u32 g_InitCount = 0;
Provider g_Provider;

void Perforce::Initialize()
{
  if ( ++g_InitCount == 1 )
  {
    RCS::SetProvider( &g_Provider );
  }
}

void Perforce::Cleanup()
{
  if ( --g_InitCount == 0 )
  {
    RCS::SetProvider( NULL );
  }
}

WaitInterface::~WaitInterface()
{

}
